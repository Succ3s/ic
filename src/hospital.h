#ifndef DASH_HEADER_HOSPITAL
#define DASH_HEADER_HOSPITAL


#include "defs.h"

#ifndef HOSPITAL_INIT_CAP
#define HOSPITAL_INIT_CAP (20)
#endif


typedef u32 patientid;

typedef struct Hospital Hospital;
struct Hospital {
	cstring* patients;
	patientid len, cap;
	ChunkBuffer buf;
};


bool      hospital_init(Hospital* h);
patientid hospital_find_patient(Hospital h, cstring str);
patientid hospital_intern(Hospital* h, cstring str);



#ifdef DASH_HOSPITAL_IMPL
#undef DASH_HOSPITAL_IMPL

bool hospital_init(Hospital* h) {
	if(h == null) { return false; }
	
	if(h->patients != null) { heap_dealloc(h->patients); }
	cstring* p = heap_alloc(HOSPITAL_INIT_CAP);	
	if(p == null) { return false; }
	h->patients = p;
	h->len = 0;
	h->cap = HOSPITAL_INIT_CAP;

	ChunkBuffer buf = {0};
	chunk_buffer_init(&buf);
	h->buf = buf;
	
	return true;
}

patientid hospital_find_patient(Hospital h, cstring str) {
	for(patientid i = 0; i < h.len; i++) {
		if(strcmp(h.patients[i], str) == 0) { return i; }
	}
	return -1;
}


cstring __cstring_clone_chunk_buffer(ChunkBuffer* buf, cstring s) {
	usize len = cstring_len(s);
	cstring p = chunk_buffer_alloc(buf, len + 1);
	if(p == null) { return null; }
	memcpy(p, s, len + 1);
	return p;
}

patientid hospital_intern(Hospital* h, cstring str) {
	if(h == null) { return -1; } 
	patientid len = h->len,
	          cap = h->cap;
	cstring* patients = h->patients;


	if(len + 1 > cap) {
		cap *= 2;
		patients = heap_realloc(patients, cap);
		if(patients == null)    { return -1; }
		h->patients = patients;
		h->cap = cap;
	}

	patientid find = hospital_find_patient(*h, str);
	if(find == -1) {
		cstring dup = __cstring_clone_chunk_buffer(&h->buf, str);
		patients[len] = dup;
		h->len += 1;
		return len;
	}
	return find;
}

#endif


#endif
