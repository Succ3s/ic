#include "src/common.c"

int main() {
	#ifdef TMAIN
	return TMAIN();
	#endif
	return 0;
}
