#ifndef OS_SPECIFICS_H
#define OS_SPECIFICS_H

uint64 get_filesize(char const *filename);
void *allocate_pages(uint64 size);
void free_pages(void *memory, uint64 size);
uint64 get_os_random_buffer(uint32 size, uint8 *buffer);
uint64 get_pagefaults_count(void);

#endif // OS_SPECIFICS_H
