#pragma once

extern std::mutex refresh_mtx;

class OperatingRoom{
    private:
    int y_max, x_max, win_height, win_width;
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
