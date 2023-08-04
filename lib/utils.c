#include <string.h>
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "utils.h"
#include "stddef.h"

int tokenize(char *str, char **argv) {
	int argc = 0;
	char *next_ptr = NULL;
	for (char *save_ptr = strtok_r(str, " ", &next_ptr); save_ptr != NULL; save_ptr = strtok_r(NULL, " ", &next_ptr)) {
		argv[argc++] = save_ptr;
	}

	return argc;
}

char *strpdup(const char *str) {
    char *copy = palloc_get_page(0);
	if (copy == NULL)
		return NULL;
    strlcpy(copy, str, PGSIZE);

    return copy;
}
