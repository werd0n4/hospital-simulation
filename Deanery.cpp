extern std::mutex refresh_mtx;


class Deanery
{
    private:
    WINDOW* window;
    int y_max, x_max, win_height, win_width;

    public:
    Deanery(){
        getmaxyx(stdscr, y_max, x_max);
        win_height = y_max/6;
        win_width = x_max/6;
        window = newwin(win_height, win_width, 0, x_max/3);

        draw();
    }

    void draw(){
        wattron(window, COLOR_PAIR(1));
        box(window, 0, 0);
        wattroff(window, COLOR_PAIR(1));
        mvwprintw(window, 1, win_width/2 - 3, "Deanery");
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(window);
        }
    }
};