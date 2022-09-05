#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <assert.h>
#include <elf.h>
#include <sys/stat.h>
#include "misc.h"
#include "elf_header.h"

/*const char * const osabi[] = {
	"UNIX System V ABI", "HP-UX ABI", "NetBSD ABI", "Linux ABI",
	"Solaris ABI", "IRIX ABI", "FreeBSD ABI", "TRU64 UNIX ABI",
	"OpenBSD ABI", "ARM architecture ABI", "Stand-alone (embedded) ABI"
};*/

static const char * const type[] = {
	"Unknown type", "Relocatable file", "Executable file",
	"Shared object file", "Core file"
};

/*const char * const machine[] = {
	"Intel 80386", "Intel 80860", "MIPS R3000 big-endiad",
	"PowerPC", "PowerPC 64-bit", "ARM", "SPARC v9 64-bit",
	"Intel Merced", "Stanford MIPS-X", "AMD x86-64 architecture",
	"MCST Elbrus", "ARM AARCH64", "RISC-V"
};*/

static const char* get_elf_osabi(uint8_t osabi)
{
	switch(osabi)
	{
	case ELFOSABI_SYSV:
		return "UNIX System V ABI";
		break;
	case ELFOSABI_HPUX:
		return "HP-UX ABI";
		break;
	case ELFOSABI_NETBSD:
		return "NetBSD ABI";
		break;
	case ELFOSABI_LINUX:
		return "Linux ABI";
		break;
	case ELFOSABI_SOLARIS:
		return "Solaris ABI";
		break;
	case ELFOSABI_IRIX:
		return "IRIX ABI";
		break;
	case ELFOSABI_FREEBSD:
		return "FreeBSD ABI";
		break;
	case ELFOSABI_TRU64:
		return "TRU64 UNIX ABI";
		break;
	case ELFOSABI_OPENBSD:
		return "OpenBSD ABI";
		break;
	case ELFOSABI_ARM:
		return "ARM architecture ABI";
		break;
	case ELFOSABI_STANDALONE:
		return "Stand-alone (embedded) ABI";
		break;
	}

	return "unknown";
}

static const char* get_elf_machine(uint16_t machine)
{
	switch(machine)
	{
	case EM_386:
		return "Intel 80386";
		break;
	case EM_860:
		return "Intel 80860";
		break;
	case EM_MIPS:
		return "MIPS R3000 big-endiad";
		break;
	case EM_PPC:
		return "PowerPC";
		break;
	case EM_PPC64:
		return "PowerPC 64-bit";
		break;
	case EM_ARM:
		return "ARM";
		break;
	case EM_SPARCV9:
		return "SPARC v9 64-bit";
		break;
	case EM_IA_64:
		return "Intel Itanium";
		break;
	case EM_MIPS_X:
		return "Stanford MIPS-X";
		break;
	case EM_X86_64:
		return "AMD x86-64";
		break;
	case EM_MCST_ELBRUS:
		return "MCST Elbrus";
		break;
	case EM_AARCH64:
		return "ARM AARCH64";
		break;
	case EM_RISCV:
		return "RISC-V";
		break;
	}

	return "unknown";
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

int check_elf_class(const char *filename)
{
	assert(filename != NULL);

	char buffer[EI_NIDENT];
	FILE *fp = NULL;

	fp = fopen_wrap(filename, "rb");

	fread_wrap(buffer, EI_NIDENT, 1, fp);

	fclose(fp);
	return buffer[EI_CLASS];
}

Elf32_Ehdr* read_elf32_header(const char *filename)
{
	assert(filename != NULL);

	Elf32_Ehdr *hdr = NULL;
	FILE *fp = NULL;

	hdr = malloc_wrap(sizeof(Elf32_Ehdr));

	fp = fopen_wrap(filename, "rb");

	fread_wrap(hdr, sizeof(Elf32_Ehdr), 1, fp);

	fclose(fp);
	return hdr;
}

Elf64_Ehdr* read_elf64_header(const char *filename)
{
	assert(filename != NULL);

	Elf64_Ehdr *hdr = NULL;
	FILE *fp = NULL;

	hdr = malloc_wrap(sizeof(Elf64_Ehdr));

	fp = fopen_wrap(filename, "rb");

	fread_wrap(hdr, sizeof(Elf64_Ehdr), 1, fp);

	fclose(fp);
	return hdr;
}

void print_elf32_header(Elf32_Ehdr *hdr)
{
	assert(hdr != NULL);

	printf("ELF Header:\n");
	printf("  Magic: %#04x%02x%02x%02x\n",
		hdr->e_ident[EI_MAG0], hdr->e_ident[EI_MAG1],
		hdr->e_ident[EI_MAG2], hdr->e_ident[EI_MAG3]);

	const char *class = (hdr->e_ident[EI_CLASS] == ELFCLASS32) ? "32 bit" :
					(hdr->e_ident[EI_CLASS] == ELFCLASS64) ? "64 bit" :
					"unknown";
	printf("  Class:                             %s\n", class);

	const char *data = (hdr->e_ident[EI_DATA] == ELFDATA2LSB) ? "little-endiad" :
					(hdr->e_ident[EI_DATA] == ELFDATA2MSB) ? "big-endiad" :
					"unknown";
	printf("  Data:                              %s\n", data);

	printf("  Version:                           %x\n", hdr->e_ident[EI_VERSION]);

	const char *osabi = get_elf_osabi(hdr->e_ident[EI_OSABI]);
	printf("  OS ABI:                            %s\n", osabi);

	printf("  ABI Version:                       %x\n", hdr->e_ident[EI_ABIVERSION]);;

	printf("  Type:                              %s\n", type[hdr->e_type]);

	const char *machine = get_elf_machine(hdr->e_machine);
	printf("  Machine:                           %s\n", machine);

	printf("  Version:                           %#0x\n", hdr->e_version);
	printf("  Entry point address:               %#0x\n", hdr->e_entry);
	printf("  Start of program headers:          %d (bytes into file)\n", hdr->e_phoff);
	printf("  Start of section headers:          %d (bytes into file)\n", hdr->e_shoff);
	printf("  Flags:                             %#0x\n", hdr->e_flags);
	printf("  Size of this header:               %d (bytes)\n", hdr->e_ehsize);
	printf("  Size of program header:            %d (bytes)\n", hdr->e_phentsize);
	printf("  Number of program headers:         %d\n", hdr->e_phnum);
	printf("  Size of section headers:           %d (bytes)\n", hdr->e_shentsize);
	printf("  Number of section headers:         %d\n", hdr->e_shnum);
	printf("  Section header string table index: %d\n", hdr->e_shstrndx);
}

void print_elf64_header(Elf64_Ehdr *hdr)
{
	assert(hdr != NULL);

	printf("ELF Header:\n");
	printf("  Magic: %#04x%02x%02x%02x\n",
		hdr->e_ident[EI_MAG0], hdr->e_ident[EI_MAG1],
		hdr->e_ident[EI_MAG2], hdr->e_ident[EI_MAG3]);

	const char *class = (hdr->e_ident[EI_CLASS] == ELFCLASS32) ? "32 bit" :
					(hdr->e_ident[EI_CLASS] == ELFCLASS64) ? "64 bit" :
					"unknown";
	printf("  Class:                             %s\n", class);

	const char *data = (hdr->e_ident[EI_DATA] == ELFDATA2LSB) ? "little-endiad" :
					(hdr->e_ident[EI_DATA] == ELFDATA2MSB) ? "big-endiad" :
					"unknown";
	printf("  Data:                              %s\n", data);

	printf("  Version:                           %x\n", hdr->e_ident[EI_VERSION]);

	const char *osabi = get_elf_osabi(hdr->e_ident[EI_OSABI]);
	printf("  OS ABI:                            %s\n", osabi);

	printf("  ABI Version:                       %x\n", hdr->e_ident[EI_ABIVERSION]);;

	printf("  Type:                              %s\n", type[hdr->e_type]);

	const char *machine = get_elf_machine(hdr->e_machine);
	printf("  Machine:                           %s\n", machine);

	printf("  Version:                           %#0x\n", hdr->e_version);
	printf("  Entry point address:               %#0lx\n", hdr->e_entry);
	printf("  Start of program headers:          %ld (bytes into file)\n", hdr->e_phoff);
	printf("  Start of section headers:          %ld (bytes into file)\n", hdr->e_shoff);
	printf("  Flags:                             %#0x\n", hdr->e_flags);
	printf("  Size of this header:               %d (bytes)\n", hdr->e_ehsize);
	printf("  Size of program header:            %d (bytes)\n", hdr->e_phentsize);
	printf("  Number of program headers:         %d\n", hdr->e_phnum);
	printf("  Size of section headers:           %d (bytes)\n", hdr->e_shentsize);
	printf("  Number of section headers:         %d\n", hdr->e_shnum);
	printf("  Section header string table index: %d\n", hdr->e_shstrndx);
}

