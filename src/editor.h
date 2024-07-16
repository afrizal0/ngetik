#ifndef EDITOR_H
#define EDITOR_H

#define CTRL_KEY(k) ((k) & 0x1f) // convert 'k' to ctrl-k 


typedef struct
{
    int screenrows;
    int screencols;
} editorConfig;

// void initEditor(void);
char editorReadKey(void);
void editorProcessReadKey(void);
void editorRefreshScreen(void);
void editorDrawRows(void);

#endif