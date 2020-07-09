extern std::mutex refresh_mtx;


class Examination
{
    private:
    WINDOW* window;
    int y_max, x_max, win_height, win_width;

    public:
    int id, patient_id, doctor_id;
    bool is_patient_in;
    bool is_doctor_in;
    bool is_exam_finished;
    std::mutex mtx;
    std::condition_variable cv;

    Examination(){
        is_patient_in = false;
        is_doctor_in = false;
        is_exam_finished = false;
        getmaxyx(stdscr, y_max, x_max);
        win_height = y_max/6;
        win_width = x_max/5;
    }

    void init(int _id){
        id = _id;
        window = newwin(win_height, win_width, 0, (id+1)*win_width);
        print_info_about_sim();
    }

    void clear_window(){
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
        clear_window();
        if(is_doctor_in)
            mvwprintw(window, 3, 1, "Doctor: %d", doctor_id);
        else
            mvwprintw(window, 3, 1, "Doctor: None");

        if(is_patient_in)
            mvwprintw(window, 4, 1, "Patient: %d", patient_id);
        else
            mvwprintw(window, 4, 1, "Patient: None");
        {
            std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
            wrefresh(window);
        }
    }

};