#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "types.h"

SimStatus simStatus;

// number of not sleeping ants
int activeAntCnt;

// is it ants' turn to modify grid or drawWindow()'s turn ?
Turn gridTurn;
pthread_mutex_t turnMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t antsTurn = PTHREAD_COND_INITIALIZER; 
pthread_cond_t drawTurn = PTHREAD_COND_INITIALIZER; 

// wait for it for simDuration seconds
pthread_mutex_t earlyStopMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t earlyStopCv = PTHREAD_COND_INITIALIZER; 

// is a cell acquired or not
bool isAvail[GRIDSIZE][GRIDSIZE];
// controls isAvail, lookCharAt() and putCharTo() utilities for a given cell
sem_t cellCheckMutex[GRIDSIZE][GRIDSIZE];

// is is returned coordinate indicates failure
bool coordSuccess(Coordinate coordinate) {
    return (coordinate.x == -1 /*&& coordinate.y == -1*/ ? false : true);
}

// is given coordinate within grid
bool isValid(int x, int y) {
    return (x >= 0 && y >= 0 && x < GRIDSIZE && y < GRIDSIZE) ? true : false;
}

// A number of ants can modify grid (exclusive)or drawWindow() is running.
// This is controlled by following 4 functions

// These two functions regulates ants' actions calls to putCharTo().
void antEnterRegion() {
    pthread_mutex_lock(&turnMutex);

    while (gridTurn == DRAW) {
        pthread_cond_wait(&antsTurn, &turnMutex);
    }

    activeAntCnt++;

    pthread_mutex_unlock(&turnMutex);
}

void antExitRegion() {
    pthread_mutex_lock(&turnMutex);

    activeAntCnt--;

    if (activeAntCnt == 0) {
        pthread_cond_signal(&drawTurn);
    }
    
    pthread_mutex_unlock(&turnMutex);
}

// These two functions regulate when drawWindow() can own grid.
void drawWindowEnterRegion() {
    pthread_mutex_lock(&turnMutex);
    
    gridTurn = DRAW;

    while (activeAntCnt != 0) {
        pthread_cond_wait(&drawTurn, &turnMutex);
    }

    pthread_mutex_unlock(&turnMutex);
}

void drawWindowExitRegion() {
    pthread_mutex_lock(&turnMutex);

    gridTurn = ANTS;

    pthread_cond_broadcast(&antsTurn);

    pthread_mutex_unlock(&turnMutex);
}

// These two functions make next ant slept or awake
void anesthetizeAnt(AntInfo *antInfo) {

    pthread_mutex_lock(&antInfo->notSleepingMutex);
    
    antInfo->notSleeping = false;

    pthread_mutex_unlock(&antInfo->notSleepingMutex);
}

void wakeUpAnt(AntInfo *antInfo) {

    pthread_mutex_lock(&antInfo->notSleepingMutex);
    
    antInfo->notSleeping = true;
    pthread_cond_signal(&antInfo->notSleepingCv);

    pthread_mutex_unlock(&antInfo->notSleepingMutex);
}

// Will be called from an ant of state WOFOOD
//  Try to find a neighbour cell with food
//  If locked = true, then order of traversing is made random.
// On success:
//  acquire cell(i, j) if locked = true
//  return Coordinate(i, j)
// On fail:
//  return COORD_FAILED
Coordinate getCellWithFood(int x, int y, bool locked, unsigned int *ptr2seed) {

    // shuffle the order of traversing adjacent 8 cells
    int shuffledArr[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    // If the cell found will not be acquired, then there is no need to shuffle
    if (locked == true) {
        for (int i = 0; i < 8; i++) {
            int magic = rand_r(ptr2seed) % 8;
            int tmp = shuffledArr[i];
            shuffledArr[i] = shuffledArr[magic];
            shuffledArr[magic] = tmp;
        }
    }

    int x1, y1;

    for (int i = 0; i < 8; i++) {
        x1 = x + diff[shuffledArr[i]][0];
        y1 = y + diff[shuffledArr[i]][1];

        if (isValid(x1, y1)) {

            sem_wait(&cellCheckMutex[x1][y1]);

            if (isAvail[x1][y1] == true && lookCharAt(x1, y1) == CELL_WFOOD) {
                if (locked == true) {
                    isAvail[x1][y1] = false;
                }
                
                sem_post(&cellCheckMutex[x1][y1]);

                return (Coordinate) { x1, y1 };
            }

            sem_post(&cellCheckMutex[x1][y1]);
        }
    }

    return COORD_FAILED;
}

// Will be called from an ant of state WOFOOD
//  if there does not exist any neighbour cell with food
// Try to find an empty neighbour cell with food (randomly)
// On success:
//  acquire that cell
//  return Coordinate(i, j)
// On fail:
//  return COORD_FAILED
Coordinate getEmptyCell(int x, int y, unsigned int *ptr2seed) {
   
    int shuffledArr[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    for (int i = 0; i < 8; i++) {
        int magic = rand_r(ptr2seed) % 8;
        int tmp = shuffledArr[i];
        shuffledArr[i] = shuffledArr[magic];
        shuffledArr[magic] = tmp;
    }

    int x1, y1;

    for (int i = 0; i < 8; i++) {
        x1 = x + diff[shuffledArr[i]][0];
        y1 = y + diff[shuffledArr[i]][1];

        if (isValid(x1, y1)) {

            sem_wait(&cellCheckMutex[x1][y1]);
            
            if (isAvail[x1][y1] == true && lookCharAt(x1, y1) == CELL_EMPTY) {
                isAvail[x1][y1] = false;
                sem_post(&cellCheckMutex[x1][y1]);

                return (Coordinate) { x1, y1 };
            }
            
            sem_post(&cellCheckMutex[x1][y1]);
        }
    }

    return COORD_FAILED;
}

// obvious
void *simTimer(void *arg) {

    int simDuration = * (int *) arg;

    struct timeval tp;
    gettimeofday(&tp, NULL);
    
    struct timespec ts;
    ts.tv_sec = tp.tv_sec + simDuration;
    ts.tv_nsec = 0;

    pthread_mutex_lock(&earlyStopMutex);
    
    // Get signal if early exit, otherwise terminate simulation after simDuration passed
    pthread_cond_timedwait(&earlyStopCv, &earlyStopMutex, &ts);
        
    pthread_mutex_unlock(&earlyStopMutex);
    
    simStatus = STOP;

    return NULL;
}

// obvious
void *antRoutine(void *arg) {

    AntInfo *antInfo = (AntInfo*) arg;
    // current coordinate
    int x = antInfo->coordinate.x,
        y = antInfo->coordinate.y;

    // candidate new coordinate
    int x1, y1;
    
    // 4381 is just a magic number for pseudo randomness lol 
    // maybe a better logic could be developed TODO
    unsigned int seed = time(NULL) + antInfo->tid * 4381;
    
    // current state
    AntStatus antStatus = WOFOOD;

    while (simStatus == CONTINUE) {

        // In case of simStatus=STOP, we should immediately break below usleep TODO
        usleep(getDelay() * 1000 + (rand_r(&seed) % 10000));

        if (antStatus == WFOOD) {

            // find that cell but not lock
            Coordinate cellWFoodCoord = (Coordinate) getCellWithFood(x, y, false, &seed);

            if (coordSuccess(cellWFoodCoord)) {

                // find and lock that cell
                Coordinate newCoordinate = (Coordinate) getEmptyCell(x, y, &seed);

                if (coordSuccess(newCoordinate)) {

                    x1 = newCoordinate.x;
                    y1 = newCoordinate.y;

                    sem_wait(&cellCheckMutex[x][y]);
                    sem_wait(&cellCheckMutex[x1][y1]);

                    isAvail[x][y] = true;
                    
                    antEnterRegion();

                    putCharTo(x, y, CELL_WFOOD);
                    putCharTo(x1, y1, CELL_WANT);

                    antExitRegion();

                    sem_post(&cellCheckMutex[x1][y1]);
                    sem_post(&cellCheckMutex[x][y]);

                    x = x1;
                    y = y1;

                    antStatus = TIRED;
                }
            }
            
            else {

                // find and lock that cell
                Coordinate newCoordinate = (Coordinate) getEmptyCell(x, y, &seed);

                if (coordSuccess(newCoordinate)) {

                    x1 = newCoordinate.x;
                    y1 = newCoordinate.y;

                    sem_wait(&cellCheckMutex[x][y]);
                    sem_wait(&cellCheckMutex[x1][y1]);

                    isAvail[x][y] = true;

                    antEnterRegion();

                    putCharTo(x, y, CELL_EMPTY);
                    putCharTo(x1, y1, CELL_WANTFOOD);

                    antExitRegion();

                    sem_post(&cellCheckMutex[x1][y1]);
                    sem_post(&cellCheckMutex[x][y]);

                    x = x1;
                    y = y1;

                    // no state change
                }
            }
        }

        else if (antStatus == WOFOOD) {

            // find and lock that cell
            Coordinate newCoordinate = (Coordinate) getCellWithFood(x, y, true, &seed);

            if (coordSuccess(newCoordinate)) {

                x1 = newCoordinate.x;
                y1 = newCoordinate.y;

                sem_wait(&cellCheckMutex[x][y]);
                sem_wait(&cellCheckMutex[x1][y1]);
                
                isAvail[x][y] = true;

                antEnterRegion();

                putCharTo(x, y, CELL_EMPTY);
                putCharTo(x1, y1, CELL_WANTFOOD);
               
                antExitRegion();

                sem_post(&cellCheckMutex[x1][y1]);
                sem_post(&cellCheckMutex[x][y]);

                x = x1;
                y = y1;

                antStatus = WFOOD;
            }
            else {

                Coordinate newCoordinate = getEmptyCell(x, y, &seed);

                if (coordSuccess(newCoordinate)) {

                    x1 = newCoordinate.x;
                    y1 = newCoordinate.y;

                    sem_wait(&cellCheckMutex[x][y]);
                    sem_wait(&cellCheckMutex[x1][y1]);

                    isAvail[x][y] = true;

                    antEnterRegion();

                    putCharTo(x, y, CELL_EMPTY);
                    putCharTo(x1, y1, CELL_WANT);

                    antExitRegion();
                    
                    sem_post(&cellCheckMutex[x1][y1]);
                    sem_post(&cellCheckMutex[x][y]);

                    x = x1;
                    y = y1;

                    // no state change
                }
            }
        }

        else if (antStatus == TIRED) {

            Coordinate newCoordinate = getEmptyCell(x, y, &seed);

            if (coordSuccess(newCoordinate)) {

                x1 = newCoordinate.x;
                y1 = newCoordinate.y;

                sem_wait(&cellCheckMutex[x][y]);
                sem_wait(&cellCheckMutex[x1][y1]);

                isAvail[x][y] = true;

                antEnterRegion();

                putCharTo(x, y, CELL_EMPTY);
                putCharTo(x1, y1, CELL_WANT);

                antExitRegion();

                sem_post(&cellCheckMutex[x1][y1]);
                sem_post(&cellCheckMutex[x][y]);

                x = x1;
                y = y1;

                antStatus = WOFOOD;
            }
        }
        
        char oldSymbol = lookCharAt(x, y);
        
        pthread_mutex_lock(&antInfo->notSleepingMutex);

        while (antInfo->notSleeping == false) {
            antEnterRegion();
        
            putCharTo(x, y, (antStatus == WFOOD ? CELL_WSANTFOOD : CELL_WSANT));
            
            antExitRegion();

            pthread_cond_wait(&antInfo->notSleepingCv, &antInfo->notSleepingMutex);
        }

        pthread_mutex_unlock(&antInfo->notSleepingMutex);

        antEnterRegion();
        
        putCharTo(x, y, oldSymbol);

        antExitRegion();

    }
    
    return NULL;
}

int main(int argc, char *argv[]) {

    srand(time(NULL));

    int numAnts = atoi(argv[1]),
        numFoods = atoi(argv[2]),
        simDuration = atoi(argv[3]);

    AntInfo *antArr = (AntInfo*) malloc(sizeof(AntInfo) * numAnts);
    pthread_t simTimerTid;

    activeAntCnt = 0;
    gridTurn = ANTS;
    simStatus = CONTINUE;

    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            sem_init(&cellCheckMutex[i][j], 0, 1);
            isAvail[i][j] = true;
            putCharTo(i, j, CELL_EMPTY);
        }
    }

    for (int i = 0; i < numAnts; i++) {
        int x, y;
        do {
            x = rand() % GRIDSIZE;
            y = rand() % GRIDSIZE;
        } while (lookCharAt(x, y) != CELL_EMPTY);
    
        isAvail[x][y] = false;

        putCharTo(x, y, CELL_WANT);

        antArr[i].coordinate = (Coordinate) { x, y };
        
        pthread_mutex_init(&antArr[i].notSleepingMutex, NULL);
        pthread_cond_init(&antArr[i].notSleepingCv, NULL);

        antArr[i].notSleeping = true;
    }

    for (int i = 0; i < numFoods; i++) {
        int x, y;
        do {
            x = rand() % GRIDSIZE;
            y = rand() % GRIDSIZE;
        } while (lookCharAt(x, y) != CELL_EMPTY);

        putCharTo(x, y, CELL_WFOOD);
    }

    startCurses();

    pthread_create(&simTimerTid, NULL, simTimer, (void*) &simDuration);
    
    for (int i = 0; i < numAnts; i++) {
        pthread_create(&antArr[i].tid, NULL, antRoutine, (void*) (antArr+i));
    }

    char c = 0;

    while (simStatus == CONTINUE) {

        drawWindowEnterRegion();

        drawWindow();
        
        drawWindowExitRegion();
        
        c = 0;
        c = getch();

        if (c == 'q' || c == ESC) {
            simStatus = STOP;
            pthread_cond_signal(&earlyStopCv);
            break;
        }
        else if (c == '+') {
            setDelay(getDelay()+10);
        }
        else if (c == '-') {
            int curdelay = getDelay();
            if (curdelay != 0) {
                setDelay(curdelay-10);
            }
        }
        else if (c == '*') {
            int ncurSleepers = getSleeperN();

            if (ncurSleepers != numAnts) {
                setSleeperN(ncurSleepers+1);
                anesthetizeAnt(antArr+ncurSleepers);
            }
        }
        else if (c == '/') {
            int ncurSleepers = getSleeperN();

            if (ncurSleepers != 0) {
                setSleeperN(ncurSleepers-1);
                wakeUpAnt(antArr+ncurSleepers-1);
            }
        }

        usleep(DRAWDELAY);
    }
   
    int ncurSleepers = getSleeperN();

    for (int i = 0; i < ncurSleepers; i++) {
        wakeUpAnt(antArr+i);
    }

    for (int i = 0; i < numAnts; i++) {
        pthread_join(antArr[i].tid, NULL);
    }

    endCurses();
    
    pthread_join(simTimerTid, NULL);

    // free/release all the sources

    for (int i = 0;i < numAnts; i++) {
        pthread_mutex_destroy(&antArr[i].notSleepingMutex);
        pthread_cond_destroy(&antArr[i].notSleepingCv);
    }

    free(antArr);

    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            sem_destroy(&cellCheckMutex[i][j]);
        }
    }

    pthread_mutex_destroy(&earlyStopMutex);
    pthread_cond_destroy(&earlyStopCv);

    return 0;
}

