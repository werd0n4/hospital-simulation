#include "Patient.hpp"

template<typename Room>
Room& find_empty_room(std::vector<Room>& rooms, std::mutex& change_room_status_mtx, std::mutex& waiting_for_room_mtx, std::condition_variable& cv, bool Room::* MemPtr);

Patient::Patient(int _id, std::vector<Examination>& _exams, OperatingRoom& _operating_room, Rehabilitation& _rehab_room, Reception& _reception) 
        : id(_id), exams(_exams), operating_room(_operating_room), rehab_room(_rehab_room), reception(_reception) 
{
    status = "Waiting for registration";

    status_window = newwin(win_height, win_width, y_max*.5 + (id%7)*win_height, id/7*(win_width));

    draw();
}

void Patient::draw(){
    werase(status_window);
    wattron(status_window, COLOR_PAIR(blue));
    box(status_window, 0, 0);
    wattroff(status_window, COLOR_PAIR(blue));
    mvwprintw(status_window, 1, 1, "Patient %d: %s", id, status.c_str());
    
    std::lock_guard<std::mutex>lg(refresh_mtx);
    wrefresh(status_window);
}

void Patient::clear_status_window(){
    werase(status_window);
    wattron(status_window, COLOR_PAIR(blue));
    box(status_window, 0, 0);
    wattroff(status_window, COLOR_PAIR(blue));
    
    std::lock_guard<std::mutex>lg(refresh_mtx);
    wrefresh(status_window);
}

void Patient::change_status(const std::string& new_status){
    status = new_status;
    clear_status_window();
    mvwprintw(status_window, 1, 1, "Patient %d: %s", id, status.c_str());
    
    std::lock_guard<std::mutex>lg(refresh_mtx);
    wrefresh(status_window);
}

void Patient::registration(){
    change_status("Wait in queue to register");
    std::unique_lock<std::mutex> ul(reception.registration_mtx);
    reception.cv.wait(ul, [this]{return !reception.get_is_occupied();});//wait unitl reception is free
    reception.set_is_occupied(true);
    change_status("Registering");
    reception.register_patient(*this);

    change_status("Going for examination");
    reception.set_is_occupied(false);
    reception.cv.notify_one();
}

void Patient::go_for_exam(){
    static std::mutex searching_mtx;
    static std::mutex waiting_mtx;
    static std::condition_variable cv;
    int room_id;
    
    //find empty examination room
    change_status("Waiting for room");
    Examination& room_found = find_empty_room(exams, searching_mtx, waiting_mtx, cv, &Examination::is_patient_in);
    room_found.cv.notify_all();
    room_found.patient_id = id;
    room_found.print_info_about_sim();

    change_status("In room "+std::to_string(room_id));
    std::unique_lock<std::mutex> ul(room_found.pat_mtx);
    room_found.cv.wait(ul, [&room_found, room_id]{return room_found.is_exam_finished.load();});
    room_found.is_exam_finished.store(false);
    room_found.is_patient_in = false;
    room_found.print_info_about_sim();
    change_status("End");
}

void Patient::undergo_surgery(){
    change_status("Waiting in queue for surgery");

    std::unique_lock<std::mutex> ul(operating_room.pat_mtx);
    operating_room.cv.wait(ul, [this]{return !operating_room.is_patient_in.load() && operating_room.is_doctor_in.load();});
    operating_room.is_patient_in.store(true);
    operating_room.cv.notify_all();
    operating_room.patient_id = id;
    operating_room.print_info_about_sim();

    change_status("Undergoing surgery");
    operating_room.cv.wait(ul, [this]{return operating_room.is_surgery_finished.load();});
    operating_room.is_patient_in.store(false);
    operating_room.is_surgery_finished.store(false);
    operating_room.print_info_about_sim();
    change_status("Surgery completed");
}

void Patient::rehabilitation(){
    time = 4000 + rand()%2001;
    rehab_room.rehab_patient(*this, time);
}

void Patient::discharge(){
    change_status("Discharging");
    reception.discharge_patient(*this);
    change_status("Out from hospital");
    std::this_thread::sleep_for(std::chrono::milliseconds(2500 + rand()%501));
}

void Patient::treatment(){
    while(running){
        registration();
        go_for_exam();
        undergo_surgery();
        rehabilitation();
        discharge();
    }
}

bool Patient::operator==(const Patient& rhs)
{
    return this->id == rhs.id;
}
