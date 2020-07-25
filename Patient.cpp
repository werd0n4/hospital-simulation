#include "Patient.hpp"


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
    std::unique_lock<std::mutex> ul(reception.mtx);
    reception.cv.wait(ul, [this]{return !reception.get_is_occupied();});//wait unitl reception is free
    reception.set_is_occupied(true);
    change_status("Registering");
    reception.register_patient(*this);

    change_status("Going for examination");
    reception.set_is_occupied(false);
    reception.cv.notify_one();
}

void Patient::go_for_exam(){
    bool room_found = false;
    int room_id;
    //find empty examination room
    change_status("Waiting for room");
    while(!room_found){
        for(auto& exam : exams){
            {
                std::lock_guard<std::mutex> lg(exam.pat_mtx);
                if(!exam.is_patient_in.load()){
                    exam.is_patient_in.store(true);
                    exam.cv.notify_all();
                    exam.patient_id = id;
                    room_id = exam.id;
                    room_found = true;
                    exam.print_info_about_sim();
                    break;
                }
            }
        } 
    }

    change_status("In room "+std::to_string(room_id));
    std::unique_lock<std::mutex> ul(exams[room_id].pat_mtx);
    exams[room_id].cv.wait(ul, [this, room_id]{return exams[room_id].is_exam_finished.load();});
    exams[room_id].is_exam_finished.store(false);
    exams[room_id].is_patient_in.store(false);
    exams[room_id].print_info_about_sim();
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

    change_status("Waiting for rehabilitation");
    rehab_room.add_patient(*this);
    change_status("Rehabilitating");
    rehab_room.display_patient_progress(*this, time);
    rehab_room.cv.notify_one();
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
