#pragma once
#include "Patient.hpp"


extern std::mutex refresh_mtx;
class Patient;

class Bed{
    private:
    int y_max, x_max, win_height, win_width;
    int id;
    std::atomic_bool isOccupied;
    WINDOW* window;
    // std::unique_ptr<Patient> patient_ptr;

    public:
    std::mutex mtx;
    std::mutex discharge_mtx;
    std::condition_variable cv;
    int patient_id;

    Bed();

    void init(int);

    void draw();

    void assign_patient(Patient);

    void remove_patient();

    bool check_if_its_patient_bed(Patient);

    bool getIsOccupied();

    void setIsOccupied(bool);
};