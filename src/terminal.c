#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>

#include "terminal.h"
#include "editor.h"
#include "winsize.h"

struct termios orig_termios;
extern editorConfig E;

void initEditor(void)
{
	E.cursorX = 0; // horizontal
	E.cursorY = 0; // vertical
	if (getWindowSize(&E.screenrows, &E.screencols) == -1)
		die("getWindowSize");
}

void die(const char *s)
{
	write(STDIN_FILENO, "\x1b[2J", 4);
	write(STDIN_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}

void disableRawMode(void)
{
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsetattr");
}

void enableRawMode(void)
{
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");
	atexit(disableRawMode);

	struct termios raw = orig_termios;

	// modify flag
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // disable transmission pause
	raw.c_oflag &= ~(OPOST);																	// remove the carriage return (\n)
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}
