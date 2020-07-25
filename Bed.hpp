#pragma once
#include "Patient.hpp"


extern std::mutex refresh_mtx;
class Patient;

class Bed{
    private:
    const int y_max = getmaxy(stdscr);
    const int x_max = getmaxx(stdscr);
    const int win_height = y_max/8;
    const int win_width = x_max/16;
    int id;
    std::atomic_bool is_occupied;
    WINDOW* window;

    public:
    std::mutex mtx;
    std::mutex discharge_mtx;
    std::condition_variable cv;
    int patient_id;

    Bed();

    void init(int);

    void draw();

    void assign_patient(const Patient&);

    void remove_patient();

    bool get_is_occupied();

    void set_is_occupied(bool);
};