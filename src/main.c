#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>
#include <errno.h>
#include <error.h>
#include <elf.h>
#include "misc.h"
#include "elf_header.h"
#include "program_header.h"

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
		error(0, EBADF, "unknown elf file class");
}

static void print_program_header(const char *filename)
{
	assert(filename != NULL);

	int is_elf, elf_class;
	Elf32_Ehdr *elf32_header = NULL;
	Elf64_Ehdr *elf64_header = NULL;
	Elf32_Phdr *program32_headers = NULL;
	Elf64_Phdr *program64_headers = NULL;

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
		program32_headers = read_program32_headers(filename, elf32_header);
		print_program32_headers(program32_headers, elf32_header);

		free(program32_headers);
		free(elf32_header);
	}
	else if(elf_class == ELFCLASS64)
	{
		elf64_header = read_elf64_header(filename);
		program64_headers = read_program64_headers(filename, elf64_header);
		print_program64_headers(program64_headers, elf64_header);

		free(program64_headers);
		free(elf64_header);
	}
	else
		error(0, EBADF, "unknown elf file class");
}

int main(int argc, char **argv)
{
	int result;
	const char * const shortopts = "he:p:";

	while((result = getopt(argc, argv, shortopts)) != -1)
	{
		switch(result) {
		case 'h':
			help();
			exit(EXIT_SUCCESS);
		case 'e':
			print_elf_header(optarg);
			break;
		case 'p':
			print_program_header(optarg);
			break;
		}
	}

	return EXIT_SUCCESS;
}
