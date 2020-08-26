#ifndef IC_HEADER_COMMON
#define IC_HEADER_COMMON


#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>



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


typedef const char* cstr;










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


#define byte     (0)
#define kilobyte (1024)
#define megabyte (kilobyte*1024)
#define gigabyte (megabyte*1024)
#define terabyte (gigabyte*1024)




#define heap_alloc(s)      (malloc((s)))
#define heap_allocz(s)     (calloc(1, (s)))
#define heap_dealloc(p)    (free((p)))
#define heap_realloc(p, s) (realloc((p), (s)))


#ifndef min
#	define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#	define max(x, y) ((x) > (y) ? (x) : (y))
#endif

#define sizeof(t) ((isize)sizeof(t))









// ===========================
// = Buffer                  =
// ===========================

#define Buf(T) T*

#define BUF_CAP (8)

typedef struct BufHdr BufHdr;
struct BufHdr {
	isize len, cap;
	u8 data[0];
};

#define buf_h(t)             ((BufHdr*)(((u8*)(t)) - offsetof(BufHdr, data)))
#define buf_push(t, v)       ((t) = buf__push(buf_h((t)), (t), sizeof(*(t)), 1), (t)[buf_h((t))->len++] = (v))
#define buf_len(t)           ((t) ? buf_h((t))->len                         : 0)
#define buf_remove(t, index) ((t) ? ((t)[(index)] = (t)[--buf_h((t))->len]) : 0)
#define buf_dealloc(t)       ((t) ? heap_dealloc((buf_h((t))))              : 0)

void* buf__push(BufHdr* hdr, void* arr, isize el_size, isize push_count);










// ===========================
// = Map                     =
// ===========================

#define Map(T) T*

typedef struct MapHdr MapHdr;
struct MapHdr {
	Buf(u64) keys;
	BufHdr buf;
};

#define map_h(t) ((MapHdr*)(((u8*)(t)) - offsetof(MapHdr, buf) - offsetof(BufHdr, data)))
#define map_push(t, key, v) (t = map__push(map_h((t)), (t), (key), sizeof(*(t))), (t)[map_h((t))->buf.len++] = (v))
#define map_find_index(t, key) ((t) != null ? map__find_index(map_h((t)), (key)) : -1)
#define map_remove(t, key) map__remove(map_h((t)), (t), (key), sizeof(*(t)))
#define map_len(t) buf_len((t))


isize map__find_index(MapHdr* hdr, u64 key);
bool  map__remove(MapHdr* hdr, void* map, u64 key, isize el_size);
void* map__push(MapHdr* hdr, void* map, u64 key, isize el_size);










// ===========================
// = String                  =
// ===========================

typedef struct str str;
struct str { cstr ptr; isize len; };


#define cstr_len(s) ((isize)strlen(s))
str str_from_cstr(cstr s);
cstr str_to_cstr(str t);
cstr cstr_clone(cstr s);

typedef Buf(cstr) StrIntern;
cstr cstr_intern(StrIntern* h, cstr str);










// ===========================
// = AST                     =
// ===========================


typedef struct Expr Expr;
typedef struct InitList InitList;
typedef struct IfExpr IfExpr;
typedef struct SwitchCase SwitchCase;
typedef struct SwitchExpr SwitchExpr;
typedef struct Type Type;




enum {
	EXPR_INFIX, // + - * % > < >= <= == != ..  ..< orelse and or ->
	EXPR_GET, // .
	EXPR_POSTFIX, // .- .* .& .!
	EXPR_UCAST,
	EXPR_CAST,
	EXPR_CALL,
	EXPR_INDEX,
	EXPR_TYPE_INIT,
	EXPR_IF,
	EXPR_SWITCH,

	EXPR_COUNT
};


struct Expr {
	u32 pos;
	u16 flags;
	u8 kind;
	u8 op;
	union {
		struct {
			Expr* l;
			union {
				Type*          eucast;   // expr '.(' type ')'
				Type*          ecast;    // expr 'as' '('? type ')'?
				Buf(Expr*)     ecall;    // expr '(' [ expr { ',' expr } ] ')'
				Buf(InitList*) einit;    // expr '{' ... '}'
				IfExpr*        eif;      // expr 'if' expr 'else' expr
				SwitchExpr*    eswitch;  // expr 'switch' '{' ... '}'
				Expr*          eindex;   // expr '[' expr ']'
				Expr*          er;       // expr op expr
				cstr           er_ident; // expr '.' ident
			};
		};
		f64  efloat;
		i64  eint;
		bool ebool;
		cstr eident;
		cstr estr;
	};
};



struct InitList {
	Expr* l;
	Expr* r;
	bool index;
};

struct IfExpr {
	Expr* l;
	Expr* r;
	Expr* cond;
};

enum {
	CASE_ELSE,
	CASE_EXPRL
};

struct SwitchCase {
	union {
		struct { cstr sname; Type* stype; };
		Buf(Expr*) sexpr;
	};
	u32 pos;
	u32 sname_pos; // 0 = else, 1 = EXPRL
};

struct SwitchExpr {
	SwitchCase scase;
	Expr* sif;
	Expr* node;
};

struct Type {
	isize tsize; // -1 = platform defined
	isize align;
	union {
		Type*                          sptr;    //  *
		Type*                          sparr;   // [*]
		Type*                          sslice;  // []
		struct { Type* t; Expr* len; } sarr;    // [N]
		Map(Expr*)                     senum;   // enum{...}
		Map(Type*)                     sstruct; // struct{...}
		Buf(Type*)                     sunion;  // union{...}
		cstr                           snamed;  // ident
	};
	u8 kind;
};




#if __STDC_VERSION__ >= 201112L /* c11 */
	_Static_assert(EXPR_COUNT <= 256, "Expr tag must fit in an u8");
#endif



#endif
