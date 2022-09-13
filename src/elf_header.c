#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <assert.h>
#include <elf.h>
#include "misc.h"
#include "elf_header.h"

enum {
	OSABI_UNKNOWN = 0,
	OSABI_SYSV,
	OSABI_HPUX,
	OSABI_NETBSD,
	OSABI_LINUX,
	OSABI_SOLARIS,
	OSABI_IRIX,
	OSABI_FREEBSD,
	OSABI_TRU64,
	OSABI_OPENBSD,
	OSABI_ARM,
	OSABI_STANDALONE
};

enum {
	MACHINE_UNKNOWN = 0,
	MACHINE_INTEL386,
	MACHINE_INTEL8086,
	MACHINE_MIPS,
	MACHINE_PPC,
	MACHINE_PPC64,
	MACHINE_ARM,
	MACHINE_SPARC,
	MACHINE_IA_64,
	MACHINE_MIPS_X,
	MACHINE_X86_64,
	MACHINE_MSCT_ELBRUS,
	MACHINE_AARCH64,
	MACHINE_RISCV
};

static const char *osabi[] = {
	"Unknown", "UNIX System V ABI", "HP-UX ABI", "NetBSD ABI",
	"Linux ABI", "Solaris ABI", "IRIX ABI", "FreeBSD ABI",
	"TRU64 UNIX ABI", "OpenBSD ABI", "ARM architecture ABI",
	"Stand-alone (embedded) ABI"
};

static const char *type[] = {
	"Unknown type", "Relocatable file", "Executable file",
	"Shared object file", "Core file"
};

static const char *machine[] = {
	"Unknown", "Intel 80386", "Intel 80860", "MIPS R3000 big-endiad",
	"PowerPC", "PowerPC 64-bit", "ARM", "SPARC v9 64-bit",
	"Intel Itanium", "Stanford MIPS-X", "AMD x86-64",
	"MCST Elbrus", "ARM AARCH64", "RISC-V"
};

static const char* get_elf_osabi(uint8_t osabi_code)
{
	switch(osabi_code)
	{
	case ELFOSABI_SYSV:
		return osabi[OSABI_SYSV];
	case ELFOSABI_HPUX:
		return osabi[OSABI_HPUX];
	case ELFOSABI_NETBSD:
		return osabi[OSABI_NETBSD];
	case ELFOSABI_LINUX:
		return osabi[OSABI_LINUX];
	case ELFOSABI_SOLARIS:
		return osabi[OSABI_SOLARIS];
	case ELFOSABI_IRIX:
		return osabi[OSABI_IRIX];
	case ELFOSABI_FREEBSD:
		return osabi[OSABI_FREEBSD];
	case ELFOSABI_TRU64:
		return osabi[OSABI_TRU64];
	case ELFOSABI_OPENBSD:
		return osabi[OSABI_OPENBSD];
	case ELFOSABI_ARM:
		return osabi[OSABI_ARM];
	case ELFOSABI_STANDALONE:
		return osabi[OSABI_STANDALONE];
	}

	return osabi[OSABI_UNKNOWN];
}

static const char* get_elf_machine(uint16_t machine_code)
{
	switch(machine_code)
	{
	case EM_386:
		return machine[MACHINE_INTEL386];
	case EM_860:
		return machine[MACHINE_INTEL8086];
	case EM_MIPS:
		return machine[MACHINE_MIPS];
	case EM_PPC:
		return machine[MACHINE_PPC];
	case EM_PPC64:
		return machine[MACHINE_PPC64];
	case EM_ARM:
		return machine[MACHINE_ARM];
	case EM_SPARCV9:
		return machine[MACHINE_SPARC];
	case EM_IA_64:
		return machine[MACHINE_IA_64];
	case EM_MIPS_X:
		return machine[MACHINE_MIPS_X];
	case EM_X86_64:
		return machine[MACHINE_X86_64];
	case EM_MCST_ELBRUS:
		return machine[MACHINE_MSCT_ELBRUS];
	case EM_AARCH64:
		return machine[MACHINE_AARCH64];
	case EM_RISCV:
		return machine[MACHINE_RISCV];
	}

	return machine[MACHINE_UNKNOWN];
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

	const char *p_osabi = get_elf_osabi(hdr->e_ident[EI_OSABI]);
	printf("  OS ABI:                            %s\n", p_osabi);

	printf("  ABI Version:                       %x\n", hdr->e_ident[EI_ABIVERSION]);;

	printf("  Type:                              %s\n", type[hdr->e_type]);

	const char *p_machine = get_elf_machine(hdr->e_machine);
	printf("  Machine:                           %s\n", p_machine);

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

	const char *p_osabi = get_elf_osabi(hdr->e_ident[EI_OSABI]);
	printf("  OS ABI:                            %s\n", p_osabi);

	printf("  ABI Version:                       %x\n", hdr->e_ident[EI_ABIVERSION]);;

	printf("  Type:                              %s\n", type[hdr->e_type]);

	const char *p_machine = get_elf_machine(hdr->e_machine);
	printf("  Machine:                           %s\n", p_machine);

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
