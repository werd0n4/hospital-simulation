#include "Doctor.hpp"

Doctor::Doctor(int _id, std::vector<Examination>& _exams, OperatingRoom& _operating_room) : id(_id), exams(_exams), operating_room(_operating_room) {
    getmaxyx(stdscr, y_max, x_max);
    win_height = 3;
    win_width = x_max/5;
    statusWindow = newwin(win_height, win_width, id*win_height+y_max/6, 0);
    progresWindow = newwin(win_height, win_width, id*win_height+y_max/6, win_width);
    status = "Idle";

    draw();
}

void Doctor::draw(){
    wattron(statusWindow, COLOR_PAIR(2));
    wattron(progresWindow, COLOR_PAIR(2));
    box(statusWindow, 0, 0);
    box(progresWindow, 0, 0);
    wattroff(statusWindow, COLOR_PAIR(2));
    wattroff(progresWindow, COLOR_PAIR(2));
    mvwprintw(statusWindow, 1, 1, "Doc nr %d: %s", id, status.c_str());
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(statusWindow);
        wrefresh(progresWindow);
    }
}

void Doctor::clear_progresWindow(){
    werase(progresWindow);
    wattron(progresWindow, COLOR_PAIR(2));
    box(progresWindow, 0, 0);
    wattroff(progresWindow, COLOR_PAIR(2));
    wmove(progresWindow, 1, 1);
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(progresWindow);
    }
}

void Doctor::changeStatus(std::string newStatus){
    werase(statusWindow);
    status = newStatus;
    wattron(statusWindow, COLOR_PAIR(2));
    box(statusWindow, 0, 0);
    wattroff(statusWindow, COLOR_PAIR(2));
    mvwprintw(statusWindow, 1, 1, "Doc nr %d: %s", id, status.c_str());
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(statusWindow);
    }
}

void Doctor::preparing(){
    changeStatus("Preparing");
    // time = 3000 + rand()%1001;
    time = 3000;
    time = time / (win_width-2);
    for(int i = 1; i <= win_width-2; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            mvwprintw(progresWindow, 1, i, "=");
            wrefresh(progresWindow);
        }
    }
}

void Doctor::examine(int exam_quantity){
    bool room_found = false;
    int room_id;
    //find empty examination room
    changeStatus("Waiting for exam room");
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
        changeStatus("Waiting for patient in "+std::to_string(room_id));
        std::unique_lock<std::mutex> ul(exams[room_id].doc_mtx);
        exams[room_id].cv.wait(ul, [this, room_id]{return exams[room_id].is_patient_in.load();});

        changeStatus("Examing patient");
        clear_progresWindow();
        // time = 2000 + rand()%1001;
        time = 2000;
        time = time / (win_width-2);
        for(int i = 1; i <= win_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(time));
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                mvwprintw(progresWindow, 1, i, "=");
                wrefresh(progresWindow);
            }
        }
        
        exams[room_id].is_exam_finished.store(true);
        exams[room_id].cv.notify_all();
        exams[room_id].print_info_about_sim();
    }
    exams[room_id].is_doctor_in.store(false);
    changeStatus("End examinations");
}

void Doctor::operate(){
    changeStatus("Waiting for operating room");

    std::unique_lock<std::mutex> ul(operating_room.doc_mtx);
    operating_room.cv.wait(ul, [this]{return !operating_room.is_doctor_in.load();});
    operating_room.is_doctor_in.store(true);
    operating_room.doctor_id = id;
    operating_room.cv.notify_all();
    operating_room.print_info_about_sim();

    changeStatus("Waiting to begin surgery");
    operating_room.cv.wait(ul, [this]{return operating_room.is_patient_in.load();});

    changeStatus("Operating patient");
    clear_progresWindow();
    // time = 2000 + rand()%1001;
    time = 2000;
    time = time / (win_width-2);
    for(int i = 1; i <= win_width-2; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            mvwprintw(progresWindow, 1, i, "=");
            wrefresh(progresWindow);
        }
    }
    operating_room.is_surgery_finished.store(true);
    operating_room.is_doctor_in.store(false);
    operating_room.cv.notify_all();
    operating_room.print_info_about_sim();
    changeStatus("Surgery ended");
}

void Doctor::on_duty(){
    preparing();
    examine(1);
    operate();
}
