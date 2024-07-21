#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "editor.h"
#include "terminal.h"
#include "buffer.h"

editorConfig E;

int editorReadKey(void)
{
	int nread;
	char input;
	while ((nread = read(STDIN_FILENO, &input, 1)) != 1)
	{
		if (nread == -1 && errno != EAGAIN)
			die("read");
	}
	if (input == '\x1b')
	{
		char seq[3];

		if (read(STDIN_FILENO, &seq[0], 1) != 1)

			return '\x1b';
		if (read(STDIN_FILENO, &seq[1], 1) != 1)
			return '\x1b';

		/*
		\x1b[A -> Arrow Up
		\x1b[B -> Arrow Down
		\x1b[C -> Arrow Right
		\x1b[D -> Arrow Left
		*/
		if (seq[0] == '[')
		{
			if (seq[1] >= '0' && seq[1] <= '9')
			{
				if (read(STDIN_FILENO, &seq[2], 1) != 1)
					return '\x1b';
				if (seq[2] == '~')
				{
					switch (seq[1])
					{
					case '1':
						return HOME_KEY;
					case '3':
						return DELETE_KEY;
					case '4':
						return END_KEY;
					case '5':
						return PAGE_UP;
					case '6':
						return PAGE_DOWN;
					case '7':
						return HOME_KEY;
					case '8':
						return END_KEY;
					}
				}
			}
			else
			{
				switch (seq[1])
				{
				case 'A':
					return ARROW_UP;
				case 'B':
					return ARROW_DOWN;
				case 'C':
					return ARROW_RIGHT;
				case 'D':
					return ARROW_LEFT;
				case 'H':
					return HOME_KEY;
				case 'F':
					return END_KEY;
				}
			}
		}
		else if (seq[0] == '0')
		{
			switch (seq[1])
			{
			case 'H':
				return HOME_KEY;
			case 'E':
				return END_KEY;
			}
		}
		return '\x1b';
	}
	else
	{
		return input;
	}
}

void editorMoveCursor(int key)
{
	switch (key)
	{
	case ARROW_UP:
		if (E.cursorY != 0)
			E.cursorY--;
		break;
	case ARROW_LEFT:
		if (E.cursorX != 0)
			E.cursorX--;
		break;
	case ARROW_RIGHT:
		if (E.cursorX != E.screencols - 1)
			E.cursorX++;
		break;
	case ARROW_DOWN:
		if (E.cursorY != E.screenrows - 1)
			E.cursorY++;
		break;
	}
}

void editorProcessReadKey(void)
{
	int c = editorReadKey();

	switch (c)
	{
	case CTRL_KEY('x'):
		write(STDOUT_FILENO, "\x1b[2J", 4);
		write(STDOUT_FILENO, "\x1b[H", 3);
		exit(0);
		break;

	case HOME_KEY:
		E.cursorX = 0;
		break;
	case END_KEY:
		E.cursorX = E.screencols - 1;
		break;

	case PAGE_UP:
	case PAGE_DOWN:
	{
		int times = E.screenrows;
		while (times--)
		{
			editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
		}
	}
	break;

	case ARROW_UP:
	case ARROW_LEFT:
	case ARROW_DOWN:
	case ARROW_RIGHT:
		editorMoveCursor(c);
		break;
	}
}

void editorDrawRows(struct abuff *ab)
{
	int y;
	for (y = 0; y < E.screenrows; y++)
	{
		if (y >= E.numrows) {
			if (E.numrows == 0 && y == E.screenrows / 3)
			{
				/* TODO: Make home screen text colored */
				char homeScreen[80];
				int homeScreenLen = snprintf(homeScreen, sizeof(homeScreen),
																		 "\033[38;5;206m%s -- version %s\n", APP_NAME, VERSION);
				// drawHomeScreenText(homeScreenLen, ab, homeScreen);

				if (homeScreenLen > E.screencols)
					homeScreenLen = E.screencols;
				// center the welcome text
				int padding = (E.screencols - homeScreenLen) / 2;
				if (padding)
				{
					abAppend(ab, "~", 1);
					padding--;
				}
				while (padding--)
					abAppend(ab, " ", 1);

				abAppend(ab, homeScreen, homeScreenLen);

				// char tx[30];
				// int text = snprintf(tx, sizeof(tx),
				//     "\033[38;5;206m%s", "Support Palestine");

				// abAppend(ab, tx, text);
			}
			else
			{
				char tilde[10];
				int tildeLen = snprintf(tilde, sizeof(tilde),
																"\033[0;37m%c", '~');
				abAppend(ab, tilde, tildeLen);
			}
		}
		else
		{
			int len = E.row.size;
			if(len > E.screencols) len = E.screencols;
			abAppend(ab, E.row.chars, len);
		}

		abAppend(ab, "\x1b[K", 3);
		if (y < E.screenrows - 1)
			abAppend(ab, "\r\n", 2);
	}
}

// clear the screen
void editorRefreshScreen(void)
{
	struct abuff ab = ABUFF_INIT;

	abAppend(&ab, "\x1b[?25l", 6);
	abAppend(&ab, "\x1b[H", 3); // reposition cursor

	editorDrawRows(&ab);

	char buff[32];
	snprintf(buff, sizeof(buff), "\x1b[%d;%dH", E.cursorY + 1, E.cursorX + 1);
	abAppend(&ab, buff, strlen(buff)); // reposition cursor

	abAppend(&ab, "\x1b[?25h", 6);

	write(STDIN_FILENO, ab.b, ab.length);
	abFree(&ab);
}

int getCursorPosition(int *rows, int *cols)
{

	char buff[32];
	unsigned int i = 0;

	if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
		return -1;

	while (i < sizeof(buff) - 1)
	{
		if (read(STDIN_FILENO, &buff[i], 1) != 1)
			break;
		if (buff[i] == 'R')
			break;
		i++;
	}
	buff[i] = '\0';
	if (buff[0] != '\x1b' || buff[i] != '[')
		return -1;
	if (sscanf(&buff[2], "%d;%d", rows, cols) != 2)
		return -1;

	return -1;
}

void editorOpen(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (!fp) die("fopen");

	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	linelen = getline(&line, &linecap, fp);
	if(linelen != -1) {
		while (linelen > 0 && (line[linelen - 1]  == '\n' || line[linelen - 1] == '\r')) {
			linelen--;
		}
		
		E.row.size = linelen;
		E.row.chars = malloc(linelen + 1);
		memcpy(E.row.chars, line, linelen);
		E.row.chars[linelen] = '\0';
		E.numrows = 1;
	}		
	free(line);
	fclose(fp);
}
