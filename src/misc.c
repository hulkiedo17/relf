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

void help(void)
{
	fprintf(stdout, "usage: relf [options]\n\n");
	fprintf(stdout, "options:\n");
	fprintf(stdout, "\t-h        - help message\n");
	fprintf(stdout, "\t-e [file] - prints elf header of specified executable file\n");
	//fprintf(stdout, "\t-p [file] - prints program header of specified executable file\n");
}