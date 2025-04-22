#ifndef MISCELLUS_ARENA
#define MISCELLUS_ARENA

#include <malloc.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#error TODO
#endif

#ifndef MISCELLUS_TYPES_H
#define MISCELLUS_TYPES_H
#include <stdint.h>
#include <stdalign.h>
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

#ifndef msc_offsetof
#define msc_offsetof(T, M) ((usize)(&((T*)0)->M))
#endif

#ifndef msc_alignof
#define msc_alignof(T) alignof(T)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct msc_arena_t {
    void *base;
    void *at;
    void *end;
} msc_arena;

msc_arena msc_arena_create(usize cap);
uptr msc_align_ptr(uptr ptr, usize align);
void *msc_arena_push_size(msc_arena *arena, usize size, usize align);
void msc_arena_reset(msc_arena *arena);

#define msc_arena_push(A, T) ((T *)msc_arena_push_size((A), sizeof(T), msc_alignof(T)))

#ifdef __cplusplus
}
#endif

#endif // MISCELLUS_ARENA

msc_arena msc_arena_create(usize cap)
{
    msc_arena arena;
    memset(&arena, 0, sizeof(arena));

#ifdef _WIN32
    arena.at = VirtualAlloc(NULL, cap, MEM_RESERVE, PAGE_READWRITE);
#else
#error TODO(jkkammersgaard): implement msc_arena_create on this platform
#endif

    arena.end = (void*)((uptr)arena.at + cap);
    return arena;
}

void *msc_arena_push_size(msc_arena *arena, usize size, usize align) {

    uptr alloc_base = msc_align_ptr((uptr)arena->at, align);
    uptr alloc_end = alloc_base + size;

    if (alloc_end >= (uptr)arena->end) {
        return NULL;
    }

    arena->at = (void *)alloc_end;

    return (void *)alloc_base;
}

uptr msc_align_ptr(uptr ptr, usize align) {
    if (msc_is_pow2(align)) {
        return (ptr + align - 1) & ~(align - 1);
    }
    return ((ptr + align - 1) / align) * align;
}

void msc_arena_reset(msc_arena *arena) {
    arena->at = arena->base;
}
