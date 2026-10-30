#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../mergesort.h"

int *A = NULL;
int *B = NULL;
int cutoff = 0;

static int compare_ints(const void *first, const void *second)
{
    int a = *(const int *)first;
    int b = *(const int *)second;

    return (a > b) - (a < b);
}

// Run test
// Variables:
// name: name of test
// input: array of inputs
// n: size
// test_cutoff: cutoff level
// results: file to store results
static int run_test(const char *name, const int input[], int n,
                    int test_cutoff, FILE *results)
{
    int *expected; // Point to reference array
    struct argument *args; // Holds arguments passed to sorting function
    int i;
    int passed = 1;
    size_t bytes = n * sizeof(int);

    A = malloc(bytes);
    B = malloc(bytes);
    expected = malloc(bytes);

    // Check for failed memory allocation
    if (A == NULL || B == NULL || expected == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(A);
        free(B);
        free(expected);
        exit(EXIT_FAILURE);
    }

    // Give both sorting methods the same input
    memcpy(A, input, bytes);
    memcpy(expected, input, bytes);

    // Sort the expected array using the C library
    qsort(expected, n, sizeof(int), compare_ints);

    cutoff = test_cutoff;
    args = buildArgs(0, n - 1, 0);

    if (args == NULL) {
        fprintf(stderr, "Argument allocation failed\n");
        free(A);
        free(B);
        free(expected);
        exit(EXIT_FAILURE);
    }

    parallel_mergesort(args);

    // The caller owns and frees the initial argument
    free(args);

    // Check that every value matches the expected result
    for (i = 0; i < n; i++) {
        if (A[i] != expected[i]) {
            passed = 0;
            printf("Mismatch at index %d: expected %d, got %d\n",
                   i, expected[i], A[i]);
            break;
        }
    }

    printf("%s, size %d, cutoff %d: %s\n",
           name, n, cutoff, passed ? "PASS" : "FAIL");

    // Record test details
    fprintf(results, "%s,%d,%d,", name, n, cutoff);

    // Record the original input array
    fprintf(results, "\"");
    for (i = 0; i < n; i++) {
        fprintf(results, "%s%d", i == 0 ? "" : " ", input[i]);
    }
    fprintf(results, "\",");

    // Record the expected array sorted by qsort
    fprintf(results, "\"");
    for (i = 0; i < n; i++) {
        fprintf(results, "%s%d", i == 0 ? "" : " ", expected[i]);
    }
    fprintf(results, "\",");

    // Record the actual array sorted by our function
    fprintf(results, "\"");
    for (i = 0; i < n; i++) {
        fprintf(results, "%s%d", i == 0 ? "" : " ", A[i]);
    }
    fprintf(results, "\",%s\n", passed ? "PASS" : "FAIL");

    fflush(results);

    free(A);
    free(B);
    free(expected);
    A = NULL;
    B = NULL;

    return passed;
}

int main(void)
{
    // Already sorted: check that the correct order is preserved
    int sorted[] = {1, 2, 3, 4, 5};

    // Reverse sorted: check rearrangement of descending values
    int reversed[] = {5, 4, 3, 2, 1};

    // All equal: check that repeated values are preserved
    int equal[] = {7, 7, 7, 7, 7};

    // Mixed: duplicates, negatives, zero, and integer limits
    int mixed[] = {5, -2, 5, 0, INT_MAX, INT_MIN, 3};

    // Single element: check the left >= right base case
    int single[] = {42};

    // Two elements: smallest array that requires sorting
    int pair[] = {2, 1};

    // Even size: check splitting into equal halves
    int even[] = {8, 3, 7, 1, 6, 2, 5, 4};

    // Odd size: check uneven splitting above a power of two
    int odd[] = {9, 3, 7, 1, 8, 2, 6, 4, 5};

    // Negative-only values: check ordering below zero
    int negative[] = {-3, -10, -1, -7, -5};

    // Repeated extremes: check duplicates at both integer limits
    int extremes[] = {INT_MAX, INT_MIN, INT_MAX, 0, INT_MIN};

    // Duplicates: check repeated values in different positions
    int duplicates[] = {3, 1, 3, 2, 1, 2};

    // All zeros: check an array containing only zero
    int zeros[] = {0, 0, 0, 0};

    // Sorted negatives: check that negative ascending order is preserved
    int sorted_negative[] = {-10, -5, -2, -1};

    // Larger descending input: exercise more recursive splitting
    int larger[100];

    // Cutoff 0: serial sorting without child threads
    // Cutoff 1: allow one level of child threads
    // Cutoff 3: allow deeper thread creation
    int cutoffs[] = {0, 1, 3};

    FILE *results;
    int i;
    int j;
    int passed = 0;
    int total = 0;

    // Fill the larger array with 100, 99, ..., 1
    for (j = 0; j < 100; j++) {
        larger[j] = 100 - j;
    }

    // Open the CSV file; "w" overwrites previous results
    results = fopen("testing/correctness.csv", "w");

    // End testing if the file failed to open
    if (results == NULL) {
        perror("Could not open results file");
        return EXIT_FAILURE;
    }

    // Write column names
    fprintf(results, "test,size,cutoff,input,expected,actual,result\n");

    // Fourteen inputs multiplied by three cutoffs gives 42 tests
    for (i = 0; i < 3; i++) {
        passed += run_test("sorted", sorted, 5, cutoffs[i], results);
        passed += run_test("reversed", reversed, 5, cutoffs[i], results);
        passed += run_test("equal", equal, 5, cutoffs[i], results);
        passed += run_test("mixed", mixed, 7, cutoffs[i], results);
        passed += run_test("single", single, 1, cutoffs[i], results);
        passed += run_test("pair", pair, 2, cutoffs[i], results);
        passed += run_test("even", even, 8, cutoffs[i], results);
        passed += run_test("odd", odd, 9, cutoffs[i], results);
        passed += run_test("negative", negative, 5, cutoffs[i], results);
        passed += run_test("extremes", extremes, 5, cutoffs[i], results);
        passed += run_test("duplicates", duplicates, 6, cutoffs[i], results);
        passed += run_test("zeros", zeros, 4, cutoffs[i], results);
        passed += run_test("sorted_negative", sorted_negative, 4, cutoffs[i], results);
        passed += run_test("larger_reversed", larger, 100, cutoffs[i], results);
        total += 14;
    }

    // Display a summary
    printf("\nPassed: %d\nFailed: %d\nTotal: %d\n",
           passed, total - passed, total);

    // Close the file and check for errors
    if (fclose(results) != 0) {
        perror("Could not close results file");
        return EXIT_FAILURE;
    }

    // Return success only if all tests passed
    return passed == total ? EXIT_SUCCESS : EXIT_FAILURE;
}