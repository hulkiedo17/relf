#ifndef MISC_H
#define MISC_H

void p_err(const char *fmt, ...);
FILE* fopen_wrap(const char *filename, const char *mode);
void* malloc_wrap(size_t size);
size_t fread_wrap(void *buf, size_t size, size_t n, FILE *fp);

#endif