#ifndef __LIB_UTILS_H
#define __LIB_UTILS_H

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((b) > (a) ? (a) : (b))

int tokenize(char *, char **);
char *strpdup(const char *);

#endif // __LIB_UTILS_H
