#pragma once

extern std::mutex refresh_mtx;

class OperatingRoom{
    private:
    const int y_max = getmaxy(stdscr);
    const int x_max = getmaxx(stdscr);
    const int win_height = y_max/6;
    const int win_width = x_max/5;
    WINDOW* window;

    public:
    int doctor_id, patient_id;
    std::atomic<bool> is_patient_in;
    std::atomic<bool> is_doctor_in;
    std::atomic<bool> is_surgery_finished;
    std::mutex pat_mtx;
    std::mutex doc_mtx;
    std::condition_variable cv;

    OperatingRoom();

    void clear_window();

    void print_info_about_sim();
};
