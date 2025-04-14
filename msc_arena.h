#ifndef MISCELLUS_ARENA
#define MISCELLUS_ARENA

#ifndef MISCELLUS_TYPES_H
#define MISCELLUS_TYPES_H
#include <stdint.h>
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef size_t usize;
typedef uintptr_t uptr;
typedef intptr_t iptr;
#endif // MISCELLUS_TYPES_H

#ifndef msc_is_pow2
#define msc_is_pow2(v) (0 == ((v) & ((v) - 1)))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct msc_arena_t {
    u8 *base;
    u8 *at;
    u8 *end;
    struct msc_arena_t *next;
} msc_arena;

void msc_arena_init(msc_arena *arena, usize capacity);
void *msc_align_ptr(void *ptr, usize align);
void *msc_arena_alloc(msc_arena *arena, usize size, usize align);
void msc_arena_reset(msc_arena *arena);

#ifdef __cplusplus
}
#endif

#endif // MISCELLUS_ARENA

#ifdef MSC_ARENA_IMPLEMENTATION

void *msc_arena_alloc(msc_arena *arena, usize size, usize align) {
    if (arena->at + size >= arena->end) {
        return NULL; // TODO(jkkammersgaard): grow arena
    }

    void *result = arena->at;

    arena->at += size;

    return result;
}

void *msc_align_ptr(void *ptr, usize align) {
    uptr p = (uptr)ptr;

    if ((align & (align - 1)) == 0) {
        p = (p + align - 1) & ~(align - 1);
    } else {
        p = ((p + align - 1) / align) * align;
    }

    return (void *)p;
}

void msc_arena_reset(msc_arena *arena) {
    arena->at = arena->base;
}

void msc_arena_init(msc_arena *arena, usize capacity) {
    arena->base = (u8 *)malloc(capacity);
    arena->at = arena->base;
    arena->end = arena->base + capacity;
    arena->next = NULL;
}

#endif // MSC_ARENA_IMPLEMENTATION
