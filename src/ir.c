#include "common.h"


#define PROC_INIT_CAP (kilobytes(1))
#define PROC_MAX_CAP  (32768) /* 2**15 */

bool procedure_init(Procedure* proc, patientid name) {
	if(proc == null) { return false; }

	if(proc->instructions != null) { heap_dealloc(proc->instructions); }
	Instr* ptr = heap_alloc(sizeof(Instr) * PROC_INIT_CAP);
	if(ptr == null) { return false; }

	if(proc->constants != null) { heap_dealloc(proc->constants); }
	Const* cptr = heap_alloc(sizeof(Const) * PROC_INIT_CAP);
	if(cptr == null) { return false; }

	proc->flags = 0;
	proc->name = name;

	proc->ilen = 1;
	proc->icap = PROC_INIT_CAP;
	proc->instructions = ptr;

	proc->clen = 1;
	proc->ccap = PROC_INIT_CAP;
	proc->constants = cptr;
	return true;
}

instrid ir_emit(Procedure* proc, Instr instruction) {
	if(proc == null) { return 0; }
	i16 len = proc->ilen,
	    cap = proc->icap;
	Instr* ptr = proc->instructions;

	if(len + 1 > cap) {
		cap *= 2;
		if(cap >= PROC_MAX_CAP) { return 0; }
		ptr = heap_realloc(ptr, cap);
		if(ptr == null)    { return 0; }
		proc->instructions = ptr;
		proc->icap = cap;
	}

	ptr[len-1] = instruction;
	proc->ilen += 1;
	return len;
}

instrid ir_const(Procedure* proc, Const c) {
	if(proc == null) { return 0; }
	i16 len = proc->clen,
	    cap = proc->ccap;
	Const* ptr = proc->constants;

	if(len + 1 > cap) {
		cap *= 2;
		if(cap >= PROC_MAX_CAP) { return 0; }
		ptr = heap_realloc(ptr, cap);
		if(ptr == null)    { return 0; }
		proc->constants = ptr;
		proc->ccap = cap;
	}

	ptr[len-1] = c;
	proc->clen += 1;
	return -len; // ref to const is negative
}
