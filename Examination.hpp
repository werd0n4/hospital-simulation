#pragma once

extern std::mutex refresh_mtx;

class Examination
{
    private:
    WINDOW* window;
    int y_max, x_max, win_height, win_width;

    public:
    int id, patient_id, doctor_id;
    std::atomic<bool> is_patient_in;
    std::atomic<bool> is_doctor_in;
    std::atomic<bool> is_exam_finished;
    std::mutex pat_mtx;
    std::mutex doc_mtx;
    std::condition_variable cv;

    Examination();

    void init(int);

    void clear_window();

    void print_info_about_sim();
};