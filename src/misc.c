#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <error.h>

void p_err(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	exit(EXIT_FAILURE);
}

FILE* fopen_wrap(const char *filename, const char *mode)
{
	assert(filename != NULL);
	assert(mode != NULL);

	FILE *fp = NULL;

	fp = fopen(filename, mode);
	if(!fp)
		error(EXIT_FAILURE, errno, "cannot access file \'%s\'", filename);
	
	return fp;
}

void* malloc_wrap(size_t size)
{
	void *buf = NULL;

	buf = malloc(size);
	if(!buf)
		error(EXIT_FAILURE, errno, "malloc() failed");
	
	return buf;
}

size_t fread_wrap(void *buf, size_t size, size_t n, FILE *fp)
{
	size_t readed;

	readed = fread(buf, size, n, fp);
	if(readed != n)
		error(EXIT_FAILURE, errno, "fread() failed: %zu", readed);
	
	return readed;
}