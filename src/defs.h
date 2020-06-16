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
string newString(cstring s) {
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

cstring stringToCString(Allocator* al, string t) {
	cstring p = allocator_alloc(al, t.len + 1);
	if(p == null) return null;
	strncpy(p, t.ptr, t.len);
	p[t.len] = '\0';
	return p;
}

#include <stdlib.h>
void* stdAllocatorProc(void* _, u8 kind, void* ptr, usize size) {
	switch(kind) {
		case AllocatorDealloc:
			free(ptr);
			return null;
		default:
			return realloc(ptr, size);
	}
}

Allocator stdAllocator() {
	return (Allocator) {
		.data = null,
		.proc = &stdAllocatorProc,
	};
}


struct BumpArena {
	struct BumpArena* last;
	u8* begin;
	u8* cursor;
	u8* end;
};

struct GrowingBumpAllocator {
	Allocator* hostAllocator;
	usize arenaSize;
	struct BumpArena* tail;
};

struct BumpArena* GrowingBumpAllocator_newArena(struct GrowingBumpAllocator* b) {
	usize allocSize = sizeof(struct BumpArena) + b->arenaSize;
	struct BumpArena* arena = allocator_alloc(b->hostAllocator, allocSize);
	if(arena == null) return null;
	arena->begin = (u8*)arena + sizeof(struct BumpArena);
	arena->end = (u8*)arena + allocSize;
	arena->cursor = arena->begin;
	arena->last = b->tail;
	b->tail = arena;
	return arena;
}

void* growingBumpAllocatorProc(void* __data, u8 kind, void* ptr, usize size) {
	struct GrowingBumpAllocator* data = __data;
	switch(kind) {
		case AllocatorAlloc: {
			struct BumpArena* arena = data->tail;
			u8* newPos = (u8*)alignup(arena->cursor + size, sizeof(usize));
			if(newPos > arena->end || arena == null) {
				arena = GrowingBumpAllocator_newArena(data);
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
			return growingBumpAllocatorProc(
				data,
				AllocatorAlloc,
				null,
				size
			);
		case AllocatorCleanup:
			for(struct BumpArena* a = data->tail;a != null;) {
				struct BumpArena* next = a->last;
				allocator_dealloc(data->hostAllocator, a);
				a = next;
			}
	}
}

Allocator growingBumpAllocator(struct GrowingBumpAllocator* target, Allocator* hostAllocator, usize arenaSize) {
	*target = (struct GrowingBumpAllocator) {
		.hostAllocator = hostAllocator,
		.arenaSize = arenaSize,
	};
	GrowingBumpAllocator_newArena(target);
	return (Allocator) {
		.data = target,
		.proc = growingBumpAllocatorProc,
	};
}


// Chunked List

#define ChunkedList(T, S)            ChunkedList_ ## T ## _ ## S
#define ChunkedListNode(T, S)        ChunkedListNode_ ## T ## _ ## S
#define ChunkedList_createNode(T, S) ChunkedList_ ## T ## _ ## S ## _createNode
#define ChunkedList_append(T, S)     ChunkedList_ ## T ## _ ## S ## _append
#define ChunkedList_push(T, S)       ChunkedList_ ## T ## _ ## S ## _push
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
bool ChunkedList_createNode(T, S)(ChunkedList(T, S)* list, Allocator* al) {   \
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
bool ChunkedList_append(T, S)(ChunkedList(T, S)* list, T val) {               \
	ChunkedListNode(T, S)* t = list->tail;                                    \
	if(t->count >= S) { return false; }                                       \
	t->data[t->count] = val;                                                  \
	t->count += 1;                                                            \
	return true;                                                              \
}                                                                             \
bool ChunkedList_push(T, S)(ChunkedList(T, S)* list, Allocator* al, T val) {  \
	if(list->tail == null) {                                                  \
		*list = (ChunkedList(T, S)) {0};                                      \
		ChunkedList_createNode(T, S)(list, al);                               \
		list->head = list->tail;                                              \
	}                                                                         \
	if(ChunkedList_append(T, S)(list, val)) { return true; }                  \
	if(!ChunkedList_createNode(T, S)(list, al)) { return false; }             \
	return ChunkedList_append(T, S)(list, val);                               \
}

#endif
