extern std::mutex refresh_mtx;


class Reception
{
    private:
    WINDOW* window;
    int y_max, x_max, win_height, win_width;
    int registration_time;
    std::atomic_bool isOccupied;
    std::vector<Bed>& beds;

    public:
    std::mutex mtx;
    std::condition_variable cv;

    Reception(std::vector<Bed>& _beds) : beds(_beds) {
        isOccupied = false;
        registration_time = 1000;

        getmaxyx(stdscr, y_max, x_max);
        win_height = y_max/6;
        win_width = x_max/5;
        window = newwin(win_height, win_width, win_height, x_max/4);

        draw();
    }

    void draw(){
        werase(window);
        wattron(window, COLOR_PAIR(1));
        box(window, 0, 0);
        wattroff(window, COLOR_PAIR(1));
        mvwprintw(window, 1, win_width/2 - 5, "Reception");
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(window);
        }
    }

    void registerPatient(int patient_id){
        int time = registration_time + rand()%1001;
        time = time / (win_width-2);
        mvwprintw(window, 3, 3, "Registering patient nr %d", patient_id);

        for(auto& bed : beds){
            if(!bed.getIsOccupied()){
                bed.assignPatient(patient_id);
                break;
            }
        }

        for(int i = 1; i <= win_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(time));
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                mvwprintw(window, 4, i, "=");
                wrefresh(window);
            }
        }
        draw();
    }

    bool getIsOccupied(){
        return isOccupied;
    }

    void setIsOccupied(bool x){
        isOccupied = x;
    }
};