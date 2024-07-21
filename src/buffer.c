#include <stdlib.h>
#include <string.h>
#include "buffer.h"

void abAppend(struct abuff *ab, const char *s, int len)
{
	char *new = realloc(ab->b, ab->length + len);

	if (new == NULL)
		return;
	memcpy(&new[ab->length], s, len);
	ab->b = new;
	ab->length += len;
}

void abFree(struct abuff *ab)
{
	free(ab->b);
}