#pragma once
#include "Patient.hpp"


extern std::mutex refresh_mtx;
// class Patient;

class Bed{
    private:
    int y_max, x_max, win_height, win_width;
    int id;
    std::atomic_bool isOccupied;
    WINDOW* window;
    // Patient patient;

    public:
    std::mutex mtx;
    std::condition_variable cv;

    Bed();

    void init(int);

    void draw();

    void assignPatient(int);

    bool getIsOccupied();

    void setIsOccupied(bool);
};