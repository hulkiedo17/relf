#ifndef SECTION_HEADER
#define SECTION_HEADER

char* read_section32_string_table(const char *filename, Elf32_Ehdr *elf_header, Elf32_Shdr *section_headers);
char* read_section64_string_table(const char *filename, Elf64_Ehdr *elf_header, Elf64_Shdr *section_headers);

Elf32_Shdr* read_section32_headers(const char *filename, Elf32_Ehdr *elf_header);
Elf64_Shdr* read_section64_headers(const char *filename, Elf64_Ehdr *elf_header);

void print_section32_headers(Elf32_Shdr *section_headers, Elf32_Ehdr *elf_header, char *strtab_buffer);
void print_section64_headers(Elf64_Shdr *section_headers, Elf64_Ehdr *elf_header, char *strtab_buffer);

#endif