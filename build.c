#include "src/common.c"
#include "src/lex.h"

int main() {
	#ifdef TMAIN
	return TMAIN();
	#endif
	return 0;
}
