#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <elf.h>
#include <errno.h>
#include <error.h>
#include <assert.h>
#include <sys/types.h>
#include "misc.h"

enum {
	P_NULL = 0,
	P_LOAD,
	P_DYNAMIC,
	P_INTERP,
	P_NOTE,
	P_SHLIB,
	P_PHDR,
	P_LOPROC,
	P_HIPROC,
	P_GNU_STACK,
	P_GNU_PROPERTY,
	P_GNU_RELRO,
	P_GNU_EH_FRAME
};

static const char * const program_header_type_names[] = {
	"NULL        ",
	"LOAD        ",
	"DYNAMIC     ",
	"INTERP      ",
	"NOTE        ",
	"SHLIB       ",
	"PHDR        ",
	"LOPROC      ",
	"HIPROC      ",
	"GNU_STACK   ",
	"GNU_PROPERTY",
	"GNU_RELRO   ",
	"GNU_EH_FRAME"
};

static const char* get_program_header_type(uint32_t type)
{
	switch(type)
	{
	case PT_LOAD:
		return program_header_type_names[P_LOAD];
	case PT_DYNAMIC:
		return program_header_type_names[P_DYNAMIC];
	case PT_INTERP:
		return program_header_type_names[P_INTERP];
	case PT_NOTE:
		return program_header_type_names[P_NOTE];
	case PT_SHLIB:
		return program_header_type_names[P_SHLIB];
	case PT_PHDR:
		return program_header_type_names[P_PHDR];
	case PT_LOPROC:
		return program_header_type_names[P_LOPROC];
	case PT_HIPROC:
		return program_header_type_names[P_HIPROC];
	case PT_GNU_STACK:
		return program_header_type_names[P_GNU_STACK];
	case PT_GNU_PROPERTY:
		return program_header_type_names[P_GNU_PROPERTY];
	case PT_GNU_RELRO:
		return program_header_type_names[P_GNU_RELRO];
	case PT_GNU_EH_FRAME:
		return program_header_type_names[P_GNU_EH_FRAME];
	case PT_NULL:
	default:
		return program_header_type_names[P_NULL];
	}
}

static char* get_program_header_flags(uint32_t flags)
{
	const size_t str_size = 4;	// 3 flags + \0
	char *str = NULL;

	str = malloc_wrap(str_size);

	memset(str, ' ', str_size);
	str[str_size - 1] = '\0';

	if((flags & 0x4) == PF_R)
		str[0] = 'R';
	if((flags & 0x2) == PF_W)
		str[1] = 'W';
	if((flags & 0x1) == PF_X)
		str[2] = 'E';

	return str;
}

static void print_program32_header(Elf32_Phdr *header)
{
	assert(header != NULL);

	const char *ptype = NULL;
	char *pflags = NULL;

	ptype = get_program_header_type(header->p_type);
	pflags = get_program_header_flags(header->p_flags);

	printf("  %-12s %#010x %#010x %#010x %#010x %#010x %-3s %#0x\n",
		ptype,
		header->p_offset,
		header->p_vaddr,
		header->p_paddr,
		header->p_filesz,
		header->p_memsz,
		pflags,
		header->p_align);

	free(pflags);
}

static void print_program64_header(Elf64_Phdr *header)
{
	assert(header != NULL);

	const char *ptype = NULL;
	char *pflags = NULL;

	ptype = get_program_header_type(header->p_type);
	pflags = get_program_header_flags(header->p_flags);

	printf("  %-12s %#018lx %#018lx %#018lx\n",
		ptype,
		header->p_offset,
		header->p_vaddr,
		header->p_paddr);
	printf("               %#018lx %#018lx %-5s %#0lx\n",
		header->p_filesz,
		header->p_memsz,
		pflags,
		header->p_align);

	free(pflags);
}

Elf32_Phdr* read_program32_headers(const char *filename, Elf32_Ehdr *elf_header)
{
	assert(filename != NULL);
	assert(elf_header != NULL);

	ssize_t program_headers_offset;
	FILE *fp = NULL;
	Elf32_Phdr *program_headers = NULL;

	fp = fopen_wrap(filename, "rb");

	program_headers_offset = (ssize_t)elf_header->e_phoff;
	fseek(fp, program_headers_offset, SEEK_SET);

	program_headers = malloc_wrap(sizeof(Elf32_Phdr) * elf_header->e_phnum);

	for(size_t i = 0; i < elf_header->e_phnum; i++)
		fread_wrap(&program_headers[i], sizeof(Elf32_Phdr), 1, fp);

	fclose(fp);
	return program_headers;
}

Elf64_Phdr* read_program64_headers(const char *filename, Elf64_Ehdr *elf_header)
{
	assert(filename != NULL);
	assert(elf_header != NULL);

	ssize_t program_headers_offset;
	FILE *fp = NULL;
	Elf64_Phdr *program_headers = NULL;

	fp = fopen_wrap(filename, "rb");

	program_headers_offset = (ssize_t)elf_header->e_phoff;
	fseek(fp, program_headers_offset, SEEK_SET);

	program_headers = malloc_wrap(sizeof(Elf64_Phdr) * elf_header->e_phnum);

	for(size_t i = 0; i < elf_header->e_phnum; i++)
		fread_wrap(&program_headers[i], sizeof(Elf64_Phdr), 1, fp);

	fclose(fp);
	return program_headers;
}

void print_program32_headers(Elf32_Phdr *program_headers, Elf32_Ehdr *elf_header)
{
	assert(program_headers != NULL);
	assert(elf_header != NULL);

	printf("Executable have %d program headers, starting at offset %d\n\n", elf_header->e_phnum, elf_header->e_phoff);

	printf("Program headers:\n");
	printf("  Type         Offset     VirtAddr   PhysAddr   FileSize   MemSize    Flg Align\n");

	for(size_t i = 0; i < elf_header->e_phnum; i++)
		print_program32_header(&program_headers[i]);
}

void print_program64_headers(Elf64_Phdr *program_headers, Elf64_Ehdr *elf_header)
{
	assert(program_headers != NULL);
	assert(elf_header != NULL);

	printf("Executable have %d program headers, starting at offset %ld\n\n", elf_header->e_phnum, elf_header->e_phoff);

	printf("Program headers:\n");
	printf("  Type         Offset             VirtAddr           PhysAddr\n");
	printf("               FileSize           MemSize            Flags Align\n");

	for(size_t i = 0; i < elf_header->e_phnum; i++)
		print_program64_header(&program_headers[i]);
}
