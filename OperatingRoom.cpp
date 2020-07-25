#include "OperatingRoom.hpp"

OperatingRoom::OperatingRoom(){
    window = newwin(win_height, win_width, 0, 0);

    is_patient_in = false;
    is_doctor_in = false;
    is_surgery_finished = false;

    print_info_about_sim();
}

void OperatingRoom::clear_window(){
    werase(window);
    wattron(window, COLOR_PAIR(green));
    box(window, 0, 0);
    wattroff(window, COLOR_PAIR(green));

    std::lock_guard<std::mutex> lg(refresh_mtx);
    wrefresh(window);
}

void OperatingRoom::print_info_about_sim(){
    clear_window();
    mvwprintw(window, 1, win_width/2 - 8, "Operating room");
    if(is_doctor_in){
        mvwprintw(window, 3, 1, "Doctor: %d", doctor_id);
    }
    else{
        mvwprintw(window, 3, 1, "Doctor: None");
    }

    if(is_patient_in){
        mvwprintw(window, 4, 1, "Patient: %d", patient_id);
    }
    else{
        mvwprintw(window, 4, 1, "Patient: None");
    }

    std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
    wrefresh(window);
}
