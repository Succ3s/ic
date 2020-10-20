#ifndef TTN_HEADER_PARSE
#define TTN_HEADER_PARSE

// TODO(pgs): parse types
// TODO(pgs): arena allocator

//










// ===========================
// = Sym
//

// forward decl
struct Type;
struct Value;

typedef struct Sym Sym;
struct Sym {
	i32 scope;
	Pos p;
	cstr name;
	struct Type* type;
	Value val;
};

Sym* sym_find(Buf(Sym) symbols, cstr name);
void sym_scope_remove(Buf(Sym)* symbols, i32 scope);

//










// ===========================
// Compiler Context

typedef struct Ctx Ctx;
struct Ctx {
	Sources* srcs;
	FILE* report;
	Buf(char) out;
	cstr pkgname;
	// symbol table
	Buf(Sym) symbols;
	i32 scope;
};

//










// ===========================
// Constant Value

// TODO(pgs)
typedef struct Value Value;
struct Value {
	isize x;
};

//










// ===========================
// Type


// TODO(pgs): proc, struct, union, enum
enum {
	// no data
	TY_UINT8,
	TY_UINT16,
	TY_UINT32,
	TY_UINT64,
	TY_UINT,
	TY_INT8,
	TY_INT16,
	TY_INT32,
	TY_INT64,
	TY_INT,
	TY_BOOL,
	TY_STRING,
	TY_CSTRING,

	// data
	TY_NAMED,
	TY_POINTER,
	TY_ARRAY,
	TY_SLICE
};

typedef struct Type Type;
struct Type {
	u8 kind;
	union {
		struct {
			Type* ty;
			cstr name;
		} named;
		struct {
			Value* val;
			Type*  ty;
		}     array;
		Type* slice;
		Type* pointer;
	};
};

//










void parse_file(Sources* srcs, isize file_id, Buf(char)* out, FILE* rep);
void parse_item(Lexer* l, Ctx* ctx);
Type* parse_type(Lexer* l, Ctx* ctx);
void parse_block(Lexer* l, Ctx* ctx);

#if 1
#define TMAIN test_parser
int test_parser(int argc, cstr* argv) {
	Sources s = {0};
	isize idx = sources_add(&s, "package foo; proc puts(x: int); proc foo(x: int, y: int) *int {}", "build.ttn");
	Buf(char) buff = {0};
	parse_file(&s, idx, &buff, stderr);
	buf_push(buff, '\0');
	printf("====== OUTPUT ======\n %s \n====================", buff);

	return 0;
}
#endif


#endif
