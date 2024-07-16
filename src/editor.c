#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "editor.h"
#include "terminal.h"
#include "winsize.h"

editorConfig E;

char editorReadKey(void) {
	int nread;
	char input;
	while((nread = read(STDIN_FILENO, &input, 1)) != 1) {
		if(nread == -1 && errno != EAGAIN) die("read");
	}
	return input;
}

void editorProcessReadKey(void) {
	char c = editorReadKey();

	switch (c)
	{
	case CTRL_KEY('x'):
		exit(0);
		break;
	}
}

/* Draw tilde */
void editorDrawRows(void) {
    int y;
    for (y = 0; y < E.screenrows; y++)
    {
        write(STDIN_FILENO, "~\r\n", 3);
    }
}

// clear the screen
void editorRefreshScreen(void) {
    write(STDIN_FILENO, "\x1b[2J", 4); // clear the screen
    write(STDIN_FILENO, "\x1b[H", 3); // reposition cursor

    editorDrawRows();
    write(STDIN_FILENO, "\x1b[H", 3);
}
