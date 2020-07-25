#include "Reception.hpp"

Reception::Reception(std::vector<Bed>& _beds) : beds(_beds) {
    isOccupied = false;
    registration_time = 1000;

    getmaxyx(stdscr, y_max, x_max);
    win_height = y_max/6;
    win_width = x_max/5;
    window = newwin(win_height, win_width, win_height, x_max*0.4);

    draw();
}

void Reception::draw(){
    werase(window);
    wattron(window, COLOR_PAIR(green));
    box(window, 0, 0);
    wattroff(window, COLOR_PAIR(green));
    mvwprintw(window, 1, win_width/2 - 5, "Reception");
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(window);
    }
}

void Reception::register_patient(Patient patient){
    int time = registration_time + rand()%1001;
    time = time / (win_width-2);
    mvwprintw(window, 3, 3, "Registering patient nr %d", patient.id);
    mvwprintw(window, 4, 3, "Waiting for free bed...");
    {
        std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
        wrefresh(window);
    }

    bool bed_found = false;
    while(!bed_found){
        for(auto& bed : beds){
            std::lock_guard<std::mutex> lg(bed.mtx);
            if(!bed.get_is_occupied()){
                bed.assign_patient(patient);
                bed_found = true;
                break;
            }
        }
    }

    draw();
    mvwprintw(window, 3, 3, "Registering patient nr %d", patient.id);
    for(int i = 1; i <= win_width-2; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            mvwprintw(window, 4, i, "=");
            wrefresh(window);
        }
    }
    draw();
}

void Reception::discharge_patient(Patient patient){
    for(Bed& bed : beds){
        std::lock_guard<std::mutex> lg(bed.mtx);
        if(bed.patient_id == patient.id){
            bed.remove_patient();
        }
    }
}

bool Reception::getIsOccupied(){
    return isOccupied;
}

void Reception::setIsOccupied(bool x){
    isOccupied = x;
}