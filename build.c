#include "src/common.c"
#include "src/lex.c"

int main(int argc, cstr* argv) {
	#ifdef TMAIN
	return TMAIN(argc, argv);
	#endif
	return 0;
}
