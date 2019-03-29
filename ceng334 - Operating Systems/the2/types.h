#ifndef TYPES_H
#define TYPES_H

#define CELL_EMPTY     '-' // empty cell
#define CELL_WFOOD     'o' // cell with food
#define CELL_WANT      '1' // cell with ant
#define CELL_WANTFOOD  'P' // cell with an ant with food
#define CELL_WSANT     'S' // cell with a sleeping ant
#define CELL_WSANTFOOD '$' // cell with a sleeping ant with a food

typedef enum {
    STOP,
    CONTINUE
} SimStatus;

typedef enum {
    ANTS,
    DRAW
} Turn;

typedef enum {
    WFOOD,
    WOFOOD,
    TIRED
} AntStatus;

typedef struct {
    int x;
    int y;
} Coordinate;

typedef struct {
    Coordinate coordinate;
    pthread_t tid;
    pthread_cond_t notSleepingCv;
    bool notSleeping;
    pthread_mutex_t notSleepingMutex;
} AntInfo;

const int diff[8][2] = {
    { -1, -1 },
    {  0,  1 },
    {  1,  0 },
    { -1,  1 },
    {  0, -1 },
    {  1, -1 },
    { -1,  0 },
    {  1,  1 }
};
const Coordinate COORD_FAILED = (Coordinate) { -1, -1 };

#endif

