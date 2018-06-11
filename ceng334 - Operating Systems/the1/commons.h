#ifndef COMMONS_H
#define COMMONS_H

int ABS(int num) { return (num < 0) ? -num : num; }

typedef enum { false, true } bool;

typedef struct _coordinate { int x; int y; } coordinate;

typedef struct _server_message {
    coordinate pos;
    coordinate adv_pos;
    int object_count;
    coordinate object_pos[4];
} server_message;

typedef struct _ph_message { coordinate move_request; } ph_message;

const int dxy[4][2] = { {-1, 0}, {+1, 0}, {0, -1}, {0, +1} };

#endif

