extern std::mutex refresh_mtx;


class Doctor
{
    private:
    WINDOW* statusWindow;
    WINDOW* progresWindow;
    int id, y_max, x_max, win_height, win_width;
    int time;
    std::string status;
    std::vector<Examination>& exams;
    OperatingRoom& operating_room;

    public:
    Doctor(int _id, std::vector<Examination>& _exams, OperatingRoom& _operating_room) : id(_id), exams(_exams), operating_room(_operating_room) {
        getmaxyx(stdscr, y_max, x_max);
        win_height = 3;
        win_width = x_max/5;
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
        mvwprintw(statusWindow, 1, 1, "Doc nr %d: %s", id, status.c_str());
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(statusWindow);
            wrefresh(progresWindow);
        }
    }

    void clear_progresWindow(){
        werase(progresWindow);
        wattron(progresWindow, COLOR_PAIR(2));
        box(progresWindow, 0, 0);
        wattroff(progresWindow, COLOR_PAIR(2));
        wmove(progresWindow, 1, 1);
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(progresWindow);
        }
    }

    void changeStatus(std::string newStatus){
        werase(statusWindow);
        status = newStatus;
        wattron(statusWindow, COLOR_PAIR(2));
        box(statusWindow, 0, 0);
        wattroff(statusWindow, COLOR_PAIR(2));
        mvwprintw(statusWindow, 1, 1, "Doc nr %d: %s", id, status.c_str());
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(statusWindow);
        }
    }

    void preparing(){
        changeStatus("Preparing");
        time = 3000 + rand()%1001;
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

    void examine(){
        bool has_exam_room = false;
        bool temp;
        int exam_id;
        
        changeStatus("Waiting for exam room");
        while(!has_exam_room){
            for(auto& exam : exams){
                if(!exam.isDoctorIn){
                    std::lock_guard<std::mutex> lg(exam.mtx);
                    exam.isDoctorIn = true;
                    has_exam_room = true;
                    exam.doctor_id = id;
                    exam_id = exam.id;
                    exam.cv.notify_one();
                    break;
                }
            }
        }

        changeStatus("Waiting for patient");
        std::unique_lock<std::mutex> ul(exams[exam_id].mtx);
        exams[exam_id].cv.wait(ul, [this, exam_id]{return exams[exam_id].isPatientIn;});
        exams[exam_id].is_exam_finished = false;

        changeStatus("Examing...");
        clear_progresWindow();
        time = 10000 + rand()%1001;
        time = time / (win_width-2);
        exams[exam_id].print_info_about_sim();
        for(int i = 1; i <= win_width-2; ++i){
            std::this_thread::sleep_for(std::chrono::milliseconds(time));
            {
                std::lock_guard<std::mutex> refresh_guard(refresh_mtx);
                mvwprintw(progresWindow, 1, i, "=");
                wrefresh(progresWindow);
            }
        }
        exams[exam_id].is_exam_finished = true;
        exams[exam_id].isDoctorIn = false;
        changeStatus("Idle");
        clear_progresWindow();
        exams[exam_id].draw();
        exams[exam_id].cv.notify_one();
    }

    void on_duty(){
        preparing();
        examine();
        // operate
    }
};
