#include <vector>
#include <thread>
#include <atomic>
#include <ncurses.h>
#include <condition_variable>
#include <ctime>
#include <memory>
#include <algorithm>
#include <deque>

enum { green = 1, red, yellow, cyan, white, blue, magenta };

#include "Patient.cpp"
#include "Doctor.cpp"
#include "Bed.cpp"
#include "Examination.cpp"
#include "OperatingRoom.cpp"
#include "Reception.cpp"
#include "Rehabilitation.cpp"

std::mutex refresh_mtx;
bool running;

//template function to look for empty room
template<typename Room>
Room& find_empty_room(std::vector<Room>& rooms, std::mutex& change_room_status_mtx,
                      std::mutex& waiting_for_room_mtx, std::condition_variable& cv, bool Room::* is_occupied)
{
    typename std::vector<Room>::iterator room_found;

    while(true){
        std::lock_guard<std::mutex> lg(change_room_status_mtx);
        room_found = std::find_if(rooms.begin(), rooms.end(), [is_occupied](Room& room){return !(room.*is_occupied);});
        if(room_found == rooms.end()){
            std::unique_lock<std::mutex> ul(waiting_for_room_mtx);
            cv.wait(ul);
        }else{
            *room_found.*is_occupied = true;
            break;
        }
    }

    return *room_found;
}

void init_screen(){
    initscr();   
    curs_set(0);
    //colors definition 
    start_color();
    init_pair(green, COLOR_GREEN, COLOR_BLACK);
    init_pair(red, COLOR_RED, COLOR_BLACK);
    init_pair(yellow, COLOR_YELLOW, COLOR_BLACK);
    init_pair(cyan, COLOR_CYAN, COLOR_BLACK);
    init_pair(white, COLOR_WHITE, COLOR_BLACK);
    init_pair(blue, COLOR_BLUE, COLOR_BLACK);
    init_pair(magenta, COLOR_MAGENTA, COLOR_BLACK);
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

    std::vector<Examination> examinations(2);
    std::vector<Bed> beds;
    std::vector<Doctor> doctors;
    std::vector<Patient> patients;
    std::vector<std::thread> patientThreads;
    std::vector<std::thread> doctorThreads;

    OperatingRoom operatingRoom{};
    Reception reception{beds};
    Rehabilitation rehabilitation{};

    for(int i = 0; i < 2; ++i){
        examinations[i].init(i);
    }
    for(int i = 0; i < 10; ++i){
        beds.push_back(Bed{i});
    }
    for(int i = 0; i < 3; ++i){
        doctors.push_back(Doctor{i, examinations, operatingRoom});
    }
    for(int i = 0; i < 21; ++i){
        patients.push_back(Patient{i, examinations, operatingRoom, rehabilitation, reception});
    }

    //threads initialization
    std::thread userInput ([]{getUserInput();});
    for(auto& patient : patients){
        patientThreads.push_back(std::thread([&patient, &examinations]{patient.treatment();}));
    }
    for(auto& doctor : doctors){
        doctorThreads.push_back(std::thread([&doctor]{doctor.on_duty();}));
    }

    userInput.join();
    for(auto& thread : patientThreads){
        thread.join();
    }
    for(auto& thread : doctorThreads){
        thread.join();
    }

    endwin();
    return 0;
}