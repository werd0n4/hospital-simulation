extern std::mutex refresh_mtx;


class Examination
{
    private:
    WINDOW* window;
    int y_max, x_max, win_height, win_width;

    public:
    int id, patient_id, doctor_id;
    bool isPatientIn;
    bool isDoctorIn;
    bool is_exam_finished;
    std::mutex mtx;
    std::condition_variable cv;

    Examination(){
        isPatientIn = false;
        isDoctorIn = false;
        is_exam_finished = false;
        getmaxyx(stdscr, y_max, x_max);
        win_height = y_max/6;
        win_width = x_max/5;
    }

    void init(int _id){
        id = _id;
        window = newwin(win_height, win_width, 0, (id+1)*win_width);
        draw();
    }

    void draw(){
        werase(window);
        wattron(window, COLOR_PAIR(1));
        box(window, 0, 0);
        wattroff(window, COLOR_PAIR(1));
        mvwprintw(window, 1, win_width/2 - 7, "Examination %d", id);
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            wrefresh(window);
        }
    }

    void print_info_about_sim(){
        mvwprintw(window, 3, 1, "Doctor %d examines patient %d", doctor_id, patient_id);
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            wrefresh(window);
        }
    }

};