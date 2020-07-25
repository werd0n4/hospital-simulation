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
    bool room_found = false;
    int room_id;
    //find empty examination room
    change_status("Waiting for exam room");
    while(!room_found){
        for(auto& exam : exams){
            {
                std::lock_guard<std::mutex> lg(exam.pat_mtx);
                if(!exam.is_doctor_in.load()){
                    exam.is_doctor_in.store(true);
                    exam.doctor_id = id;
                    room_id = exam.id;
                    room_found = true;
                    exam.print_info_about_sim();
                    break;
                }
            }
        } 
    }

    for(int i = 0; i < exam_quantity; ++i){
        change_status("Waiting for patient in "+std::to_string(room_id));
        std::unique_lock<std::mutex> ul(exams[room_id].doc_mtx);
        exams[room_id].cv.wait(ul, [this, room_id]{return exams[room_id].is_patient_in.load();});

        change_status("Examing patient");
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
        
        exams[room_id].is_exam_finished.store(true);
        exams[room_id].cv.notify_all();
        exams[room_id].print_info_about_sim();
    }
    exams[room_id].is_doctor_in.store(false);
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
