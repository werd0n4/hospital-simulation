#pragma once
#include "Bed.hpp"

extern std::mutex refresh_mtx;

class Bed;

class Reception
{
    private:
    WINDOW* window;
    const int y_max = getmaxy(stdscr);
    const int x_max = getmaxx(stdscr);
    const int win_height = y_max/6;
    const int win_width = x_max/5;
    int registration_time;
    std::atomic_bool is_occupied;
    std::vector<Bed>& beds;

    public:
    std::mutex registration_mtx;
    std::mutex bed_status_mtx;
    std::mutex waiting_for_bed_mtx;
    std::mutex discharge_patient_mtx;
    std::condition_variable cv;
    std::condition_variable discharge_cv;

    Reception(std::vector<Bed>&);

    void draw_window();

    void register_patient(Patient&);

    void discharge_patient(const Patient&);

    bool get_is_occupied();

    void set_is_occupied(bool);
};