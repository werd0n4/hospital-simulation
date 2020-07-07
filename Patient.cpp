extern std::mutex refresh_mtx;


class Patient
{
    private:
    int id, shift;
    int y_max, x_max, win_height, win_width;
    char* status;
    WINDOW* statusWindow;
    WINDOW* progresWindow;

    public:
    Patient(int _id) : id(_id) {
        status = (char*)"Waiting";

        getmaxyx(stdscr, y_max, x_max);
        win_height = 3;
        win_width = x_max/7;

        statusWindow = newwin(win_height, win_width, y_max*.6 + (id%7)*win_height, id/7*(2*win_width));
        progresWindow = newwin(win_height, win_width, y_max*.6 + (id%7)*win_height, id/7 * (2*win_width) + win_width);

        draw();
    }

    void draw(){
        werase(statusWindow);
        werase(progresWindow);
        wattron(statusWindow, COLOR_PAIR(6));
        wattron(progresWindow, COLOR_PAIR(6));
        box(statusWindow, 0, 0);
        box(progresWindow, 0, 0);
        wattroff(statusWindow, COLOR_PAIR(6));
        wattroff(progresWindow, COLOR_PAIR(6));
        mvwprintw(statusWindow, 1, 1, "Patient %d: %s", id, status);
        {
            std::lock_guard<std::mutex>lg(refresh_mtx);
            wrefresh(statusWindow);
            wrefresh(progresWindow);
        }
    }

    void changeStatus(char* newStatus){
        status = newStatus;
        draw();
    }

    void registration(Reception& reception){
        std::unique_lock<std::mutex> ul(reception.mtx);
        reception.cv.wait(ul, [&reception]{return !reception.getIsOccupied();});//wait unitl recpetion is free
        reception.setIsOccupied(true);
        changeStatus("Registering");
        reception.registerPatient(id);
        changeStatus("Waiting");
        reception.setIsOccupied(false);
        reception.cv.notify_one();
    }

    void treatment(Reception& reception){
        registration(reception);
        // examination
        // operation
        // rehabilitation
        // discharging
    }
};