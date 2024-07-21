#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

#include "winsize.h"
#include "editor.h"

int getWindowSize(int *rows, int *cols)
{
	struct winsize ws;

	if (ioctl(STDERR_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
	{
		if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
			return -1;
		editorReadKey();
		return getCursorPosition(rows, cols);
	}
	else
	{
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}