#include "src/hospital.c"
#include "src/ir.c"
#include "src/common.c"

int main() {
	__builtin_printf("COMPILES");
	// Procedure f = {0};
	// Instr nop = {0};
	// Const noc = {0};
	// init_proc(&f, 10);
	// __builtin_printf("1: %d\n", ir_emit(&f, nop));
	// __builtin_printf("2: %d\n", ir_emit(&f, nop));
	// __builtin_printf("3: %d\n", ir_emit(&f, nop));
	// __builtin_printf("4: %d\n", ir_emit(&f, nop));
	// __builtin_printf("5: %d\n", ir_emit(&f, nop));
	// __builtin_printf("6: %d\n", ir_const(&f, noc));
	// __builtin_printf("7: %d\n", ir_const(&f, noc));
	// __builtin_printf("8: %d\n", ir_const(&f, noc));
	// __builtin_printf("9: %d\n", ir_const(&f, noc));

	// Package* p = &(Package){0};
	// package_init(p, 0);
	// __builtin_printf("1: %d\n", package_add_proc(p, 1));
	// __builtin_printf("2: %d\n", package_add_proc(p, 2));
	// __builtin_printf("3: %d\n", package_add_proc(p, 3));
	// __builtin_printf("4: %d\n", package_add_import(p, 4, str_from_cstr("std:mem")));
	// __builtin_printf("5: %d\n", package_add_import(p, 5, str_from_cstr("std:collections")));
	// __builtin_printf("6: %d\n", package_add_import(p, 6, str_from_cstr("std:os")));

	// Hospital* h = &(Hospital){0};
	// hospital_init(h);
	// __builtin_printf("%d\n", hospital_intern(h, "hello"));
	// __builtin_printf("%d\n", hospital_intern(h, "world"));
	// __builtin_printf("%d\n", hospital_intern(h, "world"));
	// __builtin_printf("%d\n", hospital_find_patient(*h, "hello"));
	// __builtin_printf("%d\n", hospital_intern(h, "FAWGFKAWBAUWIGBAWÇBGUAWUGWAUGWOFGWAUFGUAWUGFIAWGFWAIFGAWFG"));



	// Allocator stdl = std_allocator();
	// struct GrowingBumpAllocator bump;
	// Allocator al = growing_bump_allocator(&bump, &stdl, megabytes(30));
	// Lexer lex = lex_build(
	// 	"#package hello\n"                           // ok 
	// 	// "#import \"foo\"\n"                       // error
	// 	// "#import bar 10\n"                        // error
	// 	"#import bar \"bar\"\n"                      // ok
	// 	// "alias false\n"                           // error
	// 	// "type foo aaa\n"                          // error
	// 	"type bar aaa;\n"                            // ok
	// 	"type baz *aaa;\n"                           // ok
	// 	"type fo1 []aa;\n"                           // ok
	// 	// "type fo2 [10]aa;\n"                      // error, precisa fazer o parser de expr :peens
	// 	"type fo3 struct {}\n"                       // ok
	// 	"type fo4 struct { x: int }\n"               // ok
	// 	"type fo5 struct { x: *int, y: int }\n"      // ok
	// 	"type fo6 union  { int, struct {}, *int }\n" // ok
	// 	"type fo7 enum   { hello, hi, hello }\n"     // ok
	// );
	// Parser p = (Parser) {
	// 	.allocator = &al,
	// 	.file      = str_from_cstr("./build.c"),
	// 	.lex       = &lex,
	// };


	return 0;
}
