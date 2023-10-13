#ifndef CBASE_OS
#define CBASE_OS

#include <stddef.h>

/* Memory */
void* os_memory_alloc(size_t size);
void  os_memory_free(void *ptr);

/* File I/O */
int    os_file_exists(char *file_path);
size_t os_file_size(char *file_path);
size_t os_file_read(char *file_path, void *dest, size_t num_bytes);
size_t os_file_write(char *file_path, void *src, size_t num_bytes);

/* Time */
unsigned long long os_time_now_microseconds(void);

/* Libraries */
typedef void (*os_proc)();
int     os_lib_load(char *name);
os_proc os_proc_get(int os_lib, char *proc_name);
void    os_lib_release(int os_lib);

#endif
