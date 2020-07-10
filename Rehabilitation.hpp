#pragma once
#include "Patient.hpp"


extern std::mutex refresh_mtx;
class Patient;

class Rehabilitation{
    private:
    int y_max, x_max, win_height, win_width;
    WINDOW* window;
    std::vector<Patient> patients;

    public:
    Rehabilitation();

    void clear_window();

    void display_patients_list();

    void add_patient(Patient);

    void display_patient_progress(Patient&, int);
};