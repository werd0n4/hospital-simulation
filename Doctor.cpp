extern std::mutex refresh_mtx;


class Doctor
{
    private:
    WINDOW* statusWindow;
    WINDOW* progresWindow;
    int id, y_max, x_max, win_height, win_width;
    int time;
    char* status;

    public:
    Doctor(int _id) : id(_id) {
        getmaxyx(stdscr, y_max, x_max);
        win_height = 3;
        win_width = x_max/8;
        statusWindow = newwin(win_height, win_width, id*win_height+y_max/6, 0);
        progresWindow = newwin(win_height, win_width, id*win_height+y_max/6, win_width);
        status = "Idle";

        draw();
    }

    void draw(){
        wattron(statusWindow, COLOR_PAIR(2));
        wattron(progresWindow, COLOR_PAIR(2));
        box(statusWindow, 0, 0);
        box(progresWindow, 0, 0);
        wattroff(statusWindow, COLOR_PAIR(2));
        wattroff(progresWindow, COLOR_PAIR(2));
        mvwprintw(statusWindow, 1, 1, "Doc nr %d: %s", id, status);
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(statusWindow);
            wrefresh(progresWindow);
        }
    }

    void changeStatus(char* newStatus){
        status = newStatus;
        draw();
    }

    void preparing(){
        changeStatus("Preparing");
        time = 500 + rand()%501;
        time = time / (win_width-2);
        for(int i = 1; i <= win_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(time));
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                mvwprintw(progresWindow, 1, i, "=");
                wrefresh(progresWindow);
            }
        }
    }

    void on_duty(){
        preparing();
        // examine
        // operate
    }
};
