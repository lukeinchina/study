#include <stdio.h>
#include <stdlib.h>

int rangeBitwiseAnd(int m, int n) {	
	int i,v;
	v = 0;
	for (i = 31; i >= 0; i--) {
		if (m & (1<<i)) {
			v = (1<<i);
			break;
		}
	}
	for (; i >= 0; i--) {
		if (n >= (v + (1<<i) )) {
			return v;
		}
		v = (v + (1<<i)) & m;
		printf("[%d] %d\n", i, v);
	}
	return v;
}

int 
main(int argc, char *argv[]) {
	int m, n;
	if (argc < 3) {
		printf("usage:%s m n\n", argv[0]);
		return 1;
	}
	m = atoi(argv[1]);
	n = atoi(argv[2]);
	printf("%d %d %d\n", m, n, rangeBitwiseAnd(m, n));
	return 0;
}
