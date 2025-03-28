#include <stdio.h>

volatile int* n = (int*)0;
static int* a = (int*)(1*sizeof(int));

int kernel() {
    int N = *n;
    int sum = 0;
    int i;
    for (i = 0; i < N; i++) 
        sum += a[i];
    return sum;
}

// Simple accumulation
int main() {

    kernel();
}
