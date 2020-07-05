extern std::mutex refresh_mtx;


class Rehabilitation{
    private:
    int y_max, x_max, win_height, win_width;
    WINDOW* window;

    public:
    Rehabilitation(){
        getmaxyx(stdscr, y_max, x_max);
        win_height = y_max/3;
        win_width = x_max/3;
        window = newwin(win_height, win_width, y_max/4, 3./5*x_max);

        draw();
    }

    void draw(){
        wattron(window, COLOR_PAIR(3));
        box(window, 0, 0);
        wattroff(window, COLOR_PAIR(3));
        mvwprintw(window, 1, win_width/2 - 8, "Rehabilitation");
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(window);
        }
    }
};
