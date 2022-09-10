#ifndef PROGRAM_HEADER_H
#define PROGRAM_HEADER_H

Elf32_Phdr* read_program32_headers(const char *filename, Elf32_Ehdr *elf_header);
Elf64_Phdr* read_program64_headers(const char *filename, Elf64_Ehdr *elf_header);

void print_program32_headers(Elf32_Phdr *program_headers, Elf32_Ehdr *elf_header);
void print_program64_headers(Elf64_Phdr *program_headers, Elf64_Ehdr *elf_header);

#endif