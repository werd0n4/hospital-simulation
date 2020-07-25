#pragma once
#include "Patient.hpp"


extern std::mutex refresh_mtx;
class Patient;

class Rehabilitation{
    private:
    const int y_max = getmaxy(stdscr);
    const int x_max = getmaxx(stdscr);
    const int win_height = y_max/4;
    const int win_width = x_max/3;
    const int patients_limit = 5;
    int patients_after_rehab;
    WINDOW* window;
    std::deque<Patient> patients;

    public:
    std::atomic_bool empty_seats;
    std::mutex mtx;
    std::condition_variable cv;

    Rehabilitation();

    void clear_window();

    void display_patients_list();

    void add_patient(const Patient&);

    void remove_patient(const Patient&);

    void display_patient_progress(const Patient&, int);
};