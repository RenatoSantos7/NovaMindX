#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Function prototypes
void kmp_matcher(const char *T, const char *P, int n, int m, int *shifts, int *count);
void compute_prefix_function(const char *P, int m, int *pi);
double single_benchmark(const char *text, const char *pattern, int *shifts, int text_size, int pattern_size, int *count);
void run_benchmark(const char *file_path, const char *pattern, int pattern_size, int multiplier, int k, FILE *results_file);
char *generate_pattern_multiple(const char *pattern, int pattern_length, int multiplier);
void longest_repeated_pattern_occurrences(int shifts[], int n, int pattern_length, int *max_length, int *start_shift) ;

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

void kmp_matcher(const char *T, const char *P, int n, int m, int *shifts, int *count) {
    int *pi;
    pi = malloc (m * sizeof(int));

    compute_prefix_function(P, m, pi);

    int q = 0;
    int local_count = 0;

    for (int i = 0; i < n; i++) {
        while (q > 0 && P[q] != T[i]) {
            q = pi[q - 1];
        }
        if (P[q] == T[i]) {
            q++;
        }
        if (q == m) {
            shifts[local_count++] = i - m + 1;
            q = pi[q - 1];
        }
    }

    *count = local_count;
    free(pi);
}

void longest_repeated_pattern_occurrences(int shifts[], int n, int pattern_length, int *max_length, int *start_shift) {
    if (n == 0) {
        *max_length = 0;
        *start_shift = -1; // Indicate no valid sequence
        return;
    }

    *max_length = 1;
    *start_shift = shifts[0];
    int current_length = 1;
    int current_start_shift = shifts[0];

    for (int i = 1; i < n; i++) {
        if (shifts[i] - shifts[i - 1] == pattern_length) {
            current_length++;
            if (current_length > *max_length) {
                *max_length = current_length;
                *start_shift = current_start_shift;
            }
        } else {
            current_length = 1;
            current_start_shift = shifts[i];
        }
    }
}

// Benchmark a single execution of the KMP algorithm
double single_benchmark(const char *text, const char *pattern, int *shifts, int text_size, int pattern_size, int *count) {
    clock_t start_time = clock();
    kmp_matcher(text, pattern, text_size, pattern_size, shifts, count);
    clock_t end_time = clock();
    return ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
}

void run_benchmark(const char *file_path, const char *pattern, int pattern_size, int multiplier, int k, FILE *results_file) {
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

    int *shifts = malloc ((file_size / pattern_size) * sizeof(int));
    if (!shifts) {
        perror("Failed to allocate shifts");
        fclose(file);
        exit(1);
    }

    int *count = malloc (sizeof(int));
    if (!shifts) {
        perror("Failed to allocate count");
        fclose(file);
        exit(1);
    }

    int *max_shift = malloc (sizeof(int));
    if (!shifts) {
        perror("Failed to allocate max_shift");
        fclose(file);
        exit(1);
    }

    int *max_length = malloc (sizeof(int));
    if (!shifts) {
        perror("Failed to allocate max_length");
        fclose(file);
        exit(1);
    }

    // Run the benchmark `k` times for the given pattern
    for (int i = 0; i < k; i++) {
        double execution_time = single_benchmark(text, pattern, shifts, file_size, pattern_size, count);
        longest_repeated_pattern_occurrences(shifts, *count, pattern_size, max_length, max_shift);
        fprintf(results_file, "%d,%d,%d,%d,%.7f\n", multiplier, i + 1, *max_length, *max_shift, execution_time);
        printf("Run %d: %.7f seconds\n", i + 1, execution_time); // Optional: print to console
    }

    // Clean up
    free(text);
    free(shifts);
    free(count);
    free(max_shift);
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
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input_file_path> <output_file_path>\n", argv[0]);
        return 1;
    }

    const char *file_path = argv[1];      // Input file path
    const char *output_file = argv[2];   // Output file path
    const char *base_pattern = "cag";    // Base pattern
    const int base_pattern_length = strlen(base_pattern);
    int k = 5;                            // Number of test runs for each pattern
    int L = 4;                            // Maximum multiplier for the pattern

    // Open the results file once in write mode
    FILE *results_file = fopen(output_file, "w");
    if (!results_file) {
        perror("Failed to open output file");
        exit(1);
    }

    fprintf(results_file, "multiplier,run,max_length,shift,time\n");

    for (int multiplier = 1; multiplier <= L; multiplier++) {
        char *current_pattern = generate_pattern_multiple(base_pattern, base_pattern_length, multiplier);

        printf("Benchmarking pattern of length %ld (multiplier %d):\n", 
                strlen(current_pattern), multiplier);

        run_benchmark(file_path, current_pattern, base_pattern_length * multiplier, multiplier, k, results_file);

        free(current_pattern);
    }

    fclose(results_file); // Close the results file at the end
    printf("Benchmark complete. Results saved to '%s'.\n", output_file);

    return 0;
}

