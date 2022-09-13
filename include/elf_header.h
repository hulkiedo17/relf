#ifndef ELF_HEADER_H
#define ELF_HEADER_H

Elf32_Ehdr* read_elf32_header(const char *filename);
Elf64_Ehdr* read_elf64_header(const char *filename);

void print_elf32_header(Elf32_Ehdr *hdr);
void print_elf64_header(Elf64_Ehdr *hdr);

#endif