#pragma once

extern std::mutex refresh_mtx;

class Examination
{
    private:
    WINDOW* window;
    const int y_max = getmaxy(stdscr);
    const int x_max = getmaxx(stdscr);
    const int win_height = y_max/6;
    const int win_width = x_max/5;

    public:
    int id, patient_id, doctor_id;
    // std::atomic<bool> is_patient_in;
    bool is_patient_in;
    bool is_doctor_in;
    std::atomic<bool> is_exam_finished;
    std::mutex pat_mtx;
    std::mutex doc_mtx;
    std::condition_variable cv;

    Examination();

    void init(int);

    void clear_window();

    void print_info_about_sim();
};