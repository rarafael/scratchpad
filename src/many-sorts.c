/*
 * many-sorts: collection of sort algorithms
 *
 * Usage: many sorts [ARRAY_SIZE] [PASSES]
 *
 * this file contains a couple of sorting algorithms written in C and when
 * compiled will test (some) of them for their performance and ability to
 * sort out a random array of int type characters. 
 *
 * Size of array is given in the first argument as a power of 2, and the number
 * of passes can be given as the second argument
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef void (*sort_func)(size_t s, int arr[s]);

/*
 * UTILITY
 */
void print_func_time(char *f, time_t t)
{
    if (t > 60)
        printf("%s: %ldm %lds\n", f, t / 60, t % 60);
    else
        printf("%s: %lds\n", f, t);
}

void fill_array(size_t s, int array[s])
{
    srand(time(NULL));

    for (size_t i = 0; i < s; i++) {
        array[i] = rand();
        srand(array[i]);
    }
}

/* runs a sort algorithm and checks if array is sorted */
bool run_and_check(size_t s, int arr[s], sort_func f, long passes)
{
    for(int i = 0; i < passes; i++) {
        fill_array(s, arr);

        f(s, arr);

        for (size_t i = 0; i < s - 1; i++) {
            if (arr[i] > arr[i + 1])
                return false;
        }
    }

    return true;
}

/* macro to test a sort algorithm and print its usage time */
#define TEST_SORT(A)                                        \
    {                                                       \
        time_t t = time(NULL);                              \
        if (!run_and_check(array_size, array, A, passes)) { \
            fprintf(stderr, #A ": array is not sorted\n");  \
            return EXIT_FAILURE;                            \
        }                                                   \
        time_t t2 = time(NULL);                             \
        t = t2 - t;                                         \
        print_func_time(#A, t);                             \
    }

/*
 * QSORT LIBC WRAPPER
 */
int comparison(const void *a1, const void *a2)
{
    if (*(int*)a1 < *(int*)a2)
        return -1;
    if (*(int*)a1 > *(int*)a2)
        return 1;
    return 0;
}

void qsort_wrapper(size_t s, int arr[s])
{
    qsort(arr, s, sizeof(int), comparison);
}

/*
 * BUBBLESORT
 */
void bubblesort(size_t s, int arr[s])
{
START:
    for (size_t i = 0; i < s - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            int swp = arr[i];
            arr[i] = arr[i + 1];
            arr[i + 1] = swp;
            goto START;
        }
    }
}

/*
 * INSERTION SORT
 */
void insertion_sort(size_t s, int arr[s])
{
    for (size_t i = 0; i < s - 1; i++) {
        for (size_t n = i; n >= 0 && arr[n] > arr[n + 1]; n--) {
            int swp = arr[n];
            arr[n] = arr[n + 1];
            arr[n + 1] = swp;
        }
    }
}

/*
 * GNOME SORT (why is this so unreasonably fast?)
 */
void gnome_sort(size_t s, int arr[s])
{
    size_t i = 0;

    while (i < s) {
        if (!i || arr[i] >= arr[i - 1])
            i++;
        else {
            int swp = arr[i];
            arr[i] = arr[i - 1];
            arr[i - 1] = arr[i];
            i--;
        }
    }
}

/*
 * COCKTAIL SHAKER SORT
 */
void cocktail_shaker_sort(size_t s, int arr[s])
{
    bool has_swapped = false;
    do {
        for (size_t i = 0; i < s - 1; i++) {
            if (arr[i] > arr[i + 1]) {
                int tmp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = tmp;
                has_swapped = true;
            }
        }

        has_swapped = false;

        for (size_t i = s - 1; i > 0; i--) {
            if (arr[i] > arr[i + 1]) {
                int tmp = arr[i];
                arr[i] = arr[i + 1];
                arr[i + 1] = tmp;
                has_swapped = true;
            }
        }
    } while (has_swapped);
}

int main(int argc, char **argv)
{
    /* default vars */
    long array_size = 15;
    long passes = 2;

    if (argc != 1) {
        if (argc < 3) {
            fprintf(stderr, "usage: many-sorts [ARRAY_SIZE] [PASSES]\n");
            return EXIT_FAILURE;
        } else {
            long array_size = strtol(argv[1], NULL, 10);
            long passes = strtol(argv[2], NULL, 10);
        }
    }

    array_size = 1 << array_size;
    int *array = malloc(sizeof(int) * array_size);

    TEST_SORT(qsort_wrapper);
    if (array_size > (1 << 14) || passes > 5)
        printf("bubblesort: not testing (array too large for a feasible time)\n");
    else
        TEST_SORT(bubblesort);
    TEST_SORT(insertion_sort);
    TEST_SORT(gnome_sort);
    TEST_SORT(cocktail_shaker_sort);

    free(array);

    return EXIT_SUCCESS;
}
