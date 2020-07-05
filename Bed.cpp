extern std::mutex refresh_mtx;


class Bed{
    private:
    int y_max, x_max, win_height, win_width;
    int id;
    std::atomic_bool isOccupied;
    WINDOW* window;

    public:
    Bed(int _id) : id(_id){
        isOccupied = false;

        getmaxyx(stdscr, y_max, x_max);
        win_height = y_max/8;
        win_width = x_max/16;
        window = newwin(win_height, win_width, id < 5 ? 0 : win_height, id%5*win_width + 3./5*x_max);

        draw();
    }

    void draw(){
        wattron(window, COLOR_PAIR(1));
        box(window, 0, 0);
        wattroff(window, COLOR_PAIR(1));
        mvwprintw(window, 1, win_width/2 - 3, "Bed %d", id);
        {
            std::lock_guard<std::mutex> lg(refresh_mtx);
            wrefresh(window);
        }
    }
};