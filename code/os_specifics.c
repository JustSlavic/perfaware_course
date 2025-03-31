#if OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <psapi.h>

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

uint64 get_os_random_buffer(uint32 size, uint8 *buffer)
{
    BCryptGenRandom(0, buffer, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return size;
}

uint64 get_pagefaults_count()
{
    static HANDLE ProcessHandle;
    if(!ProcessHandle)
    {
        ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    }

    uint64 Result = 0;
    if (ProcessHandle)
    {
        PROCESS_MEMORY_COUNTERS_EX MemoryCounters = {};
        MemoryCounters.cb = sizeof(MemoryCounters);
        GetProcessMemoryInfo(ProcessHandle, (PROCESS_MEMORY_COUNTERS *)&MemoryCounters, sizeof(MemoryCounters));

        Result = MemoryCounters.PageFaultCount;
    }
    return Result;
}
#endif


#if OS_LINUX || OS_MAC
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

uint64 get_filesize(char const *filename)
{
    struct stat filestat;
    stat(filename, &filestat);
    uint64 result = filestat.st_size;
    return result;
}

void *allocate_pages(uint64 size)
{
    void *result = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return result;
}

void free_pages(void *memory, uint64 size)
{
    munmap(memory, size);
}

uint64 get_os_random_buffer(uint32 size, uint8 *buffer)
{
    int random_device = open("/dev/urandom", O_RDONLY);
    int bytes_read = read(random_device, buffer, size);
    close(random_device);
    return bytes_read;
}
#endif

#if OS_LINUX
#include <sys/resource.h>

uint64 get_pagefaults_count(void)
{
    struct rusage usage = {};
    getrusage(RUSAGE_SELF, &usage)l;
    uint64 result = usage.ru_minflt + usage.ru_majflt;
    return result;
}
#endif

#if OS_MAC
uint64 get_pagefaults_count(void)
{
    struct proc_taskinfo info;
    int pid = getpid();

    if (proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &info, sizeof(info)) > 0) {
        // printf("Resident Memory: %llu KB\n", info.pti_resident_size / 1024);
        // printf("Virtual Memory: %llu KB\n", info.pti_virtual_size / 1024);
        // printf("Page Faults: %d\n", info.pti_faults);
        return info.pti_faults;
    } else {
        printf("Failed to get proc info\n");
    }
    return 0;
}
#endif
