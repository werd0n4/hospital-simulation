extern std::mutex refresh_mtx;


class Doctor
{
    private:
    WINDOW* statusWindow;
    WINDOW* progresWindow;
    int id, y_max, x_max, win_height, win_width;

    public:
    Doctor(int _id) : id(_id) {
        getmaxyx(stdscr, y_max, x_max);
        win_height = 3;
        win_width = x_max/8;
        statusWindow = newwin(win_height, win_width, id*win_height+y_max/6, 0);
        progresWindow = newwin(win_height, win_width, id*win_height+y_max/6, win_width);

        draw();
    }

    void draw(){
        wattron(statusWindow, COLOR_PAIR(2));
        wattron(progresWindow, COLOR_PAIR(2));
        box(statusWindow, 0, 0);
        box(progresWindow, 0, 0);
        wattroff(statusWindow, COLOR_PAIR(2));
        wattroff(progresWindow, COLOR_PAIR(2));
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(statusWindow);
            wrefresh(progresWindow);
        }
    }
};
