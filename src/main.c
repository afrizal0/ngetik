#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <unistd.h>

#include "terminal.h"
#include "editor.h"

int main(int argc, char *argv[])
{
	enableRawMode();
	initEditor();
	if(argc >= 2) {
		editorOpen(argv[1]);
	}

	while (1)
	{
		editorRefreshScreen();
		editorProcessReadKey();
	}
	return 0;
}
