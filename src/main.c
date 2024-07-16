#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#include "terminal.h"
#include "winsize.h"

int main() {	
	enableRawMode();
	initEditor();

	while(1) {
		editorRefreshScreen();
		editorProcessReadKey();
	}
	return 0;
}

