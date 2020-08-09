#ifndef G_HEADER_PARSE
#define G_HEADER_PARSE

#include "ast.h"
#include "lex.h"
#include "token.h"

ChunkedListImpl(Error, 8)

#define PARSE_FAILED    (1 << 0)
#define PARSE_NOPACKAGE (1 << 1)
#define PARSE_NOERRORS  (1 << 2)

typedef struct {
	Lexer*                lex;
	str                file;
	Allocator*            allocator;
	ChunkedList(Error, 8) errors;
	u8                    flags;
} Parser;

AstFile* parse(Parser* p);

bool parser_push_err(Parser* p, Error err) {
	if(p->flags & PARSE_NOERRORS)  { return true; }
	if(err.lvl == ErrorLevelError) { p->flags |= PARSE_FAILED; }
	if(err.file.ptr == null)       { err.file = p->file; }
	return ChunkedList_push(Error, 8)(&p->errors, p->allocator, err);
}

#endif
