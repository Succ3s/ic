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
	u8* p = heap_alloc(t.len + 1);
	if(p == null) return null;
	memcpy(p, t.ptr, t.len);
	p[t.len] = '\0';
	return (cstr)p;
}

cstr cstr_clone(cstr s) {
	usize len = cstr_len(s);
	u8* p = heap_alloc(len + 1);
	if(p == null) return null;
	memcpy(p, s, len + 1);
	return (cstr)p;
}


cstr cstr_intern(StrIntern* h, cstr s) {
	if(h == null) { return null; } 


	// try to find interned string
	cstr st = null;
	StrIntern hh = *h;
	for(isize i = 0; i < buf_len(hh); i++) {
		if(strcmp(hh[i], s) == 0) { st = hh[i]; }
	}

	// if not found, intern string
	if(st == null) {
		st = cstr_clone(s);
		buf_push(*h, st);
	}

	return st;
}










// ===========================
// = Helpers                 =
// ===========================

u8* alignup(u8* addr, usize align) {
    return (u8*)(((usize)addr + align - 1) & ~(align - 1));
}


void cwb_assert(bool expr, isize line, cstr file) {
	if(expr) { return; }
	printf(BG_RED FG_BLACK "ASSERT ERROR" COLOR_RESET " at %s:%d\n", file, line);
	abort();
}










// ===========================
// = Buffer                  =
// ===========================

void* buf__push(BufHdr* hdr, void* arr, isize el_size, isize push_count) {
	if(!arr) {
		BufHdr* hdr = heap_allocz(sizeof(BufHdr) + BUF_CAP);
		assert(hdr != null);
		hdr->cap = BUF_CAP;
		return hdr->data;
	}
	isize len = hdr->len, cap = hdr->cap;
	if(len + push_count > cap) {
		isize ncap = max(cap * 2, cap + push_count);
		hdr = heap_realloc(hdr, ncap*el_size);
		hdr->cap = ncap;
	}
	return hdr->data;
}










// ===========================
// = Map                     =
// ===========================

isize map__find_index(MapHdr* hdr, u64 key) {
	isize bufl = buf_len(hdr->keys);
	for(isize i = 0; i < bufl; i++) {
		if(key == hdr->keys[i]) {
			return i;
		}
	}
	return -1;
}

bool map__remove(MapHdr* hdr, void* map, u64 key, isize el_size) {
	if(!map) { return false; }
	isize idx = map__find_index(hdr, key);
	if(idx == -1) { return false; }
	u8* m = map;
	memmove(m + idx*el_size, m + idx*el_size + 1, map_len(m) - idx*el_size - 1);
	buf_remove(hdr->keys, idx);
	// buf_h(hdr->keys)->len--;
	hdr->buf.len--;
	return true;
}

void* map__push(MapHdr* hdr, void* map, u64 key, isize el_size) {
	if(!map) {
		MapHdr* h = heap_allocz(sizeof(MapHdr) + sizeof(BUF_CAP));
		assert(h != null);
		h->buf.cap = BUF_CAP;
		buf_push(h->keys, key);
		BufHdr* b = buf_h(h->keys);
		assert(b != null);
		return h->buf.data;
	}

	isize bufl = buf_len(hdr->keys);
	assert(bufl == hdr->buf.len);

	for(isize i = 0; i < bufl; i++) {
		if(key == hdr->keys[i]) {
			goto end;
		}
	}


	buf_push(hdr->keys, key);
	buf__push(&hdr->buf, map, el_size, 1);

	end:
	return hdr->buf.data;
}
