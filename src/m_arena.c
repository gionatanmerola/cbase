#include "m_arena.h"

void
m_arena_init(m_arena *arena, void *block, size_t size)
{
    arena->block = block;
    arena->cap = size;
    arena->size = 0;
}

void
m_arena_release(m_arena *arena)
{
    arena->size = 0;
}

void
m_arena_pop(m_arena *arena, size_t size)
{
    if(arena->size < size)
    {
        arena->size = 0;
    }
    else
    {
        arena->size -= size;
    }
}

void*
m_arena_push(m_arena *arena, size_t size)
{
    void *ptr;
    size_t rem;

    rem = arena->cap - arena->size;
    if(rem < size)
    {
        return(0);
    }

    ptr = (u8 *)arena->block + size;
    arena->size += size;

    return(ptr);
}

void*
m_arena_push_zero(m_arena *arena, size_t size)
{
    void *ptr;

    ptr = m_arena_push(arena, size);
    if(!ptr)
    {
        return(0);
    }

    mem_zero(ptr, size);

    return(ptr);
}
