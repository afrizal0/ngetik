#ifndef EDITOR_H
#define EDITOR_H

#define CTRL_KEY(k) ((k) & 0x1f) // convert 'k' to ctrl-k 

#define APP_NAME "Textiny"
#define VERSION "1.0"

typedef struct
{
    int cursorX, cursorY;
    int screenrows;
    int screencols;
} editorConfig;

enum EditorKey {
    ARROW_UP = 1000,
    ARROW_DOWN,
    ARROW_RIGHT,
    ARROW_LEFT,
    PAGE_UP,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY,
    DELETE_KEY
};

int editorReadKey(void);
void editorProcessReadKey(void);
void editorRefreshScreen(void);
int getCursorPosition(int *rows, int *cols);

#endif