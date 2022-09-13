#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <errno.h>
#include <error.h>
#include <assert.h>
#include <sys/types.h>
#include "misc.h"

enum {
	ST_NULL = 0,
	ST_PROGBITS,
	ST_SYMTAB,
	ST_STRTAB,
	ST_RELA,
	ST_HASH,
	ST_DYNAMIC,
	ST_NOTE,
	ST_NOBITS,
	ST_REL,
	ST_SHLIB,
	ST_DYNSYM,
	ST_LOPROC,
	ST_HIPROC,
	ST_LOUSER,
	ST_HIUSER
};

static const char * const section_header_type_names[] = {
	"NULL    ",
	"PROGBITS",
	"SYMTAB  ",
	"STRTAB  ",
	"RELA    ",
	"HASH    ",
	"DYNAMIC ",
	"NOTE    ",
	"NOBITS  ",
	"REL     ",
	"SHLIB   ",
	"DYNSYM  ",
	"LOPROC  ",
	"HIPROC  ",
	"LOUSER  ",
	"HIUSER  "
};

static const char* get_section_header_type(uint32_t type)
{
	switch(type)
	{
	case SHT_NULL:
		return section_header_type_names[ST_NULL];
	case SHT_PROGBITS:
		return section_header_type_names[ST_PROGBITS];
	case SHT_SYMTAB:
		return section_header_type_names[ST_SYMTAB];
	case SHT_STRTAB:
		return section_header_type_names[ST_STRTAB];
	case SHT_RELA:
		return section_header_type_names[ST_RELA];
	case SHT_HASH:
		return section_header_type_names[ST_HASH];
	case SHT_DYNAMIC:
		return section_header_type_names[ST_DYNAMIC];
	case SHT_NOTE:
		return section_header_type_names[ST_NOTE];
	case SHT_NOBITS:
		return section_header_type_names[ST_NOBITS];
	case SHT_REL:
		return section_header_type_names[ST_REL];
	case SHT_SHLIB:
		return section_header_type_names[ST_SHLIB];
	case SHT_DYNSYM:
		return section_header_type_names[ST_DYNSYM];
	case SHT_LOPROC:
		return section_header_type_names[ST_LOPROC];
	case SHT_HIPROC:
		return section_header_type_names[ST_HIPROC];
	case SHT_LOUSER:
		return section_header_type_names[ST_LOUSER];
	case SHT_HIUSER:
		return section_header_type_names[ST_HIUSER];
	default:
		return "Unknown ";
	}
}

static char* get_section_header_flags(uint64_t flags)
{
	const size_t str_size = 7;	// 6 flags + \0
	char *str = NULL;

	str = malloc_wrap(str_size);

	memset(str, ' ', str_size);
	str[str_size - 1] = '\0';

	if((flags & 0x1) == SHF_WRITE)
		str[0] = 'W';
	if((flags & 0x2) == SHF_ALLOC)
		str[1] = 'A';
	if((flags & 0x4) == SHF_EXECINSTR)
		str[2] = 'X';
	if((flags & (1 << 4)) == SHF_MERGE)
		str[3] = 'M';
	if((flags & (1 << 5)) == SHF_STRINGS)
		str[4] = 'S';
	if((flags & (1 << 6)) == SHF_INFO_LINK)
		str[5] = 'I';

	return str;
}

static void print_section32_header(Elf32_Shdr *section_header, char *strtab_buffer, size_t count_section)
{
	assert(section_header != NULL);
	assert(strtab_buffer != NULL);

	char *section_name = strtab_buffer + section_header->sh_name;
	const char *section_type = get_section_header_type(section_header->sh_type);
	char *section_flags = get_section_header_flags(section_header->sh_flags);

	printf("  [%2zu] %-20s %-9s %08x %08x %08x %02x %6s %2d  %2d %2d\n",
		count_section,
		section_name,
		section_type,
		section_header->sh_addr,
		section_header->sh_offset,
		section_header->sh_size,
		section_header->sh_entsize,
		section_flags,
		section_header->sh_link,
		section_header->sh_info,
		section_header->sh_addralign);

	free(section_flags);
}

static void print_section64_header(Elf64_Shdr *section_header, char *strtab_buffer, size_t count_section)
{
	assert(section_header != NULL);
	assert(strtab_buffer != NULL);

	char *section_name = strtab_buffer + section_header->sh_name;
	const char *section_type = get_section_header_type(section_header->sh_type);
	char *section_flags = get_section_header_flags(section_header->sh_flags);

	printf("  [%2zu] %-20s %-18s %018lx %018lx\n",
		count_section,
		section_name,
		section_type,
		section_header->sh_addr,
		section_header->sh_offset);

	printf("       %018lx   %018lx %6s  %2d   %2d       %2ld\n",
		section_header->sh_size,
		section_header->sh_entsize,
		section_flags,
		section_header->sh_link,
		section_header->sh_info,
		section_header->sh_addralign);

	free(section_flags);
}

char* read_section32_string_table(const char *filename, Elf32_Ehdr *elf_header, Elf32_Shdr *section_headers)
{
	assert(filename != NULL);
	assert(elf_header != NULL);
	assert(section_headers != NULL);

	char *buffer = NULL;
	FILE *fp = NULL;

	buffer = malloc_wrap(section_headers[elf_header->e_shstrndx].sh_size);

	fp = fopen_wrap(filename, "rb");

	fseek(fp, (ssize_t)section_headers[elf_header->e_shstrndx].sh_offset, SEEK_SET);

	fread_wrap(buffer, section_headers[elf_header->e_shstrndx].sh_size, 1, fp);

	fclose(fp);
	return buffer;
}

char* read_section64_string_table(const char *filename, Elf64_Ehdr *elf_header, Elf64_Shdr *section_headers)
{
	assert(filename != NULL);
	assert(elf_header != NULL);
	assert(section_headers != NULL);

	char *buffer = NULL;
	FILE *fp = NULL;

	buffer = malloc_wrap(section_headers[elf_header->e_shstrndx].sh_size);

	fp = fopen_wrap(filename, "rb");

	fseek(fp, (ssize_t)section_headers[elf_header->e_shstrndx].sh_offset, SEEK_SET);

	fread_wrap(buffer, section_headers[elf_header->e_shstrndx].sh_size, 1, fp);

	fclose(fp);
	return buffer;
}

Elf32_Shdr* read_section32_headers(const char *filename, Elf32_Ehdr *elf_header)
{
	assert(filename != NULL);
	assert(elf_header != NULL);

	size_t sections_size;
	FILE *fp = NULL;
	Elf32_Shdr *section_headers = NULL;

	sections_size = elf_header->e_shentsize * elf_header->e_shnum;

	section_headers = malloc_wrap(sections_size);

	fp = fopen_wrap(filename, "rb");

	fseek(fp, (ssize_t)elf_header->e_shoff, SEEK_SET);

	fread_wrap(section_headers, sections_size, 1, fp);

	fclose(fp);

	return section_headers;
}

Elf64_Shdr* read_section64_headers(const char *filename, Elf64_Ehdr *elf_header)
{
	assert(filename != NULL);
	assert(elf_header != NULL);

	size_t sections_size;
	FILE *fp = NULL;
	Elf64_Shdr *section_headers = NULL;

	sections_size = elf_header->e_shentsize * elf_header->e_shnum;

	section_headers = malloc_wrap(sections_size);

	fp = fopen_wrap(filename, "rb");

	fseek(fp, (ssize_t)elf_header->e_shoff, SEEK_SET);

	fread_wrap(section_headers, sections_size, 1, fp);

	fclose(fp);

	return section_headers;
}

void print_section32_headers(Elf32_Shdr *section_headers, Elf32_Ehdr *elf_header, char *strtab_buffer)
{
	assert(section_headers != NULL);
	assert(elf_header != NULL);
	assert(strtab_buffer != NULL);

	printf("There are %d section headers, starting at offset %#04x\n\n", elf_header->e_shnum, elf_header->e_shoff);

	printf("Section headers:\n");
	printf("  [Nr] Name                 Type      Addr     Off      Size     ES Flg    Lk Inf Al\n");

	for(size_t i = 0; i < elf_header->e_shnum; i++)
		print_section32_header(&section_headers[i], strtab_buffer, i);
}

void print_section64_headers(Elf64_Shdr *section_headers, Elf64_Ehdr *elf_header, char *strtab_buffer)
{
	assert(section_headers != NULL);
	assert(elf_header != NULL);
	assert(strtab_buffer != NULL);

	printf("There are %d section headers, starting at offset %#04lx\n\n", elf_header->e_shnum, elf_header->e_shoff);

	printf("Section headers:\n");
	printf("  [Nr] Name                 Type               Address            Offset\n");
	printf("       Size                 EntSize            Flags Link Info    Align\n");

	for(size_t i = 0; i < elf_header->e_shnum; i++)
		print_section64_header(&section_headers[i], strtab_buffer, i);
}
