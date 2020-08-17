#ifndef G_HEADER_COMMON
#define G_HEADER_COMMON


#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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


#define bytes    (0)
#define kilobyte (1024)
#define megabyte (kilobyte*1024)
#define gigabyte (megabyte*1024)
#define terabyte (gigabyte*1024)




#define heap_alloc(s)      (malloc((s)))
#define heap_allocz(s)     (calloc(1, (s)))
#define heap_dealloc(p)    (free((p)))
#define heap_realloc(p, s) (realloc((p), (s)))

u8* alignup(u8* addr, usize align);

#define Buf(T) T*

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

#define assert(x) cwb_assert(x, __LINE__, __FILE__)
void cwb_assert(bool expr, isize line, cstr file);










// ===========================
// = String                  =
// ===========================

typedef struct str str;
struct str { cstr ptr; usize len; };


#define cstr_len(s) (strlen(s))
str str_from_cstr(cstr s);
cstr str_to_cstr(str t);
cstr cstr_clone(cstr s);

typedef Buf(cstr) StringIntern;
cstr cstr_intern(StringIntern* h, cstr str);










// ===========================
// = Buffer                  =
// ===========================

#define BUF_CAP (8)

typedef struct BufHdr BufHdr;
struct BufHdr {
	isize len, cap;
	u8 data[0];
};

#define Buf(T) T*
#define buf_h(t) ((BufHdr*)(((u8*)(t)) - offsetof(BufHdr, data)))
#define buf_len(t) ((t) != null ? buf_h((t))->len : 0)
#define buf_push(t, v) ((t) = buf__push(buf_h((t)), (t), sizeof(*(t)), 1), (t)[buf_h((t))->len++] = (v))
#define buf_remove(t, index) ((t) != null ? (memmove((t) + index, (t) + index + 1, buf_len((t))-index-1), --buf_h((t))->len) : 0)
#define buf_dealloc(t) (heap_dealloc((buf_h((t)))))

void* buf__push(BufHdr* hdr, void* arr, isize el_size, isize push_count);










// ===========================
// = AST                     =
// ===========================

typedef struct {} Type;
typedef struct {} SwitchExpr;
typedef struct {} IfExpr;
typedef struct {} InitLit;
typedef struct Expr Expr;
struct Expr {
	u16 flags;
	u8 kind;
	u8 op;
	u32 pos;
	union {
		struct {
			Expr* l;
			union {
				Type* eucast;        // expr '.(' type ')'
				Type* ecast;         // expr 'as' '('? type ')'?
				Buf(Expr*) ecall;    // expr '(' [ expr { ',' expr } ] ')'
				Buf(InitLit*) einit; // expr '{' ... '}'
				IfExpr* eif;         // expr 'if' expr 'else' expr
				SwitchExpr* eswitch; // expr 'switch' '{' ... '}'
				Expr* eindex;        // expr '[' expr ']'
				Expr* er;            // expr op expr
				cstr  er_ident;      // expr '.' ident
			};
		};
		f64  efloat;
		i64  eint;
		bool ebool;
		cstr eident;
		cstr estr;
	};
};





#endif
