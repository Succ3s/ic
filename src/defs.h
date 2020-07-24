#ifndef DASH_DEFS_INCLUDE
#define DASH_DEFS_INCLUDE


#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

// TODO(pgs): format string for each type


// ====== primitives

#define null ((void*)0)

typedef _Bool bool;
#define true  ((bool)1)
#define false ((bool)0)

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






// ===== HELPERS


#define bytes(n)       (n)
#define kilobytes(n)   ((n)*1024)
#define megabytes(n)   (kilobytes(n)*1024)
#define gigabytes(n)   (megabytes(n)*1024)
#define terabytes(n)   (gigabytes(n)*1024)




#define heap_alloc(s)      (malloc((s)))
#define heap_dealloc(p)    (free((p)))
#define heap_realloc(p, s) (realloc((p), (s)))



// ====== String
typedef struct { char* ptr; usize len; } string;
typedef char*                           cstring;

#define cstr_len(s) (strlen(s))

string string_from_cstring(cstring s) {
	return (string) {
		.ptr = s,
		.len = strlen(s),
	};
}

cstring str_to_cstr(string t) {
	if(t.ptr[t.len] == '\0') { return t.ptr; }
	cstring p = heap_alloc(t.len + 1);
	if(p == null) return null;
	memcpy(p, t.ptr, t.len);
	p[t.len] = '\0';
	return p;
}

cstring cstr_dup(cstring s) {
	int len = cstr_len(s);
	cstring p = heap_alloc(len + 1);
	if(p == null) return null;
	memcpy(p, s, len + 1);
	return p;
}








// ====== Allocators

// usize alignup(u8* addr, usize align) {
//     return ((usize)addr + align - 1) & ~(align - 1);
// }

// enum {
// 	AllocatorAlloc,
// 	AllocatorDealloc,
// 	AllocatorRealloc,
// 	AllocatorCleanup,
// };
// typedef void* (*AllocatorProc)(void* allocatorData, u8 kind, void* ptr, usize size);

// typedef struct {
// 	void* data;
// 	AllocatorProc proc;
// } Allocator;

// void* allocator_alloc(Allocator a, usize size) {
// 	return a->proc(
// 		a->data,
// 		AllocatorAlloc,
// 		null,
// 		size
// 	);
// }
// void allocator_dealloc(Allocator a, void* ptr) {
// 	a->proc(
// 		a->data,
// 		AllocatorDealloc,
// 		ptr,
// 		0
// 	);
// }
// void* allocator_realloc(Allocator a, void* ptr, usize size) {
// 	return a->proc(
// 		a->data,
// 		AllocatorRealloc,
// 		ptr,
// 		size
// 	);
// }

// void allocator_cleanup(Allocator a) {
// 	a->proc(
// 		a->data,
// 		AllocatorCleanup,
// 		null,
// 		0
// 	);
// }

// cstring string_to_cstring(Allocator al, string t) {
// 	if(t.ptr[t.len] == '\0') { return t.ptr; }
// 	cstring p = allocator_alloc(al, t.len + 1);
// 	if(p == null) return null;
// 	strncpy(p, t.ptr, t.len);
// 	p[t.len] = '\0';
// 	return p;
// }

// #include <stdlib.h>
// void* _std_allocator_proc(void* _, u8 kind, void* ptr, usize size) {
// 	switch(kind) {
// 		case AllocatorDealloc:
// 			free(ptr);
// 			return null;
// 		default:
// 			return realloc(ptr, size);
// 	}
// }

// Allocator std_allocator() {
// 	return (Allocator) {
// 		.data = null,
// 		.proc = &_std_allocator_proc,
// 	};
// }

// #define Array(T) T*
// typedef struct { Allocator allocator; isize len, cap, size; } ArrayHeader;
// #define get_array_header(ptr) ((ArrayHeader*)(ptr) - 1)
// #define array_len(ptr) (get_array_header((ptr))->len)
// #define array_cap(ptr) (get_array_header((ptr))->cap)
// #define array_init(arr, al, size, init_cap) (impl_array_init((Array(void)*)arr, (al), (size), (init_cap)))
// #define array_push(arr, ptr)                (impl_array_push((Array(void)*)arr, (ptr)))

// bool impl_array_init(Array(void)* arr, Allocator allocator, isize size, isize init_cap) {
// 	if(arr != null && *arr != null) { allocator_dealloc(allocator, *arr); }
// 	ArrayHeader* h = allocator_alloc(allocator, (sizeof(ArrayHeader) + (init_cap * size)));
// 	if(h == null) { return false; }
// 	h->allocator = allocator;
// 	h->len  = 0;
// 	h->cap  = init_cap;
// 	h->size = size;
// 	*arr = h + 1;
// 	return true;
// }

// bool impl_array_push(Array(void)* arrr, void* ptr) {
// 	Array(u8)* arr = (Array(u8)*)arrr;
// 	if(arr == null)  { return false; }
// 	if(*arr == null) { return false; }
// 	ArrayHeader* head = get_array_header(*arr);
// 	if(head->len + 1 > head->cap) {
// 		head->cap *= 2;
// 		ArrayHeader* tmp = allocator_realloc(head->allocator, head,
// 			sizeof(ArrayHeader) + (head->cap * head->size)
// 		);
// 		if(tmp == null) { return false; }
// 		tmp += 1;
// 		*arr = (Array(void))tmp;
// 		// TODO(pgs): grow
// 	}
// 	memcpy(*arr + (head->len * head->size), ptr, head->size);
// 	head->len+=1;
// 	return true;
// }


#endif
