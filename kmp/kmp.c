#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Function prototypes
void compute_prefix_function(const char *P, int m, int *pi);
void kmp_matcher(const char *T, const char *P, int n, int m);
double benchmark_kmp(const char *text, const char *pattern, int text_size, int pattern_size);
void run_benchmark(const char *file_path, const char *pattern, int pattern_size, int k, const char *output_file);
char *generate_pattern_multiple(const char *pattern, int pattern_length, int multiplier);

void compute_prefix_function(const char *P, int m, int *pi) {
    pi[0] = 0;
    int k = 0;

    for (int q = 1; q < m; q++) {
        while (k > 0 && P[k] != P[q]) {
            k = pi[k - 1];
        }
        if (P[k] == P[q]) {
            k++;
        }
        pi[q] = k;
    }
}

void kmp_matcher(const char *T, const char *P, int n, int m) {
    int pi[m];

    compute_prefix_function(P, m, pi);

    int q = 0;
    for (int i = 0; i < n; i++) {
        while (q > 0 && P[q] != T[i]) {
            q = pi[q - 1];
        }
        if (P[q] == T[i]) {
            q++;
        }
        if (q == m) {
            printf("Pattern occurs with shift %d\n", i - m + 1);
            q = pi[q - 1];
        }
    }
}

// Benchmark a single execution of the KMP algorithm
double benchmark_kmp(const char *text, const char *pattern, int text_size, int pattern_size) {
    clock_t start_time = clock();
    kmp_matcher(text, pattern, text_size, pattern_size);
    clock_t end_time = clock();
    return ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
}

// Run the benchmark `k` times for a given pattern
void run_benchmark(const char *file_path, const char *pattern, int pattern_size, int k, const char *output_file) {
    // Load the text from the file
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *text = malloc(file_size + 1);
    if (!text) {
        perror("Failed to allocate memory");
        fclose(file);
        exit(1);
    }

    fread(text, 1, file_size, file);
    text[file_size] = '\0';
    fclose(file);

    // Open the output file
    FILE *results_file = fopen(output_file, "w");
    if (!results_file) {
        perror("Failed to open output file");
        free(text);
        exit(1);
    }

    // Run the benchmark `k` times for the given pattern
    for (int i = 0; i < k; i++) {
        double execution_time = benchmark_kmp(text, pattern, file_size, pattern_size);
        fprintf(results_file, "Run %d: %.6f seconds\n", i + 1, execution_time);
        printf("Run %d: %.6f seconds\n", i + 1, execution_time); // Optional: print to console
    }

    // Clean up
    fclose(results_file);
    free(text);
}

// Generate a pattern by concatenating it `multiplier` times
char *generate_pattern_multiple(const char *pattern, int pattern_length, int multiplier) {
    int total_length = pattern_length * multiplier;

    char *result = malloc(total_length + 1);
    if (!result) {
        perror("Failed to allocate memory for pattern multiple");
        exit(1);
    }

    result[0] = '\0'; // Start with an empty string
    for (int i = 0; i < multiplier; i++) {
        strcat(result, pattern);
    }

    return result;
}

// Main function
int main() {
    const char *file_path = "htt.txt";    // Input text file
    const char *base_pattern = "cag"; // Base pattern
    const int base_pattern_length = strlen(base_pattern);
    int k = 5;                            // Number of test runs for each pattern
    int L = 4;                            // Maximum multiplier for the pattern
    const char *output_file = "results.txt"; // Output file to store results

    // Run benchmarks for the base pattern and its multiples
    FILE *results_file = fopen(output_file, "w");
    if (!results_file) {
        perror("Failed to open output file");
        exit(1);
    }

    for (int multiplier = 1; multiplier <= L; multiplier++) {
        char *current_pattern = generate_pattern_multiple(base_pattern, base_pattern_length, multiplier);

        fprintf(results_file, "Benchmarking pattern of length %ld (multiplier %d):\n", 
                strlen(current_pattern), multiplier);
        printf("Benchmarking pattern of length %ld (multiplier %d):\n", 
                strlen(current_pattern), multiplier);

        run_benchmark(file_path, current_pattern, base_pattern_length * multiplier, k, output_file);

        free(current_pattern);
    }

    fclose(results_file);
    printf("Benchmark complete. Results saved to '%s'.\n", output_file);

    return 0;
}
