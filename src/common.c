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


cstr cstr_intern(StringIntern* h, cstr s) {
	if(h == null) { return null; } 


	// try to find interned string
	cstr st = null;
	StringIntern hh = *h;
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
