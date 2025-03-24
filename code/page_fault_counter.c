#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>

typedef unsigned char u8;
typedef unsigned char uint8;
typedef unsigned long long u64;
typedef unsigned long long uint64;

static HANDLE ProcessHandle;

static u64 ReadOSPageFaultCount(void)
{
    PROCESS_MEMORY_COUNTERS_EX MemoryCounters = {};
    MemoryCounters.cb = sizeof(MemoryCounters);
    GetProcessMemoryInfo(ProcessHandle, (PROCESS_MEMORY_COUNTERS *)&MemoryCounters, sizeof(MemoryCounters));

    u64 Result = MemoryCounters.PageFaultCount;
    return Result;
}

int main_Casey()
{
    if (!ProcessHandle)
        ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());

    u64 PageSize = 4096; // NOTE(casey): This may not be the OS page size! It is merely our testing page size.
    u64 PageCount = 100;
    u64 TotalSize = PageSize*PageCount;

    printf("Page Count, Touch Count, Fault Count, Extra Faults\n");

    for(u64 TouchCount = 0; TouchCount <= PageCount; ++TouchCount)
    {
        u64 TouchSize = PageSize*TouchCount;
        u8 *Data = (u8 *)VirtualAlloc(0, TotalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

        u64 StartFaultCount = ReadOSPageFaultCount();
        for(u64 Index = 0; Index < TouchSize; ++Index)
        {
            Data[Index] = (u8)Index;
        }
        u64 EndFaultCount = ReadOSPageFaultCount();

        u64 FaultCount = EndFaultCount - StartFaultCount;

        printf("%llu, %llu, %llu, %lld\n", PageCount, TouchCount, FaultCount, (FaultCount - TouchCount));

        VirtualFree(Data, 0, MEM_RELEASE);
    }

    return 0;
}


int main_me()
{
    if (!ProcessHandle)
        ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());

    int page_size = 4096;
    int page_count = 100;
    int size = page_count * page_size;

    printf("Page Count, Touch Count, Fault Count, Extra Faults\n");

    for (int touch_count = 0; touch_count < page_count; touch_count += 1)
    {
        int size_to_touch = touch_count * page_size;
        uint8 *memory = (uint8 *) VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        uint64 pf0 = ReadOSPageFaultCount();
        for (int index = 0; index < size_to_touch; index++)
        {
            memory[index] = (u8) index;
        }
        uint64 pf1 = ReadOSPageFaultCount();

        uint64 fault_count = pf1 - pf0;

        printf("%d,%d,%llu,%llu\n", page_count,touch_count,fault_count,fault_count - touch_count);

        VirtualFree(memory, 0, MEM_RELEASE);
    }

    return 0;
}

int main()
{
    printf("----------- Casey ----------\n");
    int result_Casey = main_Casey();
    printf("----------- Me    ----------\n");
    int result_me = main_me();
    return result_Casey == result_me;
}
