#include <vector>
#include <thread>
#include <ncurses.h>
#include <condition_variable>

#include "Patient.cpp"
#include "Doctor.cpp"
#include "Cleaner.cpp"
#include "Bed.cpp"
#include "Examination.cpp"
#include "OperatingRoom.cpp"
#include "Reception.cpp"
#include "Rehabilitation.cpp"


std::mutex refresh_mtx;

void init_screen(){
    initscr();   
    curs_set(0);
    //colors
    start_color();
    // init_color(COLOR_YELLOW, 800, 800, 0);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);//
    init_pair(2, COLOR_RED, COLOR_BLACK);//
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);//
    init_pair(4, COLOR_CYAN, COLOR_BLACK);//frames default
    init_pair(5, COLOR_WHITE, COLOR_BLACK);//
    init_pair(6, COLOR_BLUE, COLOR_BLACK);//
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);//
}

int main()
{
    init_screen();

    OperatingRoom operatingRoom{};
    Reception reception{};
    Rehabilitation rehabilitation{};
    Cleaner cleaner{0};
    std::vector<Examination> examinations;
    std::vector<Bed> beds;
    std::vector<Doctor> doctors;
    std::vector<Patient> patients;
    // Coatroom coatroom{};
    // Cafeteria cafeteria{};
    // std::vector<Student> students;
    // std::vector<Lecturer> lecturers;
    // std::vector<Janitor> janitors;

    for(int i = 0; i < 2; ++i){
        examinations.push_back(Examination{i});
    }
    for(int i = 0; i < 10; ++i){
        beds.push_back(Bed{i});
    }
    for(int i = 0; i < 3; ++i){
        doctors.push_back(Doctor{i});
    }
    for(int i = 0; i < 21; ++i){
        patients.push_back(Patient{i});
    }

    getchar();

    endwin();
    return 0;
}