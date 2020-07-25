#include "Bed.hpp"

extern std::mutex refresh_mtx;

Bed::Bed(){
    isOccupied = false;
    getmaxyx(stdscr, y_max, x_max);
    win_height = y_max/8;
    win_width = x_max/16;
}

void Bed::init(int _id){
    id = _id;
    window = newwin(win_height, win_width, id < 5 ? 0 : win_height, id%5*win_width + 3./5*x_max);
    draw();
}

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

void Bed::assign_patient(Patient patient){
    isOccupied = true;
    patient_id = patient.id;
    // patient_ptr = std::make_unique<Patient>(patient);
    // mvwprintw(window, 3, 3, "%d", patient_ptr->id);
    mvwprintw(window, 3, 3, "%d", patient_id);
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(window);
    }
}

void Bed::remove_patient(){
    isOccupied = false;
    patient_id = -1;
    // patient_ptr.reset(nullptr);
    draw();
}

// bool Bed::check_if_its_patient_bed(Patient patient){
//     if(patient_ptr.get() == &patient)
//         return true;
//     else
//         return false;
// }

bool Bed::getIsOccupied(){
    return isOccupied;
}

void Bed::setIsOccupied(bool x){
    isOccupied = x;
}