#include <vector>
#include <thread>
#include <atomic>
#include <ncurses.h>
#include <condition_variable>
#include <ctime>

#include "Doctor.cpp"
#include "Cleaner.cpp"
#include "Examination.cpp"
#include "OperatingRoom.cpp"
#include "Bed.cpp"
#include "Reception.cpp"
#include "Rehabilitation.cpp"
#include "Patient.cpp"


std::mutex refresh_mtx;
bool running;

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

void getUserInput(){
    char input;

    while(running){
        input = getchar();
        if(input == 'q'){
            running = false;
        }
    }
}

int main()
{
    running = true;
    srand(time(NULL));
    init_screen();

    std::vector<Examination> examinations;
    std::vector<Bed> beds(10);
    std::vector<Doctor> doctors;
    std::vector<Patient> patients;
    std::vector<std::thread> patientThreads;

    OperatingRoom operatingRoom{};
    Reception reception{beds};
    Rehabilitation rehabilitation{};
    Cleaner cleaner{0};
    // Coatroom coatroom{};
    // Cafeteria cafeteria{};
    // std::vector<Student> students;
    // std::vector<Lecturer> lecturers;
    // std::vector<Janitor> janitors;

    for(int i = 0; i < 2; ++i){
        examinations.push_back(Examination{i});
    }
    for(int i = 0; i < 10; ++i){
        beds[i].id = i;
    }
    for(int i = 0; i < 3; ++i){
        doctors.push_back(Doctor{i});
    }
    for(int i = 0; i < 21; ++i){
        patients.push_back(Patient{i});
    }

    std::thread userInput ([]{getUserInput();});
    for(auto& patient : patients){
        patientThreads.push_back(std::thread([&patient, &reception]{patient.treatment(reception);}));
    }

    userInput.join();
    for(auto& thread : patientThreads){
        thread.join();
    }

    // getchar();

    endwin();
    return 0;
}