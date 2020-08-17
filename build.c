#include "src/common.c"

int main() {
	#ifdef TEST_MAIN
	return (TEST_MAIN)();
	#endif
	__builtin_printf("COMPILES %d", sizeof(Expr));

	return 0;
}
