#pragma once
#include "Patient.hpp"


extern std::mutex refresh_mtx;
class Patient;

class Rehabilitation{
    private:
    int y_max, x_max, win_height, win_width;
    int patients_limit, patients_after_rehab;
    WINDOW* window;
    std::deque<Patient> patients;

    public:
    std::atomic_bool empty_seats;
    std::mutex mtx;
    std::condition_variable cv;

    Rehabilitation();

    void clear_window();

    void display_patients_list();

    void add_patient(Patient&);

    void remove_patient(Patient&);

    void display_patient_progress(Patient&, int);
};