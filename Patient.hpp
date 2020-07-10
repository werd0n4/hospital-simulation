#pragma once
#include "Rehabilitation.hpp"
#include "Examination.hpp"
#include "Reception.hpp"
#include "OperatingRoom.hpp"


extern std::mutex refresh_mtx;
extern bool running;
class Rehabilitation;
class Reception;

class Patient{
    private:
    int time;
    int y_max, x_max, win_height, win_width;
    std::string status;
    WINDOW* statusWindow;
    std::vector<Examination>& exams;
    OperatingRoom& operating_room;
    Rehabilitation& rehab_room;

    public:
    int id;

    Patient(int _id, std::vector<Examination>& _exams, OperatingRoom& _operating_room, Rehabilitation& _rehab_room);

    void draw();

    void clear_status_window();

    void changeStatus(std::string newStatus);

    void registration(Reception& reception);

    void go_for_exam();

    void undergo_surgery();

    void rehabilitation();

    void discharge();

    void treatment(Reception& reception);

    bool operator==(const Patient&);
};