#include <stdio.h>

volatile int * N = (int*)0xf00;
static int *a = (int*)0xa00;
static int *b = (int*)0xb00;
static int *m = (int*)0xe00;

int main() {
    int i, j;
    int n = *N;

    int sum = 0;

    for (i = 1; i < n-1; i++) {
        //DFGLoop: loop
        b[i] = a[i] * 10 + a[i + 1] * 20; //+ a[i+ 2] * 3;
    }

    return 0;
}