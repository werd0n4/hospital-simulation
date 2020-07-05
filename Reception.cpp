extern std::mutex refresh_mtx;


class Reception
{
    private:
    WINDOW* window;
    int y_max, x_max, win_height, win_width;

    public:
    Reception(){
        getmaxyx(stdscr, y_max, x_max);
        win_height = y_max/6;
        win_width = x_max/5;
        window = newwin(win_height, win_width, win_height, x_max/4);

        draw();
    }

    void draw(){
        wattron(window, COLOR_PAIR(1));
        box(window, 0, 0);
        wattroff(window, COLOR_PAIR(1));
        mvwprintw(window, 1, win_width/2 - 5, "Reception");
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(window);
        }
    }
};