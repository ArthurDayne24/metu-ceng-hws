#ifndef do_not_submit
#define do_not_submit
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <time.h>

//////////////////////////////////////////
// things you can access
#define GRIDSIZE 30
#define DRAWDELAY 50000
void setDelay(int d);
int getDelay();
void setSleeperN(int d);
int getSleeperN();
void putCharTo(int i, int j, char c);
char lookCharAt(int i, int j);
void startCurses();
void endCurses();
void drawWindow();
//////////////////////////////////////////

//////////////////////////////////////////
// things you must not access
char grid[GRIDSIZE][GRIDSIZE];
int delay_n = 50;
int sleeper_n = 0;
long actions[GRIDSIZE][GRIDSIZE];
long prev_actions = 0;
struct timespec time_pre;
WINDOW *gridworld = NULL;
#define ESC 27
int offsetx, offsety;

void setDelay(int d) {
    if (d >= 0) delay_n = d;
}

int getDelay() {
    return delay_n;
}

void setSleeperN(int d) {
    if (d >= 0) sleeper_n = d;
}

int getSleeperN() {
    return sleeper_n;
}

void putCharTo(int i, int j, char c) {
    actions[i][j]++;
    grid[i][j] = c;
    usleep(1000 + (rand() % 500));
}

char lookCharAt(int i, int j) {
    actions[i][j]++;
    return grid[i][j];
}

void getDimensions() {
    offsetx = (COLS - 2*GRIDSIZE+1) / 2;
    offsety = (LINES - GRIDSIZE) / 2;
}

void initCurses(){
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    refresh();
}

void startCurses() {
    initCurses();
    
    getDimensions();
    
    int i,j;
    for (i = 0; i < GRIDSIZE; i++)
        for (j = 0; j < GRIDSIZE; j++){
            actions[i][j] = 0;
        }
}

void endCurses() {
    erase();
    refresh();
    if (gridworld != NULL) delwin(gridworld);
    gridworld = NULL;
    
    endwin();
}



void drawWindow() {
    
    if (COLS > 90 && LINES > 40){
        getDimensions();
        erase();
        werase(gridworld);
        if (gridworld != NULL) delwin(gridworld);
        gridworld = newwin(GRIDSIZE+2, 2*GRIDSIZE+1, offsety, offsetx);
        
        wborder(gridworld, 0, 0, 0, 0, 0, 0, 0, 0);

        struct timespec time_now;
        clock_gettime(CLOCK_MONOTONIC, &time_now);
        double elapsed = (time_now.tv_sec - time_pre.tv_sec) * 1e3 + (time_now.tv_nsec - time_pre.tv_nsec) / 1.0e6;
        time_pre = time_now;
        
        int total_actions = 0;
        int i,j;
        for (i = 0; i < GRIDSIZE; i++)
            for (j = 0; j < GRIDSIZE; j++){
                total_actions += actions[i][j];
            }
        int n_actions = total_actions - prev_actions;
        prev_actions = total_actions;

        char thr[5];
        FILE* stats = fopen("/proc/self/stat", "r");
        fscanf(stats,"%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s", thr);
        fclose(stats);

        int nants = 0;
        int nfoods = 0;
        int nsants = 0;
        for (i = 0; i < GRIDSIZE; i++) {
            for (j = 0; j < GRIDSIZE; j++) {
                mvwaddch(gridworld, i+1, 2*j+1, grid[i][j]);
                switch (grid[i][j]) {
                    case 'P':
                        nants++;
                        nfoods++;
                        break;
                    case '$':
                        nants++;
                        nfoods++;
                        nsants++;
                        break;
                    case '1':
                        nants++;
                        break;
                    case 'S':
                        nants++;
                        nsants++;
                        break;
                    case 'o':
                        nfoods++;
                        break;
                    default:
                        break;
                  }
            }
        }
        
        mvprintw(0, 0, "Elapsed time since last call to drawWindow(): %5.5f               ", elapsed);
        mvprintw(1, 0, "Total number of actions per ms: %f               ", n_actions != 0 ? n_actions/elapsed:0);
        mvprintw(2, 0, "# Ants(sleep/total): (%3d/%3d) |# Foods: %3d |# Threads: %s", nsants, nants, nfoods, thr);
        mvprintw(3, 0, "Expected number of sleepers: %3d, Delay amount: %3d", sleeper_n, delay_n);
        mvprintw(LINES-2, 0, "'q' for exit, '+' and '-' for delay, '*' and '/' for sleepers.");

        refresh();
        wrefresh(gridworld);
        
    }
    else{
        erase();
        mvprintw(0, 0, "You need a bigger terminal window, you can resize");
        refresh();
    }
    
}

#endif
