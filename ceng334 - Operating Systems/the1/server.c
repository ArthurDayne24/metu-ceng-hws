#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <limits.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "commons.h"

#define SOCK_PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)
#define NOWAIT 0

typedef struct pollfd Pollfd;
typedef enum { AGENT_END, SERVER_END } pipeEnd;
typedef enum { STOP, CONTINUE } status;
typedef struct _Agent { int x; int y; int e; bool alive; } Agent;

int heigth, width;
int nobstacles, nhunters, npreys;
int alivePreys, aliveHunters;
char **grid;
int **preyFds, **hunterFds;
Agent *hunters, *preys;
pid_t *hunterPids, *preyPids;
Pollfd *agentPoll, *hunterPoll, *preyPoll;

bool isValid(int x, int y) {
    return x >= 0 && x < heigth && y >= 0 && y < width;
}

void killHunter(int hunterIdx) {
    aliveHunters--;
    close(hunterFds[hunterIdx][SERVER_END]);
    kill(hunterPids[hunterIdx], SIGTERM);
    hunters[hunterIdx].alive = false;
}

void killPrey(int preyIdx) {
    alivePreys--;
    close(preyFds[preyIdx][SERVER_END]);
    kill(preyPids[preyIdx], SIGTERM);
    preys[preyIdx].alive = false;
}

void printGrid() {
    printf("+");
    fflush(stdout);
    for (int i = 0; i < width; i++) {
        printf("-");
        fflush(stdout);
    }
    printf("+\n");
    fflush(stdout);
    for (int i = 0; i < heigth; i++) {
        printf("|");
        fflush(stdout);
        for (int j = 0; j < width; j++) {
            printf("%c", grid[i][j]);
            fflush(stdout);
        }
        printf("|\n");
        fflush(stdout);
    }
    printf("+");
    fflush(stdout);
    for (int i = 0; i < width; i++) {
        printf("-");
        fflush(stdout);
    }
    printf("+\n");
    fflush(stdout);
}

server_message message2Prey(int preyIdx) {

    server_message message;
    
    int x = preys[preyIdx].x, y = preys[preyIdx].y;
    
    message.pos = (coordinate) { x, y };
    message.object_count = 0;

    coordinate closestCoord = { x, y };
    int closestDist = INT_MAX;

    for (int i = 0; i < nhunters; i++) {
        if (hunters[i].alive == true) {
            int hx = hunters[i].x, hy = hunters[i].y;
            int newDist = ABS(x - hx) + ABS(y - hy);

            if (newDist < closestDist) {
                closestDist = newDist;
                closestCoord.x = hx;
                closestCoord.y = hy;
            }
        }
    }

    message.adv_pos = closestCoord;

    for (int dir = 0; dir < 4; dir++) {
        int nx = message.pos.x + dxy[dir][0], ny = message.pos.y + dxy[dir][1];
        if (isValid(nx, ny) == true && (grid[nx][ny] == 'X' || grid[nx][ny] == 'P')) {
            message.object_pos[message.object_count++] = (coordinate) { nx, ny };
        }
    }

    for (int i = message.object_count; i < 4; i++) {
        message.object_pos[i] = (coordinate) { 0, 0 };
    }
    
    return message;
}

server_message message2Hunter(int hunterIdx) {

    server_message message;

    int x = hunters[hunterIdx].x, y = hunters[hunterIdx].y;
    
    message.pos = (coordinate) { x, y };
    message.object_count = 0;

    coordinate closestCoord = { x, y };
    int closestDist = INT_MAX;    

    for (int i = 0; i < npreys; i++) {
        if (preys[i].alive == true) {
            int px = preys[i].x, py = preys[i].y;
            int newDist = ABS(x - px) + ABS(y - py);

            if (newDist < closestDist) {
                closestDist = newDist;
                closestCoord.x = px;
                closestCoord.y = py;
            }
        }
    }

    message.adv_pos = closestCoord;

    for (int dir = 0; dir < 4; dir++) {
        int nx = message.pos.x + dxy[dir][0], ny = message.pos.y + dxy[dir][1];
        if (isValid(nx, ny) && (grid[nx][ny] == 'X' || grid[nx][ny] == 'H')) {
            message.object_pos[message.object_count++] = (coordinate) { nx, ny };
        }
    }

    for (int i = message.object_count; i < 4; i++) {
        message.object_pos[i] = (coordinate) { 0, 0 };
    }

    return message;
}

int main() {   
    scanf("%d %d", &width, &heigth);

    // Allocate table and reset
    grid = malloc(heigth * sizeof(char*));
    int **indexTable = malloc(heigth * sizeof(int*));

    for (int i = 0; i < heigth; i++) {
        grid[i] = malloc(width);
        memset(grid[i], ' ', width);
        indexTable[i] = calloc(width, sizeof(int));
    }

    // Set table for obs.
    scanf("%d", &nobstacles);
    for (int i = 0; i < nobstacles; i++) {
        int x, y;
        scanf("%d %d", &x, &y);
        grid[x][y] = 'X';
    }

    // Set hunters, preys and table
    scanf("%d", &nhunters);

    hunters = malloc(nhunters * sizeof(Agent));
    hunterFds = malloc(sizeof(int*) * nhunters);

    for (int i = 0; i < nhunters; i++) {
        int x, y, e;
        scanf("%d %d %d", &x, &y, &e);

        grid[x][y] = 'H';
        indexTable[x][y] = i;
        hunters[i] = (Agent) { x, y, e, true };
        hunterFds[i] = calloc(2, sizeof(int));
    }

    scanf("%d", &npreys);

    preys = malloc(npreys * sizeof(Agent));
    preyFds = malloc(npreys * sizeof(int*));

    for (int i = 0; i < npreys; i++) {
        int x, y, e;
        scanf("%d %d %d", &x, &y, &e);
        
        grid[x][y] = 'P';
        indexTable[x][y] = i;
        preys[i] = (Agent) { x, y, e, true };
        preyFds[i] = calloc(2, sizeof(int));
    }

    // Allocate pollfd array for all agents
    // [0, nhunters) + [nhunters, nhunters + npreys)
    int nagents = nhunters + npreys;
    agentPoll  = malloc(nagents * sizeof(Pollfd));
    
    hunterPoll = agentPoll;
    preyPoll   = agentPoll + nhunters;

    for (int i = 0; i < nhunters; i++) {
        SOCK_PIPE(hunterFds[i]);
        hunterPoll[i] = (Pollfd) { hunterFds[i][SERVER_END], POLLIN, 0 };
    }

    for (int i = 0; i < npreys; i++) {
        SOCK_PIPE(preyFds[i]);
        preyPoll[i] = (Pollfd) { preyFds[i][SERVER_END], POLLIN, 0 };
    }

    // Print grid
    printGrid();

    // Init **argv array
    char *agentArgv[4];
    agentArgv[0] = calloc(9, 1);
    agentArgv[1] = calloc(11, 1);
    agentArgv[2] = calloc(11, 1);
    agentArgv[3] = NULL;
    sprintf(agentArgv[1], "%d", width);
    sprintf(agentArgv[2], "%d", heigth);

    // Spawn hunters
    strcpy(agentArgv[0], "./hunter");
    
    hunterPids = malloc(nhunters * sizeof(pid_t));
    
    for (int i = 0; i < nhunters; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork ERROR, this should not be the case :(\n");
            fflush(stderr);
        }
        else if (pid == 0) { // AGENT
            int agentEnd = hunterFds[i][AGENT_END];
            
            dup2(agentEnd, STDIN_FILENO);
            dup2(agentEnd, STDOUT_FILENO);

            for (int j = 0; j < nhunters; j++) {
                close(hunterFds[j][AGENT_END]);
                close(hunterFds[j][SERVER_END]);
            }

            for (int j = 0; j < npreys; j++) {
                close(preyFds[j][AGENT_END]);
                close(preyFds[j][SERVER_END]);
            }
    
            if (execvp("./hunter", agentArgv) < 0) {
                fprintf(stderr, "Execvp ERROR, this should not be the case :(\n");
                fflush(stderr);
            }
        }

        else { // if (pid > 0) { SERVER
            hunterPids[i] = pid;

            close(hunterFds[i][AGENT_END]);            
        }
    }

    // Spawn preys
    strcpy(agentArgv[0], "./prey");
    
    preyPids = malloc(npreys * sizeof(pid_t));
    
    for (int i = 0; i < npreys; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork ERROR, this should not be the case :(\n");
            fflush(stderr);
        }
        else  if (pid == 0) { // AGENT
            int agentEnd = preyFds[i][AGENT_END];

            dup2(agentEnd, STDIN_FILENO);
            dup2(agentEnd, STDOUT_FILENO);

            for (int j = 0; j < npreys; j++) {
                close(preyFds[j][AGENT_END]);
                close(preyFds[j][SERVER_END]);
            }
        
            for (int j = 0; j < nhunters; j++) {
                close(hunterFds[j][AGENT_END]);
                close(hunterFds[j][SERVER_END]);
            }

            if (execvp("./prey", agentArgv) < 0) {
                fprintf(stderr, "Execvp ERROR, this should not be the case :(\n");
                fflush(stderr);
            }
        }
        else { // if (pid > 0) { SERVER
            preyPids[i] = pid;

            close(preyFds[i][AGENT_END]);            
        }
    }

    // Send initial messages to agents
    for (int i = 0; i < nhunters; i++) {
        server_message serverMessage = message2Hunter(i);

        if (write(hunterFds[i][SERVER_END], (void*) &serverMessage,
                    sizeof(server_message)) == -1) {
            fprintf(stderr, "ERROR on write to hunter id = %d\n", i);
            fflush(stderr);
        }
    }

    for (int i = 0; i < npreys; i++) {
        server_message serverMessage = message2Prey(i);

        if (write(preyFds[i][SERVER_END], (void*) &serverMessage,
                    sizeof(server_message)) == -1) {
            fprintf(stderr, "ERROR on write to prey id = %d\n", i);
            fflush(stderr);
        }
    }

    alivePreys = npreys;
    aliveHunters = nhunters;

    status simStatus = (aliveHunters > 0 && alivePreys > 0) ? CONTINUE : STOP;

    while (simStatus == CONTINUE) {
        int retval;
        retval = poll(agentPoll, nagents, NOWAIT);

        if (retval < 0) {
            fprintf(stderr, "Poll listen ERROR, this should not be the case :(\n");
            fflush(stderr);
        }
        /*
        else if (retval == 0) {
            > Ignore timeout 
        }
        */
        else if (retval > 0) {
            
            bool isStateChanged = false;
            
            for (int i = 0; i < nagents && simStatus == CONTINUE; i++) {
            
                if ((agentPoll[i].revents & POLLIN) == POLLIN) {
                    
                    if (i >= nhunters) {
                        // I m a prey !
                        
                        int preyIdx = i - nhunters;

                        if (preys[preyIdx].alive == false) {
                            continue;
                        }
                        
                        ph_message message;

                        int serverEnd = preyFds[preyIdx][SERVER_END];
                        
                        if (read(serverEnd, (void*) &message,
                                    sizeof(ph_message)) == -1) {
                            fprintf(stderr, "ERROR: on read prey id = %d\n", preyIdx);
                            fflush(stderr);
                        }
                    
                        int x = preys[preyIdx].x, y = preys[preyIdx].y;
                        int nx = message.move_request.x, ny = message.move_request.y;
                        int cell = grid[nx][ny];

                        if ((x == nx && y == ny) || cell == 'P') {
                            server_message serverMessage = message2Prey(preyIdx);

                            if (write(serverEnd, (void*) &serverMessage,
                                        sizeof(server_message)) == -1) {
                                fprintf(stderr, "ERROR on write prey id = %d\n", preyIdx);
                                fflush(stderr);
                            }
                        }

                        else {
                            if (cell == ' ') {
                                grid[x][y] = ' ';
                                
                                grid[nx][ny] = 'P';
                                indexTable[nx][ny] = preyIdx;

                                preys[preyIdx].x = nx;
                                preys[preyIdx].y = ny;
                           
                                server_message serverMessage = message2Prey(preyIdx);

                                if (write(serverEnd, (void*) &serverMessage,
                                            sizeof(server_message)) == -1) {
                                    fprintf(stderr, "ERROR on write prey id = %d\n",
                                            preyIdx);
                                    fflush(stderr);
                                }
                            }
                            else { // if (cell == 'H')
                                grid[x][y] = ' ';

                                int hunterIdx = indexTable[nx][ny];

                                // Kill prey and get/set energy
                                
                                hunters[hunterIdx].e += preys[preyIdx].e;
                               
                                killPrey(preyIdx);

                                if (alivePreys == 0) {
                                    simStatus = STOP;
                                }
                            }
                            isStateChanged = true;
                        }
                    }
                    else {
                        // I m a hunter !

                        ph_message message;
                        
                        int hunterIdx = i;

                        int serverEnd = hunterFds[hunterIdx][SERVER_END];

                        if (read(serverEnd, (void*) &message,
                                    sizeof(ph_message)) == -1) {
                            fprintf(stderr, "ERROR on read hunter id = %d\n",
                                    hunterIdx);
                            fflush(stderr);
                        }

                        int x = hunters[hunterIdx].x, y = hunters[hunterIdx].y;
                        int nx = message.move_request.x, ny = message.move_request.y;
                        int cell = grid[nx][ny];

                        // ASSUMPTION
                        // Assumed that if energy decreases to zero it dies
                        // But if it also eats a prey at the same time,
                        // then it does not die

                        if ((x == nx && y == ny) || cell == 'H') {
                            server_message serverMessage = message2Hunter(hunterIdx);

                            if (write(serverEnd, (void*) &serverMessage,
                                        sizeof(server_message)) == -1) {
                                fprintf(stderr, "ERROR on write hunter id = %d\n",
                                        hunterIdx);
                                fflush(stderr);
                            }
                        }
                        else {
                            if (cell == 'P') {
                                int preyIdx = indexTable[nx][ny];
                                
                                grid[x][y] = ' ';

                                grid[nx][ny] = 'H';
                                indexTable[nx][ny] = hunterIdx;

                                hunters[hunterIdx].x = nx;
                                hunters[hunterIdx].y = ny;

                                // Kill prey and get/set energy
                                
                                hunters[hunterIdx].e += (preys[preyIdx].e-1);
                               
                                killPrey(preyIdx);

                                if (alivePreys == 0) {
                                    simStatus = STOP;
                                }
                                else {
                                    server_message serverMessage =
                                        message2Hunter(hunterIdx);

                                    if (write(serverEnd, (void*) &serverMessage,
                                                sizeof(server_message)) == -1) {
                                        fprintf(stderr, "ERROR on write hunter id = %d\n",
                                                hunterIdx);
                                        fflush(stderr);
                                    }
                                }

                            }
                            else { // if (cell == ' ')
                                if (hunters[hunterIdx].e > 1) {
                                    
                                    grid[x][y] = ' ';

                                    grid[nx][ny] = 'H';
                                    indexTable[nx][ny] = hunterIdx;

                                    hunters[hunterIdx].x = nx;
                                    hunters[hunterIdx].y = ny;

                                    hunters[hunterIdx].e--;
                                    
                                    server_message serverMessage =
                                        message2Hunter(hunterIdx);

                                    if (write(serverEnd, (void*) &serverMessage,
                                                sizeof(server_message)) == -1) {
                                        fprintf(stderr, "ERROR on write hunter id = %d\n",
                                                hunterIdx);
                                        fflush(stderr);
                                    }
                                }
                                else {
                                    grid[x][y] = ' ';

                                    killHunter(hunterIdx);

                                    if (aliveHunters == 0) {
                                        simStatus = STOP;
                                    }
                                }
                            }
                            isStateChanged = true;
                        }
                    }
                }
            }
            if (isStateChanged == true) {
                printGrid();
            }
        }
    }
    
    for (int i = 0; i < npreys; i++) {
        if (preys[i].alive == true) {
            killPrey(i);
        }
    }

    for (int i = 0; i < nhunters; i++) {
        if (hunters[i].alive == true) {
            killHunter(i);
        }
    }

    for (int i = 0; i < heigth; i++) {
        free(grid[i]);
        free(indexTable[i]);
    }

    for (int i = 0; i < nhunters; i++) {
        free(hunterFds[i]);
    } 

    for (int i = 0; i < npreys; i++) {
        free(preyFds[i]);
    }

    free(grid);
    free(indexTable);
    free(agentPoll);
    free(hunters);
    free(preys);
    free(hunterPids);
    free(preyPids);
    free(hunterFds);
    free(preyFds);
    free(agentArgv[0]);
    free(agentArgv[1]);
    free(agentArgv[2]);

    while (wait(NULL) > 0);

    return 0;
}

