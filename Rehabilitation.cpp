#include "Rehabilitation.hpp"


Rehabilitation::Rehabilitation(){
    getmaxyx(stdscr, y_max, x_max);
    win_height = y_max/3;
    win_width = x_max/3;
    window = newwin(win_height, win_width, y_max/4, 3./5*x_max);

    clear_window();
}

void Rehabilitation::clear_window(){
    wattron(window, COLOR_PAIR(3));
    box(window, 0, 0);
    wattroff(window, COLOR_PAIR(3));
    mvwprintw(window, 1, win_width/2 - 14, "Patients on rehabilitation:");
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(window);
    }
}

void Rehabilitation::display_patients_list(){
    clear_window();
    for(int i = 0; i < patients.size(); ++i){
        mvwprintw(window, 2 + i, 1, "%d ");
    }

    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(window);
    }
}

void Rehabilitation::addPatient(Patient _patient){
    patients.push_back(_patient);
    display_patients_list();
}
