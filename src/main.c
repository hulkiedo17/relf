#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <errno.h>
#include <error.h>
#include <elf.h>
#include "misc.h"
#include "elf_header.h"
#include "program_header.h"
#include "section_header.h"

static void print_elf_header(const char *filename)
{
	if(!filename)
		error(EXIT_FAILURE, EINVAL, "you did not provide input file");

	int is_elf, elf_class;
	Elf32_Ehdr *elf32_header = NULL;
	Elf64_Ehdr *elf64_header = NULL;

	is_elf = is_elf_file(filename);
	if(is_elf != 0)
	{
		error(0, ENOEXEC, "\'%s\' is not executable file", filename);
		return;
	}

	elf_class = get_elf_class(filename);
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
	if(!filename)
		error(EXIT_FAILURE, EINVAL, "you did not provide input file");

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

	elf_class = get_elf_class(filename);
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

static void print_section_header(const char *filename)
{
	if(!filename)
		error(EXIT_FAILURE, EINVAL, "you did not provide input file");

	int is_elf, elf_class;
	char *section_strtab_buffer = NULL;
	Elf32_Ehdr *elf32_header = NULL;
	Elf64_Ehdr *elf64_header = NULL;
	Elf32_Shdr *section32_headers = NULL;
	Elf64_Shdr *section64_headers = NULL;

	is_elf = is_elf_file(filename);
	if(is_elf != 0)
	{
		error(0, ENOEXEC, "\'%s\' is not executable file", filename);
		return;
	}

	elf_class = get_elf_class(filename);
	if(elf_class == ELFCLASS32)
	{
		elf32_header = read_elf32_header(filename);
		section32_headers = read_section32_headers(filename, elf32_header);
		section_strtab_buffer = read_section32_string_table(filename, elf32_header, section32_headers);

		print_section32_headers(section32_headers, elf32_header, section_strtab_buffer);

		free(elf32_header);
		free(section32_headers);
		free(section_strtab_buffer);
	}
	else if(elf_class == ELFCLASS64)
	{
		elf64_header = read_elf64_header(filename);
		section64_headers = read_section64_headers(filename, elf64_header);
		section_strtab_buffer = read_section64_string_table(filename, elf64_header, section64_headers);

		print_section64_headers(section64_headers, elf64_header, section_strtab_buffer);

		free(elf64_header);
		free(section64_headers);
		free(section_strtab_buffer);
	}
	else
		error(0, EBADF, "unknown elf file class");
}

int main(int argc, char **argv)
{
	int result;
	bool is_elf_header = false;
	bool is_program_header = false;
	bool is_section_header = false;
	char *input_file = NULL;
	const char * const shortopts = "hepsf:";

	while((result = getopt(argc, argv, shortopts)) != -1)
	{
		switch(result) {
		case 'h':
			help();
			exit(EXIT_SUCCESS);
		case 'e':
			is_elf_header = true;
			break;
		case 'p':
			is_program_header = true;
			break;
		case 's':
			is_section_header = true;
			break;
		case 'f':
			input_file = strdup(optarg);
			break;
		}
	}

	if(is_elf_header)
		print_elf_header(input_file);
	if(is_program_header)
		print_program_header(input_file);
	if(is_section_header)
		print_section_header(input_file);

	free(input_file);
	return EXIT_SUCCESS;
}
