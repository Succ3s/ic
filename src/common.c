#include "common.h"

// ===========================
// = String                  =
// ===========================

str str_from_cstr(cstr s) {
	return (str) {
		.ptr = s,
		.len = strlen(s),
	};
}

cstr str_to_cstr(str t) {
	if(t.ptr[t.len] == '\0') { return t.ptr; }
	cstr p = heap_alloc(t.len + 1);
	if(p == null) return null;
	memcpy(p, t.ptr, t.len);
	p[t.len] = '\0';
	return p;
}

cstr cstr_clone(cstr s) {
	usize len = cstr_len(s);
	cstr p = heap_alloc(len + 1);
	if(p == null) return null;
	memcpy(p, s, len + 1);
	return p;
}










// ===========================
// = Helpers                 =
// ===========================

u8* alignup(u8* addr, usize align) {
    return (u8*)(((usize)addr + align - 1) & ~(align - 1));
}










// ===========================
// = Buffer                  =
// ===========================

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










// ===========================
// = ChunkBuffer             =
// ===========================

#ifndef CHUNK_BUFFER_SIZE
#define CHUNK_BUFFER_SIZE (kilobytes(1))
#endif

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

bool chunk_buffer_deinit(ChunkBuffer* chnk) {
	if(chnk == null) { return false; }
	for(ChunkBufferNode* n = chnk->first; n != null; n = n->next) {
		heap_dealloc(n);
	}
	// *chnk = (ChunkBuffer) {0};
	return true;
}

cstr chunk_buffer_cstr_clone(ChunkBuffer* buf, cstr s) {
	usize len = cstr_len(s);
	cstr p = chunk_buffer_alloc(buf, len + 1);
	if(p == null) { return null; }
	memcpy(p, s, len + 1);
	return p;
}

cstr chunk_buffer_str_clone_to_cstr(ChunkBuffer* buf, str s) {
	cstr p = chunk_buffer_alloc(buf, s.len + 1);
	if(p == null) { return null; }
	memcpy(p, s.ptr, s.len);
	p[s.len + 1] = '\0';
	return p;
}
