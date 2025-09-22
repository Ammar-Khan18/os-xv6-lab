#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("sleep is missing an argument.\n");
        exit(1);
    }

    int secs = atoi(argv[1]);
    if (secs <= 0) {
        exit(0);
    }

    const int TICKS_PER_SEC = 100;
    pause(secs * TICKS_PER_SEC);
    exit(0);
}

