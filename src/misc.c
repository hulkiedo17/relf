#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <error.h>

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
		error(EXIT_FAILURE, errno, "cannot allocate memory");
	
	return buf;
}

size_t fread_wrap(void *buf, size_t size, size_t n, FILE *fp)
{
	assert(buf != NULL);
	assert(fp != NULL);

	size_t readed;

	readed = fread(buf, size, n, fp);
	if(readed != n)
		error(EXIT_FAILURE, errno, "fread() failed: %zu", readed);
	
	return readed;
}

void help(void)
{
	fprintf(stdout, "usage: relf [options...]\n\n");
	fprintf(stdout, "options:\n");
	fprintf(stdout, "\t-h        - help message\n");
	fprintf(stdout, "\t-e        - prints elf header\n");
	fprintf(stdout, "\t-p        - prints program headers\n");
	fprintf(stdout, "\t-s        - prints section headers\n");
	fprintf(stdout, "\t-f [file] - specifies the input executable file\n");
}
