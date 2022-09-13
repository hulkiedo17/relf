#ifndef ELF_HEADER_H
#define ELF_HEADER_H

int is_elf_file(const char *filename);
int get_elf_class(const char *filename);

Elf32_Ehdr* read_elf32_header(const char *filename);
Elf64_Ehdr* read_elf64_header(const char *filename);

void print_elf32_header(Elf32_Ehdr *hdr);
void print_elf64_header(Elf64_Ehdr *hdr);

#endif