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
    const int y_max = getmaxy(stdscr);
    const int x_max = getmaxx(stdscr);
    const int win_height = 3;
    const int win_width = x_max/4;
    std::string status;
    WINDOW* status_window;
    std::vector<Examination>& exams;
    OperatingRoom& operating_room;
    Rehabilitation& rehab_room;
    Reception& reception;

    public:
    int id;
    int bed_id;

    Patient(int _id, std::vector<Examination>&, OperatingRoom&, Rehabilitation&, Reception&);

    void draw();

    void clear_status_window();

    void change_status(const std::string&);

    void registration();

    void go_for_exam();

    void undergo_surgery();

    void rehabilitation();

    void discharge();

    void treatment();

    bool operator==(const Patient&);
};