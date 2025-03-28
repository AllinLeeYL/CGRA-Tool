#define NTAPS 32

float input[NTAPS];
float output[NTAPS];
float coefficients[NTAPS] = {0.25, 1.50, 3.75, -2.25, 0.50, 0.75, -3.00, 1.25,
0.25, 1.50, 3.75, -2.25, 0.50, 0.75, -3.00, 1.25,
0.25, 1.50, 3.75, -2.25, 0.50, 0.75, -3.00, 1.25,
0.25, 1.50, 3.75, -2.25, 0.50, 0.75, -3.00, 1.25};

// Sigmoid activation function and its derivative
inline double ReLU(double x) {
    return x > 0 ? x : 0;
}

// Forward propagation for a single layer
void kernel(double inputs[], int input_size, 
            double outputs[], int output_size, 
            double biases[], double weights[][]) {
    for (int i = 0; i < output_size; i++) {
        // Calculate the weighted sum of inputs
        double activation = biases[i];
        for (int j = 0; j < input_size; j++) {
            activation += inputs[j] * weights[j][i];
        }
        // Apply the activation function
        outputs[i] = ReLU(activation);
    }
}

int main()
{

//  input_dsp (input, NTAPS, 0);

  kernel(input, NTAPS, output, NTAPS, coefficients);

//  output_dsp (input, NTAPS, 0);
//  output_dsp (coefficients, NTAPS, 0);
//  output_dsp (output, NTAPS, 0);
  return 0;
}
