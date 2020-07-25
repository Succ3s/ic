#include "src/common.h"
#include "src/lex.c"
// #include "src/parse.c"

#include "src/ir.h"

int main() {
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

	Hospital* h = &(Hospital){0};
	hospital_init(h);
	__builtin_printf("%d\n", hospital_intern(h, "hello"));
	__builtin_printf("%d\n", hospital_intern(h, "world"));
	__builtin_printf("%d\n", hospital_intern(h, "world"));
	__builtin_printf("%d\n", hospital_find_patient(*h, "hello"));
	__builtin_printf("%d\n", hospital_intern(*h, "FAWGFKAWBAUWIGBAWÇBGUAWUGWAUGWOFGWAUFGUAWUGFIAWGFWAIFGAWFG"));

	return 0;
// #	ifdef IR_TEST
// 	ir_test();
// 	return 0;
// #	endif




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
	// 	.file      = string_from_cstring("./build.c"),
	// 	.lex       = &lex,
	// };
}
