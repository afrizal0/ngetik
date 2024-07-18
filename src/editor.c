#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "editor.h"
#include "terminal.h"
#include "winsize.h"
#include "buffer.h"

editorConfig E;

int editorReadKey(void) {
	int nread;
	char input;
	while((nread = read(STDIN_FILENO, &input, 1)) != 1) {
		if(nread == -1 && errno != EAGAIN) die("read");
	}

    if(input == '\x1b') {
        char seq[3];

        if(read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';   
        if(read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
       
        /* 
        \x1b[A -> Arrow Up
        \x1b[B -> Arrow Down
        \x1b[C -> Arrow Right
        \x1b[D -> Arrow Left
        */       
        if(seq[0] == '[') {
            switch (seq[1])
            {
                case 'A': return ARROW_UP;
                case 'B': return ARROW_DOWN;
                case 'C': return ARROW_RIGHT;
                case 'D': return ARROW_LEFT;
            }
        }
        return '\x1b';
    } else {
	    return input;
    }
}

void editorMoveCursor(int key) {
    switch (key)
    {
    case ARROW_UP:
        if(E.cursorY != 0)
            E.cursorY--;    
        break;
    case ARROW_LEFT:
        if(E.cursorX != 0)
            E.cursorX--;    
        break;
    case ARROW_RIGHT:
        if(E.cursorX != E.screencols - 1)
            E.cursorX++;    
        break;
    case ARROW_DOWN:
        if(E.cursorY != E.screenrows - 1)
            E.cursorY++;    
        break;
    }
}

void editorProcessReadKey(void) {
	int c = editorReadKey();

	switch (c)
	{
	case CTRL_KEY('x'):
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
		exit(0);
		break;
    case ARROW_UP:
    case ARROW_LEFT:
    case ARROW_DOWN:
    case ARROW_RIGHT:
        editorMoveCursor(c);
        break;
	}
}

/* Draw tilde */
void editorDrawRows(struct abuff *ab ) {
    int y;
    for (y = 0; y < E.screenrows; y++)
    {
        if(y == E.screenrows / 3) {
            char homeScreen[80];
            int homeScreenLen = snprintf(homeScreen, sizeof(homeScreen),
            "%s -- version %s", APP_NAME, VERSION);
            if (homeScreenLen > E.screencols) homeScreenLen = E.screencols;
            // center the welcome text
            int padding = (E.screencols - homeScreenLen) / 2;
            if(padding) {
                abAppend(ab, "~", 1);
                padding--;
            }
            while(padding--) abAppend(ab, " ", 1);
            abAppend(ab, homeScreen, homeScreenLen);
        } else {
            abAppend(ab, "~", 1);
        }
        abAppend(ab, "\x1b[K", 3);
        if(y < E.screenrows - 1) 
            abAppend(ab, "\r\n", 2);
    }
}

// clear the screen
void editorRefreshScreen(void) {
    struct abuff ab = ABUFF_INIT;

    abAppend(&ab, "\x1b[?25l", 6);
    abAppend(&ab, "\x1b[H", 3); // reposition cursor
    
    editorDrawRows(&ab);
    
    char buff[32];
    snprintf(buff, sizeof(buff), "\x1b[%d;%dH", E.cursorY + 1 , E.cursorX + 1);
    abAppend(&ab, buff, strlen(buff)); // reposition cursor

    abAppend(&ab, "\x1b[?25h", 6);
    
    write(STDIN_FILENO, ab.b, ab.length);
    abFree(&ab);
}

int getCursorPosition(int *rows, int *cols) {

    char buff[32];
    unsigned int i = 0;

    if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

    while(i < sizeof(buff) - 1) {
        if (read(STDIN_FILENO, &buff[i], 1) != 1) break;
        if (buff[i] == 'R') break;
        i++;
    }
    buff[i] = '\0';
    if(buff[0] != '\x1b' || buff[i] != '[') return -1;
    if(sscanf(&buff[2], "%d;%d", rows, cols) != 2) return -1;


    return -1;
}