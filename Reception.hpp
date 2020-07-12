#pragma once
#include "Bed.hpp"

extern std::mutex refresh_mtx;

class Bed;

class Reception
{
    private:
    WINDOW* window;
    int y_max, x_max, win_height, win_width;
    int registration_time;
    std::atomic_bool isOccupied;
    std::vector<Bed>& beds;

    public:
    std::mutex mtx;
    std::mutex discharge_mtx;
    std::condition_variable cv;
    std::condition_variable discharge_cv;

    Reception(std::vector<Bed>&);

    void draw();

    void register_patient(Patient);

    void discharge_patient(Patient);

    bool getIsOccupied();

    void setIsOccupied(bool);
};