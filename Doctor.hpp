extern std::mutex refresh_mtx;
extern bool running;

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
    Doctor(int _id, std::vector<Examination>& _exams, OperatingRoom& _operating_room);

    void draw();

    void clear_progresWindow();

    void changeStatus(std::string);

    void preparing();

    void examine(int);

    void operate();

    void on_duty();
};