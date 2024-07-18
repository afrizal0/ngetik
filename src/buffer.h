#ifndef BUFFER_H
#define BUFFER_H

// append buffer
struct abuff
{
    char *b;
    int length;
};
#define ABUFF_INIT {NULL, 0}

void abAppend(struct abuff *ab, const char *s, int len);
void abFree(struct abuff *ab);

#endif
