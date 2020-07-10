#include "Patient.hpp"


Patient::Patient(int _id, std::vector<Examination>& _exams, OperatingRoom& _operating_room, Rehabilitation& _rehab_room) 
        : id(_id), exams(_exams), operating_room(_operating_room), rehab_room(_rehab_room) 
{
    status = "Waiting for registration";

    getmaxyx(stdscr, y_max, x_max);
    win_height = 3;
    win_width = x_max/4;

    statusWindow = newwin(win_height, win_width, y_max*.6 + (id%7)*win_height, id/7*(win_width));

    draw();
}

void Patient::draw(){
    werase(statusWindow);
    wattron(statusWindow, COLOR_PAIR(6));
    box(statusWindow, 0, 0);
    wattroff(statusWindow, COLOR_PAIR(6));
    mvwprintw(statusWindow, 1, 1, "Patient %d: %s", id, status.c_str());
    {
        std::lock_guard<std::mutex>lg(refresh_mtx);
        wrefresh(statusWindow);
    }
}

void Patient::clear_status_window(){
    werase(statusWindow);
    wattron(statusWindow, COLOR_PAIR(6));
    box(statusWindow, 0, 0);
    wattroff(statusWindow, COLOR_PAIR(6));
    {
        std::lock_guard<std::mutex>lg(refresh_mtx);
        wrefresh(statusWindow);
    }
}

void Patient::changeStatus(std::string newStatus){
    status = newStatus;
    clear_status_window();
    mvwprintw(statusWindow, 1, 1, "Patient %d: %s", id, status.c_str());
    {
        std::lock_guard<std::mutex>lg(refresh_mtx);
        wrefresh(statusWindow);
    }
}

void Patient::registration(Reception& reception){
    std::unique_lock<std::mutex> ul(reception.mtx);
    reception.cv.wait(ul, [&reception]{return !reception.getIsOccupied();});//wait unitl reception is free
    reception.setIsOccupied(true);
    changeStatus("Registering");
    reception.registerPatient(id);

    changeStatus("Going for examination");
    reception.setIsOccupied(false);
    reception.cv.notify_one();
}

void Patient::go_for_exam(){
    bool room_found = false;
    int room_id;
    //find empty examination room
    changeStatus("Waiting for room");
    while(!room_found){
        for(auto& exam : exams){
            {
                std::lock_guard<std::mutex> lg(exam.pat_mtx);
                if(!exam.is_patient_in.load()){
                    exam.is_patient_in.store(true);
                    exam.cv.notify_one();
                    exam.patient_id = id;
                    room_id = exam.id;
                    room_found = true;
                    exam.print_info_about_sim();
                    break;
                }
            }
        } 
    }

    changeStatus("In room "+std::to_string(room_id));
    std::unique_lock<std::mutex> ul(exams[room_id].pat_mtx);
    exams[room_id].cv.wait(ul, [this, room_id]{return exams[room_id].is_exam_finished.load();});
    exams[room_id].is_exam_finished.store(false);
    exams[room_id].is_patient_in.store(false);
    exams[room_id].print_info_about_sim();
    changeStatus("End");
}

void Patient::undergo_surgery(){
    changeStatus("Waiting in queue for surgery");

    std::unique_lock<std::mutex> ul(operating_room.pat_mtx);
    operating_room.cv.wait(ul, [this]{return !operating_room.is_patient_in.load() && operating_room.is_doctor_in.load();});
    operating_room.is_patient_in.store(true);
    operating_room.patient_id = id;
    operating_room.cv.notify_one();
    operating_room.print_info_about_sim();

    changeStatus("Undergoing surgery");
    operating_room.cv.wait(ul, [this]{return operating_room.is_surgery_finished.load();});
    operating_room.is_patient_in.store(false);
    operating_room.is_surgery_finished.store(false);
    operating_room.print_info_about_sim();
    changeStatus("Surgery completed");
}

void Patient::rehabilitation(){
    time = 2000 + rand()%2001;
    rehab_room.add_patient(*this);
    rehab_room.display_patient_progress(*this, time);
}

void Patient::treatment(Reception& reception){
    registration(reception);
    go_for_exam();
    undergo_surgery();
    rehabilitation();
    //discharge();
}

bool Patient::operator==(const Patient& rhs)
{
    return this->id == rhs.id;
}