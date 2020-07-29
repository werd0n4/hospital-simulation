#include "Bed.hpp"

extern std::mutex refresh_mtx;

Bed::Bed(int _id) : id(_id){
    is_occupied = false;
    window = newwin(win_height, win_width, id < 5 ? 0 : win_height, id%5*win_width + 3./5*x_max);
    draw();
}

// void Bed::init(int _id){
//     id = _id;
//     window = newwin(win_height, win_width, id < 5 ? 0 : win_height, id%5*win_width + 3./5*x_max);
//     draw();
// }

void Bed::draw(){
    werase(window);
    wattron(window, COLOR_PAIR(green));
    box(window, 0, 0);
    wattroff(window, COLOR_PAIR(green));
    mvwprintw(window, 1, win_width/2 - 3, "Bed %d", id);
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(window);
    }
}

void Bed::assign_patient(int id){
    // is_occupied = true;
    patient_id = id;
    mvwprintw(window, 3, 3, "%d", patient_id);
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(window);
    }
}

void Bed::remove_patient(){
    is_occupied = false;
    patient_id = -1;
    draw();
}
