#ifndef DASH_HEADER_DEFS
#define DASH_HEADER_DEFS


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

#define cstring_len(s) (strlen(s))

string string_from_cstring(cstring s) {
	return (string) {
		.ptr = s,
		.len = strlen(s),
	};
}

cstring string_to_cstring(string t) {
	if(t.ptr[t.len] == '\0') { return t.ptr; }
	cstring p = heap_alloc(t.len + 1);
	if(p == null) return null;
	memcpy(p, t.ptr, t.len);
	p[t.len] = '\0';
	return p;
}

cstring cstring_clone(cstring s) {
	usize len = cstring_len(s);
	cstring p = heap_alloc(len + 1);
	if(p == null) return null;
	memcpy(p, s, len + 1);
	return p;
}


u8* alignup(u8* addr, usize align) {
    return (u8*)(((usize)addr + align - 1) & ~(align - 1));
}

typedef struct Buffer Buffer;
struct Buffer {
	u8* begin; u8* end; u8* cursor;
};

bool buffer_init(Buffer* buf, u8* mem, usize size) {
	if(mem == null) { return false; }
	Buffer b = {
		.begin = mem,
		.cursor = mem,
		.end = mem + size
	};
	*buf = b;
	return true;
}

void* buffer_alloc(Buffer* buf, usize size) {
	if(buf == null || buf->begin == null) { return null; }
	u8*	cursor = buf->cursor;
	u8*	end    = buf->end;

	u8* tmp = alignup(cursor + size, sizeof(void*));
	if(tmp > buf->end) { return null; }

	buf->cursor = tmp;
	return cursor;
}

void buffer_clean(Buffer* buf) {
	buf->begin = buf->cursor;
}



#define CHUNK_BUFFER_SIZE (kilobytes(1))

typedef struct ChunkBufferNode ChunkBufferNode;
struct ChunkBufferNode {
	ChunkBufferNode* next;
	Buffer buf;
};

typedef struct ChunkBuffer ChunkBuffer;
struct ChunkBuffer {
	ChunkBufferNode* first;
	ChunkBufferNode* last;
	Buffer* curr;
};

bool chunk_buffer_init(ChunkBuffer* chnk) {
	if(chnk == null) { return false; }
	for(ChunkBufferNode* n = chnk->first; n != null; n = n->next) {
		heap_dealloc(n);
	}
	ChunkBufferNode* n = heap_alloc(sizeof(ChunkBufferNode) + CHUNK_BUFFER_SIZE);
	if(n == null) { return false; }
	Buffer buf = {0};
	buffer_init(&buf, (u8*)(n + 1), CHUNK_BUFFER_SIZE);
	n->buf = buf;

	chnk->first = n;
	chnk->last = n;
	chnk->curr = &n->buf;
	return true;
}

void* chunk_buffer_alloc(ChunkBuffer* chnk, usize size) {
	void* try = buffer_alloc(chnk->curr, size);
	if(try != null) { return try;  }

	if(size > CHUNK_BUFFER_SIZE) { return null; }

	ChunkBufferNode* n = heap_alloc(sizeof(ChunkBufferNode) + CHUNK_BUFFER_SIZE);
	if(n == null) { return null; }
	Buffer buf;
	buffer_init(&buf, (u8*)(n + 1), CHUNK_BUFFER_SIZE);
	n->buf = buf;
	n->next = null;


	chnk->last->next = n;
	chnk->last = n;
	chnk->curr = &n->buf;

	return chunk_buffer_alloc(chnk, size);
}

bool chunk_buffer_clean(ChunkBuffer* chnk) {
	if(chnk == null) { return false; }
	for(ChunkBufferNode* n = chnk->first; n != null; n = n->next) {
		heap_dealloc(n);
	}
	return true;
}

#endif
