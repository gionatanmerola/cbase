#ifndef CBASE_M_ARENA
#define CBASE_M_ARENA

typedef struct
m_arena
{
    void *block;
    size_t cap;
    size_t size;
} m_arena;

void m_arena_init(m_arena *arena, void *block, size_t size);
void m_arena_release(m_arena *arena);

void m_arena_pop(m_arena *arena, size_t size);
void* m_arena_push(m_arena *arena, size_t size);
void* m_arena_push_zero(m_arena *arena, size_t size);

#endif
