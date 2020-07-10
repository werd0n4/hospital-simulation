#include "Cleaner.hpp"

extern std::mutex refresh_mtx;
extern bool running;

Cleaner::Cleaner(int _id) : id(_id) {
    getmaxyx(stdscr, y_max, x_max);
    win_height = 3;
    win_width = x_max/6;
    statusWindow = newwin(win_height, win_width, (id+1)*3*win_height+y_max/6, 0);
    progresWindow = newwin(win_height, win_width, (id+1)*3*win_height+y_max/6, win_width);

    draw();
}

void Cleaner::draw(){
    wattron(statusWindow, COLOR_PAIR(7));
    wattron(progresWindow, COLOR_PAIR(7));
    box(statusWindow, 0, 0);
    box(progresWindow, 0, 0);
    wattroff(statusWindow, COLOR_PAIR(7));
    wattroff(progresWindow, COLOR_PAIR(7));
    {
        std::lock_guard<std::mutex> lg(refresh_mtx);
        wrefresh(statusWindow);
        wrefresh(progresWindow);
    }
}

void Cleaner::keep_clean(){
    // clean_beds
    // clean_examinations
    // clean_operating
}
