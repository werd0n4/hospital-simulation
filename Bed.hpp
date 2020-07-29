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
    WINDOW* window;

    public:
    int id;
    // std::mutex mtx;
    // std::mutex discharge_mtx;
    // std::condition_variable cv;
    bool is_occupied;
    int patient_id;

    Bed(int);

    // void init(int);

    void draw();

    void assign_patient(int);

    void remove_patient();
};