extern std::mutex refresh_mtx;


class LectureHall{
    private:
    int y_max, x_max, win_height, win_width;
    WINDOW* window;

    public:
    LectureHall(){
        getmaxyx(stdscr, y_max, x_max);
        win_height = y_max/4;
        win_width = x_max/3;
        window = newwin(win_height, win_width, 0, 0);

        draw();
    }

    void draw(){
        wattron(window, COLOR_PAIR(1));
        box(window, 0, 0);
        wattroff(window, COLOR_PAIR(1));
        mvwprintw(window, 1, win_width/2 - 6, "Lecture Hall");
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(window);
        }
    }
};
