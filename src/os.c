#include "os.h"

#ifdef CBASE_WIN32

#include <windows.h>

#define OS_LARGE_INTEGER_TO_SIZE_T(large_int)\
            (((unsigned long long)((large_int).u.LowPart) << 0) |\
             ((unsigned long long)((large_int).u.HighPart) << 32));

void*
os_memory_alloc(size_t size)
{
    void *result = 0;

    HANDLE process_heap = GetProcessHeap();
    if(!process_heap)
    {
        return(0);
    }

    result = (void *)HeapAlloc(process_heap, 0, size);
    return(result);
}

void
os_memory_free(void *ptr)
{
    VirtualFree(ptr, 0, MEM_RELEASE | MEM_DECOMMIT);
}

int
os_file_exists(char *file_path)
{
    HANDLE h_file;

    h_file = CreateFileA(
        file_path,
        GENERIC_READ, FILE_SHARE_READ,
        0, OPEN_EXISTING, 0, 0);
    if(h_file == INVALID_HANDLE_VALUE)
    {
        return(0);
    }

    return(1);
}

size_t
os_file_size(char *file_path)
{
    size_t file_size;
    HANDLE h_file;
    LARGE_INTEGER file_size_large_integer;
    BOOL operation_result;

    file_size = 0;
    h_file = CreateFileA(
        file_path,
        GENERIC_READ, FILE_SHARE_READ,
        0, OPEN_EXISTING, 0, 0);
    if(h_file == INVALID_HANDLE_VALUE)
    {
        return(0);
    }

    operation_result = GetFileSizeEx(h_file, &file_size_large_integer);
    if(!operation_result)
    {
        return(0);
    }

    CloseHandle(h_file);

    file_size = OS_LARGE_INTEGER_TO_SIZE_T(file_size_large_integer);

    return(file_size);
}

size_t
os_file_read(char *file_path, void *dest, size_t num_bytes)
{
    size_t file_size;
    size_t bytes_to_read, bytes_read;
    HANDLE h_file;
    BOOL operation_result;

    if(!os_file_exists(file_path))
    {
        return(0);
    }

    file_size = os_file_size(file_path);
    if(file_size == 0)
    {
        return(0);
    }

    h_file = CreateFileA(
        file_path,
        GENERIC_READ, FILE_SHARE_READ,
        0, OPEN_EXISTING, 0, 0);
    if(h_file == INVALID_HANDLE_VALUE)
    {
        return(0);
    }

    bytes_to_read = num_bytes;
    if(bytes_to_read > file_size)
    {
        bytes_to_read = file_size;
    }
    bytes_read = 0;
    while(bytes_to_read > 0)
    {
        DWORD bytes_to_read_dword;
        DWORD bytes_read_dword;

        bytes_to_read_dword = (DWORD)(bytes_to_read & 0xffffffff);

        operation_result = ReadFile(
            h_file, dest,
            bytes_to_read_dword, &bytes_read_dword, 0);
        if(!operation_result || bytes_read_dword == 0)
        {
            return(bytes_read);
        }

        bytes_read += (size_t)bytes_read_dword;
        bytes_to_read -= (size_t)bytes_read_dword;
    }

    CloseHandle(h_file);

    return(bytes_read);
}

size_t
os_file_write(char *file_path, void *src, size_t num_bytes)
{
    size_t bytes_to_write, bytes_written;
    HANDLE h_file;
    BOOL operation_result;

    h_file = CreateFileA(
        file_path,
        GENERIC_WRITE, 0,
        0, CREATE_ALWAYS, 0, 0);
    if(h_file == INVALID_HANDLE_VALUE)
    {
        return(0);
    }

    bytes_to_write = num_bytes;
    bytes_written = 0;
    while(bytes_to_write > 0)
    {
        DWORD bytes_to_write_dword;
        DWORD bytes_written_dword;

        bytes_to_write_dword = (DWORD)(bytes_to_write & 0xffffffff);

        operation_result = WriteFile(
            h_file, src,
            bytes_to_write_dword, &bytes_written_dword, 0);
        if(!operation_result || bytes_written_dword == 0)
        {
            return(bytes_written);
        }

        bytes_written += (size_t)bytes_written_dword;
        bytes_to_write -= (size_t)bytes_written_dword;
    }

    CloseHandle(h_file);

    return(bytes_written);
}

unsigned long long
os_time_now_microseconds(void)
{
    unsigned long long result = 0;
    LARGE_INTEGER performance_counter = {0};
    LARGE_INTEGER performance_frequency = {0};
    unsigned long long ticks, ticks_per_second;

    if( QueryPerformanceCounter(&performance_counter) &&
        QueryPerformanceFrequency(&performance_frequency))
    {
        ticks_per_second = OS_LARGE_INTEGER_TO_SIZE_T(performance_frequency);
        ticks = OS_LARGE_INTEGER_TO_SIZE_T(performance_counter);
        result = ticks*1000000/ticks_per_second;
    }

    return(result);
}

typedef struct
os_win32_loaded_lib
{
    int loaded;
    HMODULE h_module;
} os_win32_loaded_lib;

#define OS_WIN32_MAX_LOADED_LIBS 30
os_win32_loaded_lib os_win32_loaded_libs[OS_WIN32_MAX_LOADED_LIBS] = {0};

int
os_lib_load(char *name)
{
    HMODULE h_module;
    int i, free_slot;

    h_module = LoadLibraryA(name);
    if(h_module == NULL)
    {
        return(0);
    }

    free_slot = -1;
    for(i = 1;
        i <= OS_WIN32_MAX_LOADED_LIBS;
        ++i)
    {
        if(!os_win32_loaded_libs[i - 1].loaded)
        {
            free_slot = i - 1;
            break;
        }
    }

    if(free_slot < 0)
    {
        return(0);
    }

    os_win32_loaded_libs[free_slot].loaded = 1;
    os_win32_loaded_libs[free_slot].h_module = h_module;

    return(free_slot + 1);
}

os_proc
os_proc_get(int os_lib, char *proc_name)
{
    os_proc result;

    result = (os_proc)0;
    if(os_lib <= 0 || os_lib > OS_WIN32_MAX_LOADED_LIBS)
    {
        return((os_proc)0);
    }

    if( !os_win32_loaded_libs[os_lib - 1].loaded ||
        os_win32_loaded_libs[os_lib - 1].h_module == NULL)
    {
        return((os_proc)0);
    }

    result = (os_proc)GetProcAddress(
        os_win32_loaded_libs[os_lib - 1].h_module,
        proc_name);

    return(result);
}

void
os_lib_release(int os_lib)
{
    if(os_lib <= 0 || os_lib > OS_WIN32_MAX_LOADED_LIBS)
    {
        return;
    }

    if( !os_win32_loaded_libs[os_lib - 1].loaded ||
        os_win32_loaded_libs[os_lib - 1].h_module == NULL)
    {
        return;
    }

    FreeLibrary(os_win32_loaded_libs[os_lib - 1].h_module);
}

#undef OS_LARGE_INTEGER_TO_SIZE_T

#else

#error "OS not specified (supported OS: win32)"

#endif
