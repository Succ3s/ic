#ifndef G_HEADER_COMMON
#define G_HEADER_COMMON


#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// TODO(pgs): format str for each type
// TODO(pgs): lexer:  interning identifiers
// TODO(pgs): parser: rewrite to generate ir


// ===========================
// = primitives              =
// ===========================

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










// ===========================
// = Helpers                 =
// ===========================


#define BG_BLACK    "\033[40m"
#define BG_RED      "\033[41m"
#define BG_GREEN    "\033[42m"
#define BG_YELLOW   "\033[43m"
#define BG_BLUE     "\033[44m"
#define BG_MAGENTA  "\033[45m"
#define BG_CYAN     "\033[46m"
#define BG_WHITE    "\033[47m"

#define FG_BLACK    "\033[30m"
#define FG_RED      "\033[31m"
#define FG_GREEN    "\033[32m"
#define FG_YELLOW   "\033[33m"
#define FG_BLUE     "\033[34m"
#define FG_MAGENTA  "\033[35m"
#define FG_CYAN     "\033[36m"
#define FG_WHITE    "\033[37m"

#define COLOR_RESET "\033[0m"


#define bytes(n)       (n)
#define kilobytes(n)   ((n)*1024)
#define megabytes(n)   (kilobytes(n)*1024)
#define gigabytes(n)   (megabytes(n)*1024)
#define terabytes(n)   (gigabytes(n)*1024)




#define heap_alloc(s)      (malloc((s)))
#define heap_dealloc(p)    (free((p)))
#define heap_realloc(p, s) (realloc((p), (s)))

u8* alignup(u8* addr, usize align);










// ===========================
// = String                  =
// ===========================

typedef struct { char* ptr; usize len; } str;
typedef char*                           cstr;

#define cstr_len(s) (strlen(s))
str str_from_cstr(cstr s);
cstr str_to_cstr(str t);
cstr cstr_clone(cstr s);










// ===========================
// = Buffer                  =
// ===========================

typedef struct Buffer Buffer;
struct Buffer {
	u8* begin; u8* end; u8* cursor;
};

bool buffer_init(Buffer* buf, u8* mem, usize size);
void* buffer_alloc(Buffer* buf, usize size);
void buffer_clean(Buffer* buf);










// ===========================
// = ChunkBuffer             =
// ===========================

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

bool chunk_buffer_init(ChunkBuffer* chnk);
void* chunk_buffer_alloc(ChunkBuffer* chnk, usize size);
bool chunk_buffer_deinit(ChunkBuffer* chnk);
cstr chunk_buffer_cstr_clone(ChunkBuffer* buf, cstr s);
cstr chunk_buffer_str_clone_to_cstr(ChunkBuffer* buf, str s);










// ===========================
// = Hospital                =
// ===========================

typedef u32 patientid;

typedef struct Hospital Hospital;
struct Hospital {
	cstr* patients;
	patientid len, cap;
	ChunkBuffer buf;
};


bool      hospital_init(Hospital* h);
patientid hospital_find_patient(Hospital h, cstr str);
patientid hospital_intern(Hospital* h, cstr str);









// ===========================
// = IR                      =
// ===========================

/*

compiler = { [collection], [procid:Type], [typeid:Procedure] }

collectionDesc = { collection, string, nameid }

colletion = { [FILE*], [FileCtx], [procid], [typeid], [Decl], Sym }
FileCtx   = [Import]
Sym       = { Map(nameid, Ids) }
Ids       = tagged(procid, typeid, globalid, instrid, argid)

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
	IrCallArg,

	IrPushAttr,
	IrPopAttr,

	IrJmp,
	IrCondJmp,


	IR_OPCODE_COUNT
};


typedef i64 typeid;
typedef i16 instrid;
typedef i32 procid;
typedef i32 globalid;
typedef i32 argid;

typedef union IrData IrData;
union IrData {
	u64      raw;
	instrid  instr;
	argid    arg;
	globalid global;
	struct { instrid r, l;              } bin;
	struct { procid  proc; instrid arg; } call;
	struct { instrid nxt, arg;          } call_arg;
	struct { instrid trgt, val;         } condjmp;
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
		typeid                                                 slice;
		struct { isize len; typeid T;                        } array;
		struct { patientid name; typeid T;                   } named;
		struct { isize len; typeid* T;                       } tuple;
		struct { isize len; typeid* T; patientid fields;     } strct;
		struct { isize len; typeid* T;                       } unon;
		typeid                                                 ptr;
	};
};


typedef union Value Value;
union Value {
	i8      _i8;
	i16     _i16;
	i32     _i32;
	i64     _i64;

	u8      _u8;
	u16     _u16;
	u32     _u32;
	u64     _u64;

	f32     _f32;
	f64     _f64;

	bool    _bool;

	Value*  tuple; // len not require because all fields must be initialized
	Value*  strct; // TODO(pgs): len
	str  str;
	cstr cstr;
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
	u64 loc;
};

typedef struct Procedure Procedure;
struct Procedure {
	u64 flags;
	patientid name;
	Instr* instructions;
	i16 ilen, icap;
	Const* constants;
	i16 clen, ccap;
};

// typedef struct Import Import;
// struct Import {
// 	patientid name;
// 	cstr path;
// };

bool procedure_init(Procedure* proc, patientid name);
instrid ir_emit(Procedure* proc, Instr instruction);
instrid ir_const(Procedure* proc, Const c);

#if __STDC_VERSION__ >= 201112L /* c11 */
	_Static_assert(IR_OPCODE_COUNT <= 256, "there are more opcodes than what is allowed");
	_Static_assert(sizeof(IrData) == 8, "IrData occupies more memory than it should");
	_Static_assert(sizeof(Instr) == 24, "Instr occupies more(or less) memory than it should");
#endif





#endif
