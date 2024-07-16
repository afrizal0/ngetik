#include <sys/ioctl.h>
#include <unistd.h>

#include "winsize.h"

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    
    if(ioctl(STDERR_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}