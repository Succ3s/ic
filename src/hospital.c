#include "common.h"

#ifndef HOSPITAL_INIT_CAP
#define HOSPITAL_INIT_CAP (20)
#endif

bool hospital_init(Hospital* h) {
	if(h == null) { return false; }
	
	if(h->patients != null) { heap_dealloc(h->patients); }
	cstr* p = heap_alloc(HOSPITAL_INIT_CAP);	
	if(p == null) { return false; }
	h->patients = p;
	h->len = 0;
	h->cap = HOSPITAL_INIT_CAP;

	ChunkBuffer buf = {0};
	chunk_buffer_init(&buf);
	h->buf = buf;
	
	return true;
}

patientid hospital_find_patient(Hospital h, cstr str) {
	for(patientid i = 0; i < h.len; i++) {
		if(strcmp(h.patients[i], str) == 0) { return i; }
	}
	return -1;
}

patientid hospital_intern(Hospital* h, cstr str) {
	if(h == null) { return -1; } 
	patientid len = h->len,
	          cap = h->cap;
	cstr* patients = h->patients;


	if(len + 1 > cap) {
		cap *= 2;
		patients = heap_realloc(patients, cap);
		if(patients == null)    { return -1; }
		h->patients = patients;
		h->cap = cap;
	}

	patientid find = hospital_find_patient(*h, str);
	if(find == -1) {
		cstr dup = chunk_buffer_cstr_clone(&h->buf, str);
		patients[len] = dup;
		h->len += 1;
		return len;
	}
	return find;
}
