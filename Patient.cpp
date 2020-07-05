extern std::mutex refresh_mtx;


class Patient
{
    private:
    int id, shift;
    int y_max, x_max, win_height, win_width;
    WINDOW* statusWindow;
    WINDOW* progresWindow;

    public:
    Patient(int _id) : id(_id) {
        getmaxyx(stdscr, y_max, x_max);
        win_height = 3;
        win_width = x_max/8;

        statusWindow = newwin(win_height, win_width, y_max*.6 + (id%7)*win_height, id/7*(2*win_width));
        progresWindow = newwin(win_height, win_width, y_max*.6 + (id%7)*win_height, id/7 * (2*win_width) + win_width);

        draw();
    }

    void draw(){
        wattron(statusWindow, COLOR_PAIR(6));
        wattron(progresWindow, COLOR_PAIR(6));
        box(statusWindow, 0, 0);
        box(progresWindow, 0, 0);
        wattroff(statusWindow, COLOR_PAIR(6));
        wattroff(progresWindow, COLOR_PAIR(6));
        {
            std::lock_guard<std::mutex>lg(refresh_mtx);
            wrefresh(statusWindow);
            wrefresh(progresWindow);
        }
    }
};