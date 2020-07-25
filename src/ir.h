#ifndef DASH_HEADER_IR
#define DASH_HEADER_IR

// #include "common.h"

#define DASH_HOSPITAL_IMPL
#include "hospital.h"

/*

// TODO:
  - hospital
  - finish IR
  - remove AST
  - rewrite parser to generate IR

*/

enum {
	IrNop,

	// arith
	IrAdd,
	IrSub,
	IrMul,
	IrDiv,
	IrMod,

	// cmp
	IrGt,
	IrLt,
	IrGe,
	IrLe,
	IrEq,
	IrNe,

	IrAnd,
	IrOr,
	IrNot,

	IrCast,

	IrRef,
	IrLoad,
	IrStore,

	IrLdArg,
	IrLdGlobal,

	IrInit,
	IrZeroInit,



	IrIndex,


	IrCall,
	IrCallPkg,
	IrCarg,


	IrSetLoc,

	IrPushAttr,
	IrPopAttr,

	IrJmp,
	IrCondJmp,


	IR_OPCODE_COUNT
};


typedef i64 typeid;
typedef i16 instrid;
typedef u32 globalid;
typedef u32 argid;

typedef union IrData IrData;
union IrData {
	u64      raw;
	instrid  instr;
	argid    arg;
	globalid global;
	struct { instrid   r, l;              } bin;
	struct { patientid proc; instrid arg; } call;
	struct { instrid   nxt, arg;          } carg;
	struct { instrid   trgt, val;         } condjmp;
};


// TODO(pgs)
enum {
	TypeInfo_Pointer,
	TypeInfo_Struct,
	TypeInfo_Array,
	TypeInfo_Tuple,
	TypeInfo_Slice,
	TypeInfo_Named,
	TypeInfo_Union,

	TypeInfo_Int8,
	TypeInfo_Int16,
	TypeInfo_Int32,
	TypeInfo_Int64,

	TypeInfo_UInt8,
	TypeInfo_UInt16,
	TypeInfo_UInt32,
	TypeInfo_UInt64,

	TypeInfo_Float32,
	TypeInfo_Float64,

	TypeInfo_Bool,

	TypeInfo_CString,
	TypeInfo_String,
};


typedef struct TypeInfo TypeInfo;
struct TypeInfo {
	u8 tag;
	union {
		typeid                                              slice;
		struct { isize len; typeid T;                     } array;
		struct { patientid name; typeid T;                   } named;
		struct { isize len; typeid* T;                    } tuple;
		struct { isize len; patientid fields; typeid* types; } strct;
		struct { isize len; typeid* T;                    } unon;
		typeid                                              ptr;
	};
};


typedef union Value Value;
union Value {
	i8      i8;
	i16     i16;
	i32     i32;
	i64     i64;

	u8      u8;
	u16     u16;
	u32     u32;
	u64     u64;

	f32     f32;
	f64     f64;

	bool    bool;

	Value*  tuple;
	Value*  strct;
	string  string;
	cstring cstring;
	u8*     ptr;

};

typedef struct Const Const;
struct Const {
	typeid type;
	Value v;
};


typedef struct Instr Instr;
struct Instr {
	unsigned    opcode: 8;
	signed long typeid: 56;
	IrData      raw;
};


#define PROC_INIT_CAP (1024)
#define PROC_MAX_CAP  (32768) /* 2**15 */

typedef struct Procedure Procedure;
struct Procedure {
	u64 flags;
	patientid name;
	Instr* instructions;
	i16 ilen, icap;
	Const* constants;
	i16 clen, ccap;
};


int init_proc(Procedure* proc, patientid name) {
	if(proc == null) { return 1; }
	if(proc->instructions != null) { heap_dealloc(proc->instructions); }
	Instr* ptr = heap_alloc(sizeof(Instr) * PROC_INIT_CAP);
	if(ptr == null) { return 1; }

	if(proc->constants != null) { heap_dealloc(proc->constants); }
	Const* cptr = heap_alloc(sizeof(Const) * PROC_INIT_CAP);
	if(cptr == null) { return 1; }

	proc->flags = 0;
	proc->name = name;

	proc->ilen = 1;
	proc->icap = PROC_INIT_CAP;
	proc->instructions = ptr;

	proc->clen = 1;
	proc->ccap = PROC_INIT_CAP;
	proc->constants = cptr;
	return 0;
}

instrid ir_emit(Procedure* proc, Instr instruction) {
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
	return -len; // constid are negative
}







#if __STDC_VERSION__ >= 201112L /* c11 */
	_Static_assert(IR_OPCODE_COUNT <= 256, "there are more opcodes than what is allowed");
	_Static_assert(sizeof(IrData) == 8, "IrData occupies more memory than it should");
	_Static_assert(sizeof(Instr) == 16, "Instr occupies more memory than it should");
#endif

#endif
