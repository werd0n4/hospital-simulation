extern std::mutex refresh_mtx;
extern bool running;


class Doctor
{
    private:
    WINDOW* status_window;
    WINDOW* progres_window;
    const int y_max = getmaxy(stdscr);
    const int x_max = getmaxx(stdscr);
    const int win_height = 3;
    const int win_width = x_max/5;
    int id, time;
    std::string status;
    std::vector<Examination>& exams;
    OperatingRoom& operating_room;

    public:
    Doctor(int, std::vector<Examination>&, OperatingRoom&);

    void draw();

    void clear_progres_window();

    void change_status(const std::string&);

    void preparing();

    void examine(int);

    void operate();

    void on_duty();
};