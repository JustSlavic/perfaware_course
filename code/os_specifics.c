#if OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>

#pragma comment (lib, "bcrypt.lib")

uint64 get_filesize(char const *filename)
{
    HANDLE handle = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    LARGE_INTEGER result;
    BOOL success = GetFileSizeEx(handle, &result);
    CloseHandle(handle);
    return result.QuadPart;
}
void *allocate_pages(uint64 size)
{
    void *result = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    return result;
}
void free_pages(void *memory, uint64 size)
{
    VirtualFree(memory, 0, MEM_RELEASE);
}
void get_os_random_buffer(uint32 size, uint8 *buffer)
{
    BCryptGenRandom(0, buffer, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
}
#endif


#if OS_LINUX
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <libproc.h>
uint64 get_filesize(char const *filename)
{
    struct stat filestat;
    stat(filename, &filestat);
    uint64 result = filestat.st_size;
    return result;
}
void *allocate_pages(uint64 size)
{
    void *result = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return result;
}
void free_pages(void *memory, uint64 size)
{
    munmap(memory, filesize);
}
#endif
