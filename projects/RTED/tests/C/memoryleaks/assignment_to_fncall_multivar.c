#include <stdlib.h>


int* foo() {
	int* y = (int*)malloc(sizeof(int));
  int* z = y;
	return y;
}

int main( int argc, char* argv) {
	int x[2];
	int* a;

	a = foo();
	// error, did not free memory malloc-d by foo
	a = x;

	return 0;
}

