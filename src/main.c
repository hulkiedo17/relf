#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <error.h>
#include <elf.h>
#include "misc.h"
#include "elf_header.h"

static void print_elf_header(const char *filename)
{
	assert(filename != NULL);

	int is_elf, elf_class;
	Elf32_Ehdr *elf32_header = NULL;
	Elf64_Ehdr *elf64_header = NULL;

	is_elf = is_elf_file(filename);
	if(is_elf != 0)
	{
		error(0, ENOEXEC, "\'%s\' is not executable file", filename);
		return;
	}

	elf_class = check_elf_class(filename);
	if(elf_class == ELFCLASS32)
	{
		elf32_header = read_elf32_header(filename);
		print_elf32_header(elf32_header);
		free(elf32_header);
	}
	else if(elf_class == ELFCLASS64)
	{
		elf64_header = read_elf64_header(filename);
		print_elf64_header(elf64_header);
		free(elf64_header);
	}
	else
	{
		error(0, EBADF, "unknown elf file class");
	}
}

int main(int argc, char **argv)
{
	if(argc != 2)
		p_err("usage: ./relf [file]\n");

	print_elf_header(argv[1]);

	return EXIT_SUCCESS;
}
