extern std::mutex refresh_mtx;
extern bool running;

class Cleaner
{
    private:
    WINDOW* statusWindow;
    WINDOW* progresWindow;
    int id, y_max, x_max, win_height, win_width;

    public:
    Cleaner(int);

    void draw();

    void keep_clean();
};