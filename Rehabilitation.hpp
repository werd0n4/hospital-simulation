#pragma once
#include "Patient.hpp"


extern bool running;
extern std::mutex refresh_mtx;
class Patient;

struct RehabRoom{
    bool is_occupied;
    int id;
    WINDOW* window;

    RehabRoom(int _id, int win_width, double y, double x) : is_occupied(false), id(_id) {
        window = newwin(1, win_width, y + id, x);
    }
};

class Rehabilitation{
    private:
    const int y_max = getmaxy(stdscr);
    const int x_max = getmaxx(stdscr);
    const int title_win_height = 3;
    const int win_width = x_max/3;
    const int patients_limit = 5;
    WINDOW* title_window;
    std::vector<RehabRoom> rehab_rooms;

    public:
    std::mutex change_room_status_mtx;
    std::mutex waiting_for_room_mtx;
    std::condition_variable cv;

    Rehabilitation();

    void display_patient_progress(WINDOW*, const int);

    void rehab_patient(Patient&, const int);

    void remove_patient(std::vector<RehabRoom>::iterator&);
};