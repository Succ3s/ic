#include "common.h"


// ===========================
// = Buffer                  =
// ===========================

void* buf__push(BufHdr* hdr, void* arr, isize el_size, isize push_count) {
	if(!arr) {
		isize ncap = max(BUF_CAP, push_count);
		BufHdr* hdr = heap_allocz(sizeof(BufHdr) + ncap*el_size);
		assert(hdr != null);
		hdr->cap = ncap;

		return hdr->data;
	}
	isize len = hdr->len, cap = hdr->cap;
	if(len + push_count > cap) {
		isize ncap = max(cap * 2, cap + push_count);
		hdr = heap_realloc(hdr, sizeof(BufHdr) + ncap*el_size);
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

	// m[idx] = m[--hdr->buf.len];
	memmove(m + idx*el_size, m + (--hdr->buf.len)*el_size, el_size);

	buf_remove(hdr->keys, idx);
	return true;
}

void* map__push(MapHdr* hdr, void* map, u64 key, isize el_size) {
	if(!map) {
		MapHdr* h = heap_allocz(sizeof(MapHdr) + BUF_CAP*el_size);
		assert(h != null);
		h->buf.cap = BUF_CAP;
		buf_push(h->keys, key);
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
	isize len = cstr_len(s);
	u8* p = heap_alloc(len + 1);
	if(p == null) return null;
	memcpy(p, s, len);
	p[len+1] = '\0';
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
		buf_push(hh, (char*)st);
		*h = hh;
	}

	return st;
}










// ===========================
// = Sources                 =
// ===========================

isize sources_add(Sources* srcs, cstr stream, cstr path) {
	// base can't be 0
	srcs->base += (srcs->base == 0);

	cstr dup = cstr_clone(stream);
	Source src = (Source) {
		.path = path,
		.stream = dup,
		.size = cstr_len(stream),
		.base = srcs->base
	};

	srcs->base += src.size;

	for(;*dup; dup++) {
		if(*dup == '\n') { break; }
		src.eol++;
	}

	buf_push(srcs->list, src);
	return buf_len(srcs->list)-1;
}


// NOTE(pgs): I think this is slow, *but* most packages have less than 10 files, so I think this is fine
isize sources_find(Sources* srcs, Pos pos) {
	isize blen = buf_len(srcs->list);
	for(isize i = 0; i < blen; i++) {
		i32 base = srcs->list[i].base;
		i32 end = base + srcs->list[i].size;

		if(pos >= base && pos < end) {
			return i;
		}
	}
	return -1;
}


// NOTE(pgs): I think this is slow, *but* it's only used when printing errors, should try to optimize
Position sources_position(Sources* srcs, Pos pos) {
	Position p = { .line = 1, .column = -1, 0};
	isize idx = sources_find(srcs, pos);
	if(idx == -1) { return p; }

	Source src = srcs->list[idx];
	p.path = src.path;

	i32 inp = pos - src.base;

	if(inp <= src.eol) {
		p.column = inp;
		return p;
	}

	for(cstr i = src.stream + inp; *i-- != '\n';) {
		p.column++;
	}

	p.line++;
	for(cstr i = src.stream + inp; i > src.stream + src.eol; i--) {
		if(*i == '\n') { p.line++; }
	}

	
	return p;
}

#if 0
#define TMAIN srcs
int srcs(int argc, cstr* argv) {
	Sources src = {0};
	isize idx0 = sources_add(&src, "\n\n foo bar\n", "build.ttn");

	Pos p = 4;

	isize idx = sources_find(&src, p);
	assert(idx0 == idx);

	char ch = src.list[idx].stream[p-1];
	assert(ch == 'f');
	printf("'%c'\n", ch);

	Position pos = sources_position(&src, p);
	printf("{ line = %d, column = %d, path = %s }\n", pos.line, pos.column, pos.path);
}
#endif
