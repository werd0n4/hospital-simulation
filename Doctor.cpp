#include "Doctor.hpp"

Doctor::Doctor(int _id, std::vector<Examination>& _exams, OperatingRoom& _operating_room) : id(_id), exams(_exams), operating_room(_operating_room) {
    status_window = newwin(win_height, win_width, id*win_height+y_max/6, 0);
    progres_window = newwin(win_height, win_width, id*win_height+y_max/6, win_width);
    status = "Idle";

    draw();
}

void Doctor::draw(){
    wattron(status_window, COLOR_PAIR(red));
    wattron(progres_window, COLOR_PAIR(red));
    box(status_window, 0, 0);
    box(progres_window, 0, 0);
    wattroff(status_window, COLOR_PAIR(red));
    wattroff(progres_window, COLOR_PAIR(red));
    mvwprintw(status_window, 1, 1, "Doc nr %d: %s", id, status.c_str());
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(status_window);
        wrefresh(progres_window);
    }
}

void Doctor::clear_progres_window(){
    werase(progres_window);
    wattron(progres_window, COLOR_PAIR(red));
    box(progres_window, 0, 0);
    wattroff(progres_window, COLOR_PAIR(red));
    wmove(progres_window, 1, 1);
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(progres_window);
    }
}

void Doctor::change_status(const std::string& new_status){
    werase(status_window);
    status = new_status;
    wattron(status_window, COLOR_PAIR(red));
    box(status_window, 0, 0);
    wattroff(status_window, COLOR_PAIR(red));
    mvwprintw(status_window, 1, 1, "Doc nr %d: %s", id, status.c_str());
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(status_window);
    }
}

void Doctor::preparing(){
    clear_progres_window();
    change_status("Preparing");
    time = 3000 + rand()%1001;
    time = time / (win_width-2);
    for(int i = 1; i <= win_width-2; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            mvwprintw(progres_window, 1, i, "=");
            wrefresh(progres_window);
        }
    }
}

void Doctor::examine(int exam_quantity){
    static std::mutex searching_mtx;
    static std::mutex waiting_mtx;
    static std::condition_variable cv;
    std::vector<Examination>::iterator found_room;

    //doctor is looking for free examination room
    change_status("Waiting for exam room");
    while(true){
        std::lock_guard<std::mutex> lg(searching_mtx);
        found_room = std::find_if(exams.begin(), exams.end(), [](Examination& exam){return !exam.is_doctor_in.load();});
        if(found_room == exams.end()){
            std::unique_lock<std::mutex> ul(waiting_mtx);
            cv.wait(ul);
        } else{
            found_room->is_doctor_in.store(true);
            found_room->doctor_id = id;
            found_room->print_info_about_sim();
            break;
        }
    }

    //examin simulation: in exam room there are doctor and patient
    for(int i = 0; i < exam_quantity; ++i){
        change_status("Waiting for patient in "+std::to_string(found_room->id));
        std::unique_lock<std::mutex> ul(found_room->doc_mtx);
        found_room->cv.wait(ul, [this, &found_room]{return found_room->is_patient_in.load();});

        change_status("Examing patient");
        clear_progres_window();
        time = 4000 + rand()%1001;
        time = time / (win_width-2);
        for(int i = 1; i <= win_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(time));
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                mvwprintw(progres_window, 1, i, "=");
                wrefresh(progres_window);
            }
        }
        
        found_room->is_exam_finished.store(true);
        found_room->cv.notify_all();
        found_room->print_info_about_sim();
    }

    found_room->is_doctor_in.store(false);
    cv.notify_all();
    change_status("End examinations");
}

void Doctor::operate(){
    change_status("Waiting for operating room");

    std::unique_lock<std::mutex> ul(operating_room.doc_mtx);
    operating_room.cv.wait(ul, [this]{return !operating_room.is_doctor_in.load();});
    operating_room.is_doctor_in.store(true);
    operating_room.doctor_id = id;
    operating_room.cv.notify_all();
    operating_room.print_info_about_sim();

    change_status("Waiting to begin surgery");
    operating_room.cv.wait(ul, [this]{return operating_room.is_patient_in.load();});

    change_status("Operating patient");
    clear_progres_window();
    time = 2000 + rand()%1001;
    time = time / (win_width-2);
    for(int i = 1; i <= win_width-2; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            mvwprintw(progres_window, 1, i, "=");
            wrefresh(progres_window);
        }
    }
    operating_room.is_surgery_finished.store(true);
    operating_room.is_doctor_in.store(false);
    operating_room.cv.notify_all();
    operating_room.print_info_about_sim();
    change_status("Surgery ended");
}

void Doctor::on_duty(){
    while(running){
        preparing();
        examine(1);
        operate();
    }
}
