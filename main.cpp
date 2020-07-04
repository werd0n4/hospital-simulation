#include <vector>
#include <thread>
#include <ncurses.h>
#include <condition_variable>

#include "LectureHall.cpp"
#include "Deanery.cpp"
#include "Coatroom.cpp"
#include "Cafeteria.cpp"
#include "Student.cpp"


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

}

int main()
{
    init_screen();

    LectureHall lectureHall{};    
    Deanery deanery{};
    Coatroom coatroom{};
    Cafeteria cafeteria{};
    std::vector<Student> students;
    for(int i = 0; i < 20; ++i){
        students.push_back(Student{i});
    }

    getchar();

    endwin();
    return 0;
}