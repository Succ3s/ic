#ifndef DASH_DEFS_INCLUDE
#define DASH_DEFS_INCLUDE

// TODO(pgs): format string for each type


// ====== primitives

#define null ((void*)0)

typedef _Bool bool;
#define true  ((bool)1)
#define false ((bool)0)

#include <stdint.h>
#include <stddef.h>
typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;
typedef ptrdiff_t isize;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t   usize;

typedef float  f32;
typedef double f64;


// ====== String
typedef char*                            cstring;
typedef struct { char* ptr; usize len; } string;

#include <string.h>
string string_from_cstring(cstring s) {
	return (string) {
		.ptr = s,
		.len = strlen(s),
	};
}






// ===== HELPERS


#define bytes(n)       (n)
#define kilobytes(n)   ((n)*1024)
#define megabytes(n)   (kilobytes(n)*1024)
#define gigabytes(n)   (megabytes(n)*1024)
#define terabytes(n)   (gigabytes(n)*1024)








// ====== Allocators

usize alignup(u8* addr, usize align) {
    return ((usize)addr + align - 1) & ~(align - 1);
}

enum {
	AllocatorAlloc,
	AllocatorDealloc,
	AllocatorRealloc,
	AllocatorCleanup,
};
typedef void* (*AllocatorProc)(void* allocatorData, u8 kind, void* ptr, usize size);

typedef struct {
	void* data;
	AllocatorProc proc;
} Allocator;

void* allocator_alloc(Allocator* a, usize size) {
	return a->proc(
		a->data,
		AllocatorAlloc,
		null,
		size
	);
}
void allocator_dealloc(Allocator* a, void* ptr) {
	a->proc(
		a->data,
		AllocatorDealloc,
		ptr,
		0
	);
}
void* allocator_realloc(Allocator* a, void* ptr, usize size) {
	return a->proc(
		a->data,
		AllocatorRealloc,
		ptr,
		size
	);
}

void allocator_cleanup(Allocator* a) {
	a->proc(
		a->data,
		AllocatorCleanup,
		null,
		0
	);
}

cstring string_to_cstring(Allocator* al, string t) {
	cstring p = allocator_alloc(al, t.len + 1);
	if(p == null) return null;
	strncpy(p, t.ptr, t.len);
	p[t.len] = '\0';
	return p;
}

#include <stdlib.h>
void* _std_allocator_proc(void* _, u8 kind, void* ptr, usize size) {
	switch(kind) {
		case AllocatorDealloc:
			free(ptr);
			return null;
		default:
			return realloc(ptr, size);
	}
}

Allocator std_allocator() {
	return (Allocator) {
		.data = null,
		.proc = &_std_allocator_proc,
	};
}


struct _BumpArena {
	struct _BumpArena* last;
	u8* begin;
	u8* cursor;
	u8* end;
};

struct GrowingBumpAllocator {
	Allocator* host_allocator;
	usize arena_size;
	struct _BumpArena* tail;
};

struct _BumpArena* _GrowingBumpAllocator_new_arena(struct GrowingBumpAllocator* b) {
	usize alloc_size = sizeof(struct _BumpArena) + b->arena_size;
	struct _BumpArena* arena = allocator_alloc(b->host_allocator, alloc_size);
	if(arena == null) return null;
	arena->begin = (u8*)arena + sizeof(struct _BumpArena);
	arena->end = (u8*)arena + alloc_size;
	arena->cursor = arena->begin;
	arena->last = b->tail;
	b->tail = arena;
	return arena;
}

void* _growing_bump_allocator_proc(void* __data, u8 kind, void* ptr, usize size) {
	struct GrowingBumpAllocator* data = __data;
	switch(kind) {
		case AllocatorAlloc: {
			struct _BumpArena* arena = data->tail;
			u8* newPos = (u8*)alignup(arena->cursor + size, sizeof(usize));
			if(newPos > arena->end || arena == null) {
				arena = _GrowingBumpAllocator_new_arena(data);
				newPos = arena->cursor + size;
				if(arena == null) { return null; }
			}

			u8* ret = arena->cursor;
			arena->cursor = newPos;
			return ret;

			break;
		}
		case AllocatorDealloc: return null;
		case AllocatorRealloc:
			return _growing_bump_allocator_proc(
				data,
				AllocatorAlloc,
				null,
				size
			);
		case AllocatorCleanup:
			for(struct _BumpArena* a = data->tail;a != null;) {
				struct _BumpArena* next = a->last;
				allocator_dealloc(data->host_allocator, a);
				a = next;
			}
	}
}

Allocator growing_bump_allocator(struct GrowingBumpAllocator* target, Allocator* host_allocator, usize arena_size) {
	*target = (struct GrowingBumpAllocator) {
		.host_allocator = host_allocator,
		.arena_size = arena_size,
	};
	_GrowingBumpAllocator_new_arena(target);
	return (Allocator) {
		.data = target,
		.proc = _growing_bump_allocator_proc,
	};
}


// Chunked List

#define ChunkedList(T, S)              ChunkedList_ ## T ## _ ## S
#define ChunkedListNode(T, S)          ChunkedListNode_ ## T ## _ ## S
#define _ChunkedList_create_node(T, S) _ChunkedList_ ## T ## _ ## S ## _create_node
#define _ChunkedList_append(T, S)      _ChunkedList_ ## T ## _ ## S ## _append
#define ChunkedList_push(T, S)         ChunkedList_ ## T ## _ ## S ## _push
#define ChunkedListImpl(T, S)                                                 \
typedef struct ChunkedListNode(T, S) ChunkedListNode(T, S);                   \
typedef struct ChunkedList(T, S) ChunkedList(T, S);                           \
struct ChunkedList(T, S) {                                                    \
    ChunkedListNode(T, S)* head;                                              \
    ChunkedListNode(T, S)* tail;                                              \
};                                                                            \
struct ChunkedListNode(T, S) {                                                \
    ChunkedListNode(T, S)* next;                                              \
    usize count;                                                              \
    T data[S];                                                                \
};                                                                            \
bool _ChunkedList_create_node(T, S)(ChunkedList(T, S)* list, Allocator* al) { \
    ChunkedListNode(T, S)* _new = allocator_alloc(                            \
        al,                                                                   \
        sizeof(ChunkedListNode(T, S)));                                       \
    if(_new == null) { return false; }                                        \
    *_new = (ChunkedListNode(T, S)){0};                                       \
    if(list->tail != null) {                                                  \
        list->tail->next = _new;                                              \
    }                                                                         \
    list->tail = _new;                                                        \
    return true;                                                              \
}                                                                             \
bool _ChunkedList_append(T, S)(ChunkedList(T, S)* list, T val) {              \
    ChunkedListNode(T, S)* t = list->tail;                                    \
    if(t->count >= S) { return false; }                                       \
    t->data[t->count] = val;                                                  \
    t->count += 1;                                                            \
    return true;                                                              \
}                                                                             \
bool ChunkedList_push(T, S)(ChunkedList(T, S)* list, Allocator* al, T val) {  \
    if(list->tail == null) {                                                  \
        *list = (ChunkedList(T, S)) {0};                                      \
        _ChunkedList_create_node(T, S)(list, al);                             \
        list->head = list->tail;                                              \
    }                                                                         \
    if(_ChunkedList_append(T, S)(list, val)) { return true; }                 \
    if(!_ChunkedList_create_node(T, S)(list, al)) { return false; }           \
    return _ChunkedList_append(T, S)(list, val);                              \
}

#endif
