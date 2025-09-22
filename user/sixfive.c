#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// separators for numbers
char *seps = " -\r\t\n./,";

void process_file(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("cannot open %s\n", filename);
        return;
    }
    char buf[1];
    char numbuf[32];
    int npos = 0;
    while (read(fd, buf, 1) == 1) {
        char c = buf[0];
        if (c >= '0' && c <= '9') {
            if (npos < sizeof(numbuf) - 1) {
                numbuf[npos++] = c;
            }
        } else {
            if (npos > 0) {
                numbuf[npos] = '\0';
                int val = atoi(numbuf);
                if (val % 5 == 0 || val % 6 == 0) {
                    printf("%d\n", val);
                }
                npos = 0;
            }
        }
    }
    if (npos > 0) {
        numbuf[npos] = '\0';
        int val = atoi(numbuf);
        if (val % 5 == 0 || val % 6 == 0) {
            printf("%d\n", val);
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("text file, not included\n");
        exit(1);
    }
    for (int i = 1; i < argc; i++) {
        process_file(argv[i]);
    }
    exit(0);
}

