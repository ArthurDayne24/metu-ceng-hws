#include "commons.h"

/*
 * arguments can be:
 * task1 <ethernet>
 * task2 <ethernet> <icmp_dest_ip>
 * task3
 */
int main(int argc, char **argv)
{
    // task1 or task2
    if ((argc == 3 && strcmp(argv[1], "task1") == 0) || (argc == 4 && strcmp(argv[1], "task2") == 0)) {
        task1_2(argc, argv);
    }
    // task3
    else if (argc == 2 && strcmp(argv[1], "task3") == 0) {
        task3(argc, argv);
    }
    else {
        error_exit("Wrong arguments are given.");
    }

    return 0;
}
