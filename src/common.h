#ifndef TTN_HEADER_COMMON
#define TTN_HEADER_COMMON


#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>


// TODO(pgs): format str for each type
// TODO(pgs): parser: rewrite to generate ir
// TODO(pgs): depend less on the stdlib
// -- assert is easy to replace
// -- stdio is harder, but I'll need to write an api over the OS's filesystem, the only problem will be formating, but sprintf go brrrrrr
// -- string is used in many places, but the functions present there are easy to write
// -- stdlib is only included for the heap allocator
// -- -- heap allocations will be replaced by arenas using virtual memory or chunked allocations through the OS's native interface
// -- stddef needed for ptrdiff_t and size_t, could be replaced by manual checks
// -- stdint could be replaced by manual checks

//










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

//










// ===========================
// = Helpers                 =
// ===========================

#define internal static


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

//










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
#define buf_push(t, ...)     ((t) = buf__push(buf_h((t)), (t), sizeof(*(t)), 1), (t)[buf_h((t))->len++] = (__VA_ARGS__))
#define buf_len(t)           ((t) ? buf_h((t))->len                            : 0)
#define buf_remove(t, index) ((t) ? ((t)[(index)] = (t)[--buf_h((t))->len], 1) : 0)
#define buf_dealloc(t)       ((t) ? heap_dealloc((buf_h((t))))                 : 0)


void* buf__push(BufHdr* hdr, void* arr, isize el_size, isize push_count);

#define buf_write(Buf, Str) for(cstr s = (Str);*s;s++) { buf_push((Buf), *s); }

//










// ===========================
// = String                  =
// ===========================

typedef struct str str;
struct str { cstr ptr; isize len; };


#define cstr_len(s) ((isize)strlen(s))
str str_from_cstr(cstr s);
cstr str_to_cstr(str t);
cstr cstr_clone(cstr s);

typedef Buf(char*) StrIntern;
cstr cstr_intern(StrIntern* h, cstr str);

//










// ===========================
// = Sources                 =
// ===========================

typedef struct Position Position;
struct Position {
	i32 line, column;
	cstr path;
};
typedef i32 Pos;

typedef struct Source Source;
struct Source {
	cstr path, stream;
	i32  size, eol;
	Pos base;
};

typedef struct Sources Sources;
struct Sources {
	Buf(Source) list;
	Pos base;
};

isize sources_add(Sources* srcs, cstr stream, cstr path);
isize sources_find(Sources* srcs, Pos pos);
Position sources_position(Sources* srcs, Pos p);

#endif
