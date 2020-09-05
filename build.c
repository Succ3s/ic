#include "src/common.c"
#include "src/lex.h"

int main(int argc, cstr* argv) {
	#ifdef TMAIN
	return TMAIN(argc, argv);
	#endif
	return 0;
}
