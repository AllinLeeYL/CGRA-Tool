
int fibonacci(int n) {
    // Handle boundary conditions immediately
    if (n < 0) return -1;
    if (n == 0) return 0;
    if (n == 1) return 1;

    // Static allocation of variables (maps directly to hardware registers)
    int prev2 = 0; // Represents F(i-2)
    int prev1 = 1; // Represents F(i-1)
    int current = 0;

    // Perfectly predictable loop-carried dependency chain
    for (int i = 2; i <= n; i++) {
        current = prev1 + prev2;
        prev2 = prev1; // Shift register 
        prev1 = current; // Shift register
    }

    return current;
}

int main() {
    int n = 10;
    int t = fibonacci(n);
}