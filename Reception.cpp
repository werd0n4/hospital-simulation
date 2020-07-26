#include "Reception.hpp"

Reception::Reception(std::vector<Bed>& _beds) : beds(_beds) {
    is_occupied.store(false);
    registration_time = 1000;

    window = newwin(win_height, win_width, win_height, x_max*0.4);

    draw_window();
}

void Reception::draw_window(){
    werase(window);
    wattron(window, COLOR_PAIR(green));
    box(window, 0, 0);
    wattroff(window, COLOR_PAIR(green));
    mvwprintw(window, 1, win_width/2 - 5, "Reception");
    
    std::lock_guard<std::mutex> lg(refresh_mtx);
    wrefresh(window);
}

void Reception::register_patient(const Patient& patient){
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
        if(!bed_found){
            std::unique_lock<std::mutex> ul(release_bed_mtx);
            cv.wait(ul);
        }
    }

    draw_window();
    mvwprintw(window, 3, 3, "Registering patient nr %d", patient.id);
    for(int i = 1; i <= win_width-2; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            mvwprintw(window, 4, i, "=");
            wrefresh(window);
        }
    }
    draw_window();
}

void Reception::discharge_patient(const Patient& patient){
    for(Bed& bed : beds){
        std::lock_guard<std::mutex> lg(bed.mtx);
        if(bed.patient_id == patient.id){
            bed.remove_patient();
            cv.notify_one();
        }
    }
}

bool Reception::get_is_occupied(){
    return is_occupied;
}

void Reception::set_is_occupied(bool x){
    is_occupied.store(x);
}