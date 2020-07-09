extern std::mutex refresh_mtx;


class Patient
{
    private:
    int id, time;
    int y_max, x_max, win_height, win_width;
    std::string status;
    WINDOW* statusWindow;
    std::vector<Examination>& exams;

    public:
    Patient(int _id, std::vector<Examination>& _exams) : id(_id), exams(_exams) {
        status = "Waiting for registration";

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

        changeStatus("Going for examination");
        reception.setIsOccupied(false);
        reception.cv.notify_one();
    }

    void go_for_exam(){
       bool room_found = false;
       int room_id;
       //find empty examination room
        changeStatus("Waiting for room");
        while(!room_found){
            for(auto& exam : exams){
                if(!exam.is_patient_in.load()){
                    exam.is_patient_in.store(true);
                    exam.cv.notify_one();
                    exam.patient_id = id;
                    room_id = exam.id;
                    room_found = true;
                    exam.print_info_about_sim();
                    break;
                }
            } 
        }

        changeStatus("In room "+std::to_string(room_id));
        std::unique_lock<std::mutex> ul(exams[room_id].pat_mtx);
        exams[room_id].cv.wait(ul, [this, room_id]{return exams[room_id].is_exam_finished.load();});
        exams[room_id].is_exam_finished.store(false);
        exams[room_id].is_patient_in.store(false);
        exams[room_id].print_info_about_sim();
        changeStatus("End");
    }

    void treatment(Reception& reception){
        registration(reception);
        go_for_exam();
        // operation
        // rehabilitation
        // discharging
    }
};