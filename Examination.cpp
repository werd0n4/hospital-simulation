extern std::mutex refresh_mtx;


class Examination
{
    private:
    WINDOW* window;
    int y_max, x_max, win_height, win_width;
    int id;

    public:
    Examination(int _id) : id(_id) {
        getmaxyx(stdscr, y_max, x_max);
        win_height = y_max/6;
        win_width = x_max/5;
        window = newwin(win_height, win_width, 0, (id+1)*win_width);

        draw();
    }

    void draw(){
        wattron(window, COLOR_PAIR(1));
        box(window, 0, 0);
        wattroff(window, COLOR_PAIR(1));
        mvwprintw(window, 1, win_width/2 - 7, "Examination %d", id);
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(window);
        }
    }
};