#define ARRAY_SIZE 16

void process_signal(const int src[ARRAY_SIZE], int dst[ARRAY_SIZE]) {
    int coefficient = 3;
    int accumulator = 0;

    // A perfectly statically-schedulable loop
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        int product = src[i] * coefficient;
        accumulator += product; // Loop-carried dependency
        dst[i] = accumulator;
    }
}