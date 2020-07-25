#include "Examination.hpp"

Examination::Examination(){
    is_patient_in = false;
    is_doctor_in = false;
    is_exam_finished = false;
}

void Examination::init(int _id){
    id = _id;
    window = newwin(win_height, win_width, 0, (id+1)*win_width);
    print_info_about_sim();
}

void Examination::clear_window(){
    werase(window);
    wattron(window, COLOR_PAIR(green));
    box(window, 0, 0);
    wattroff(window, COLOR_PAIR(green));
    
    std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
    wrefresh(window);
}

void Examination::print_info_about_sim(){
    clear_window();
    mvwprintw(window, 1, win_width/2 - 7, "Examination %d", id);
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
