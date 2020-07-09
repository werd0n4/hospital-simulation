extern std::mutex refresh_mtx;


class Patient
{
    private:
    int id, time;
    int y_max, x_max, win_height, win_width;
    std::string status;
    WINDOW* statusWindow;

    public:
    Patient(int _id) : id(_id) {
        status = "Waiting";

        getmaxyx(stdscr, y_max, x_max);
        win_height = 3;
        win_width = x_max/4;

        statusWindow = newwin(win_height, win_width, y_max*.6 + (id%7)*win_height, id/7*(win_width));

        draw();
    }

    void draw(){
        werase(statusWindow);
        wattron(statusWindow, COLOR_PAIR(6));
        box(statusWindow, 0, 0);
        wattroff(statusWindow, COLOR_PAIR(6));
        mvwprintw(statusWindow, 1, 1, "Patient %d: %s", id, status.c_str());
        {
            std::lock_guard<std::mutex>lg(refresh_mtx);
            wrefresh(statusWindow);
        }
    }

    void clear_status_window(){
        werase(statusWindow);
        wattron(statusWindow, COLOR_PAIR(6));
        box(statusWindow, 0, 0);
        wattroff(statusWindow, COLOR_PAIR(6));
        {
            std::lock_guard<std::mutex>lg(refresh_mtx);
            wrefresh(statusWindow);
        }
    }

    void changeStatus(std::string newStatus){
        status = newStatus;
        clear_status_window();
        mvwprintw(statusWindow, 1, 1, "Patient %d: %s", id, status.c_str());
        {
            std::lock_guard<std::mutex>lg(refresh_mtx);
            wrefresh(statusWindow);
        }
    }

    void registration(Reception& reception){
        std::unique_lock<std::mutex> ul(reception.mtx);
        reception.cv.wait(ul, [&reception]{return !reception.getIsOccupied();});//wait unitl reception is free
        reception.setIsOccupied(true);
        changeStatus("Registering");
        reception.registerPatient(id);

        changeStatus("Waiting");
        reception.setIsOccupied(false);
        reception.cv.notify_one();
    }

    void go_for_exam(std::vector<Examination>& exams){
        bool isInExamRoom = false;
        int exam_id;

        changeStatus("Waiting for exam");
        while(!isInExamRoom){
            for(auto& exam : exams){
                if(!exam.isPatientIn){
                    std::lock_guard<std::mutex> lg(exam.mtx);
                    exam.isPatientIn = true;
                    isInExamRoom = true;
                    exam.patient_id = id;
                    exam_id = exam.id;
                    exam.cv.notify_one();
                    break;
                }
            }
        }
        changeStatus("Waiting for doc in " + std::to_string(exam_id));
        std::unique_lock<std::mutex> ul(exams[exam_id].mtx);
        exams[exam_id].cv.wait(ul, [&exams, exam_id]{return exams[exam_id].isDoctorIn;});

        changeStatus("Undergoing exam in " + std::to_string(exam_id));
        exams[exam_id].cv.wait(ul, [&exams, exam_id]{return exams[exam_id].is_exam_finished;});
        exams[exam_id].isPatientIn = false;
    }

    void treatment(Reception& reception, std::vector<Examination>& examinations){
        registration(reception);
        go_for_exam(examinations);
        // operation
        // rehabilitation
        // discharging
    }
};