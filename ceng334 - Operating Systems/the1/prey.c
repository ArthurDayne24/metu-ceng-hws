#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "commons.h"

int main(int argc, char *argv[]) {
   
    int width  = atoi(argv[1]), height = atoi(argv[2]);

    while (true) {

        server_message smessage;
        ph_message pmessage;

        if (read(STDIN_FILENO, (void*) &smessage, sizeof(server_message)) == -1) {
              fprintf(stderr, "Error on read on child");
              fflush(stderr);
        }

        int x    = smessage.pos.x;
        int y    = smessage.pos.y;
        int advx = smessage.adv_pos.x;
        int advy = smessage.adv_pos.y;

        int curDist = ABS(x-advx) + ABS(y-advy);

        pmessage.move_request = (coordinate) { x, y };

        for (int dir = 0; dir < 4; dir++) {
            int nx = x+dxy[dir][0], ny = y+dxy[dir][1];
            if (nx >= 0 && nx < height && ny >= 0 && ny < width) {
                bool obstructed = false;
                for (int i = 0; i < smessage.object_count; i++) {
                    if (nx == smessage.object_pos[i].x &&
                            ny == smessage.object_pos[i].y) {
                        obstructed = true;
                        break;                    
                    }
                }
                if (obstructed == false) {
                    int nextDist = ABS(nx - advx) + ABS(ny - advy);
                    if (nextDist > curDist) {
                        pmessage.move_request = (coordinate) { nx, ny };
                        break;
                    }
                }
            }
        }

        usleep((1+rand()%9)*10000);

        if (write(STDOUT_FILENO, (void*) &pmessage, sizeof(ph_message)) == -1) {
            fprintf(stderr, "Error on write in child");
            fflush(stderr);
        }
    }

    exit(0);
}
