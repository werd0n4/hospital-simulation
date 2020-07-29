#include "Rehabilitation.hpp"

template<typename Room>
Room& find_empty_room(std::vector<Room>& rooms, std::mutex& change_room_status_mtx, std::mutex& waiting_for_room_mtx, std::condition_variable& cv);

Rehabilitation::Rehabilitation(){
    title_window = newwin(title_win_height, win_width, y_max/4-1, 3./5*x_max);
    mvwprintw(title_window, 1, win_width/2 - 14, "Patients on rehabilitation:");
    wattron(title_window, COLOR_PAIR(yellow));
    box(title_window, 0, 0);
    wattroff(title_window, COLOR_PAIR(yellow));
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(title_window);
    }

    for(int i = 0; i < patients_limit; ++i){
        rehab_rooms.push_back(RehabRoom{i, win_width, y_max/4. + 2 + i, 3./5*x_max});
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(rehab_rooms[i].window);
    }
}

void Rehabilitation::rehab_patient(Patient& patient, const int time){
    int index;

    patient.change_status("Waiting for rehabilitation");
    RehabRoom& room_found = find_empty_room(rehab_rooms, change_room_status_mtx, waiting_for_room_mtx, cv);

    wprintw(room_found.window, "%d. Patient %d:", room_found.id, patient.id);
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(room_found.window);
    }
    patient.change_status("Rehabilitating in room: " + std::to_string(room_found.id));
    display_patient_progress(room_found.window, time);
    remove_patient(room_found);
}

void Rehabilitation::display_patient_progress(WINDOW* window, const int time){
    int bar_units = win_width - 1 - 16;
    int ml_secs = time / bar_units;

    mvwprintw(window, 0, 16, "|");
    mvwprintw(window, 0, win_width - 1, "|");
    {
        std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
        wrefresh(window);
    }
    for(int i = 1; i < bar_units; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(ml_secs));
        mvwprintw(window, 0, 16 + i, "=");
        std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
        wrefresh(window);
    }
}

void Rehabilitation::remove_patient(RehabRoom& room_found){
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wclear(room_found.window);
        wrefresh(room_found.window);
    }
    
    std::lock_guard<std::mutex> lg(change_room_status_mtx);
    room_found.is_occupied = false;
    cv.notify_one();
}
