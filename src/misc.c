#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <elf.h>
#include <sys/stat.h>
#include <errno.h>
#include <error.h>

static const char * const program_version = "0.1";

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

int is_elf_file(const char *filename)
{
	assert(filename != NULL);

	int ret;
	char buffer[SELFMAG];
	struct stat statbuf;
	FILE *fp = NULL;

	ret = stat(filename, &statbuf);
	if(ret < 0)
		error(EXIT_FAILURE, errno, "cannot access file \'%s\'", filename);

	if(!S_ISREG(statbuf.st_mode))
		error(EXIT_FAILURE, EBADF, "\'%s\' is not an ordinary file", filename);

	fp = fopen_wrap(filename, "rb");

	fread_wrap(buffer, SELFMAG, 1, fp);

	fclose(fp);

	return strcmp(buffer, ELFMAG);
}

int get_elf_class(const char *filename)
{
	assert(filename != NULL);

	char buffer[EI_NIDENT];
	FILE *fp = NULL;

	fp = fopen_wrap(filename, "rb");

	fread_wrap(buffer, EI_NIDENT, 1, fp);

	fclose(fp);
	return buffer[EI_CLASS];
}

void help(void)
{
	fprintf(stdout, "usage: relf [options...]\n\n");
	fprintf(stdout, "options:\n");
	fprintf(stdout, "\t-v        - prints program version\n");
	fprintf(stdout, "\t-h        - prints help message\n");
	fprintf(stdout, "\t-a        - equivalent to: -e -p -s\n");
	fprintf(stdout, "\t-e        - prints elf header\n");
	fprintf(stdout, "\t-p        - prints program headers\n");
	fprintf(stdout, "\t-s        - prints section headers\n");
	fprintf(stdout, "\t-f [file] - specifies the input executable file\n");
}

void version(void)
{
	fprintf(stdout, "relf v%s\n", program_version);
}