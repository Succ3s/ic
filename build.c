#include "src/lex.c"
#include "src/common.h"
#include "src/parse.h"
#include "src/parse.c"

// #include "src/common.h"

int main() {
	// print_size();
	Allocator stdl = std_allocator();
	struct GrowingBumpAllocator bump;
	Allocator al = growing_bump_allocator(&bump, &stdl, megabytes(30));
	Lexer lex = lex_build(
		"#package hello\n"                           // ok 
		// "#import \"foo\"\n"                       // error
		// "#import bar 10\n"                        // error
		"#import bar \"bar\"\n"                      // ok
		// "alias false\n"                           // error
		// "type foo aaa\n"                          // error
		"type bar aaa;\n"                            // ok
		"type baz *aaa;\n"                           // ok
		"type fo1 []aa;\n"                           // ok
		// "type fo2 [10]aa;\n"                      // error, precisa fazer o parser de expr :peens
		"type fo3 struct {}\n"                       // ok
		"type fo4 struct { x: int }\n"               // ok
		"type fo5 struct { x: *int, y: int }\n"      // ok
		"type fo6 union  { int, struct {}, *int }\n" // ok
		"type fo7 enum   { hello, hi, hello }\n"     // ok
	);
	Parser p = (Parser) {
		.allocator = &al,
		.file      = string_from_cstring("./build.c"),
		.lex       = &lex,
	};

	AstFile* a = parse(&p);
	if(p.flags & PARSE_FAILED) {
		for(ChunkedListNode(Error, 8)* e = p.errors.head; e != null; e = e->next) {
			for(int i = 0; i < e->count; i++) {
				print_error(e->data[i]);
			}
		}
	}
	printf("pkg name: %s\n", a->package->name);
	for(ChunkedListNode(AstItemPtr, 8)* n = a->items.head; n != null;n=n->next) {
		for(u8 i = 0; i < n->count; i++) {
			switch(n->data[i]->item_kind) {
				case ItemKindImport:
					printf("IMPORT[%d:%d][%d:%d]:\nnamespace: %s\npath: %s\n\n",
						n->data[i]->loc->begin.line,
						n->data[i]->loc->begin.column,
						n->data[i]->loc->end.line,
						n->data[i]->loc->end.column,
						n->data[i]->item.import->namespace,
						n->data[i]->item.import->path
					);
					break;
				case ItemKindTypeDecl:
					printf("TYPE DECL[%d:%d][%d:%d]:\nis alias: %d\nname: %s\ntype: %x\n\n",
						n->data[i]->loc->begin.line,
						n->data[i]->loc->begin.column,
						n->data[i]->loc->end.line,
						n->data[i]->loc->end.column,
						n->data[i]->item.type_decl->is_alias,
						n->data[i]->item.type_decl->new_name,
						n->data[i]->item.type_decl->type
					);
			}
		}

	}
}
