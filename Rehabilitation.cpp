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
    std::lock_guard<std::mutex> lg(refresh_mtx);
    for(int i = 0; i < patients.size(); ++i){
        mvwprintw(window, 2 + i, 1, "%d: ", patients[i].id);
    }
    wrefresh(window);
}

void Rehabilitation::add_patient(Patient _patient){
    patients.push_back(_patient);
    display_patients_list();
}

void Rehabilitation::display_patient_progress(Patient& patient, int time){
        time = time / (win_width-2);
        std::vector<Patient>::iterator it = std::find(patients.begin(), patients.end(), patient);
        int index = std::distance(patients.begin(), it);
        mvwprintw(window, 2 + index, 4, "|", patients[index].id);
        mvwprintw(window, 2 + index, win_width - 2, "|", patients[index].id);
        for(int i = 1; i <= (win_width-7); ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(time));
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                mvwprintw(window, 2+index, i+4, "=");
                wrefresh(window);
            }
        }
}
