#include "Rehabilitation.hpp"


Rehabilitation::Rehabilitation(){
    getmaxyx(stdscr, y_max, x_max);
    win_height = y_max/4;
    win_width = x_max/3;
    window = newwin(win_height, win_width, y_max/4, 3./5*x_max);
    patients_limit = 5;
    patients_after_rehab = 0;
    empty_seats = true;

    clear_window();
}

void Rehabilitation::clear_window(){
    werase(window);
    wattron(window, COLOR_PAIR(yellow));
    box(window, 0, 0);
    wattroff(window, COLOR_PAIR(yellow));
    mvwprintw(window, 1, win_width/2 - 14, "Patients on rehabilitation:");
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(window);
    }
}

void Rehabilitation::display_patients_list(){
    // clear_window();
    std::lock_guard<std::mutex> lg(refresh_mtx);
    for(int i = 0; i < patients.size(); ++i){
        mvwprintw(window, 3 + i, 1, "%d: ", patients[i].id);
    }
    wrefresh(window);
}

void Rehabilitation::add_patient(Patient& _patient){
    std::unique_lock<std::mutex> ul(mtx);
    cv.wait(ul, [this]{return empty_seats.load();});
    patients.push_back(_patient);
    display_patients_list();
    if(patients.size() >= patients_limit)
        empty_seats = false;
}

void Rehabilitation::remove_patient(Patient& _patient){
    ++patients_after_rehab;
    // patients.pop_front();
    if(patients_after_rehab == patients_limit){
        patients.clear();
        patients_after_rehab = 0;
        clear_window();    
        display_patients_list();
        empty_seats.store(true);
        cv.notify_one();
    }
}

void Rehabilitation::display_patient_progress(Patient& patient, int time){
    int bar_width, bar_start_pos;
    time = time / (win_width-2);
    std::deque<Patient>::iterator it = std::find(patients.begin(), patients.end(), patient);
    int index = std::distance(patients.begin(), it);

    if(patients[index].id >= 10){
        bar_width = win_width - 8;
        bar_start_pos = 5;
    }
    else{
        bar_width = win_width - 7;
        bar_start_pos = 4;
    }
    mvwprintw(window, 3 + index, bar_start_pos, "|", patients[index].id);
    mvwprintw(window, 3 + index, win_width - 2, "|", patients[index].id);
    for(int i = 1; i <= bar_width; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            mvwprintw(window, 3+index, i+bar_start_pos, "=");
            wrefresh(window);
        }
    }
    remove_patient(patient);
}
