/*
 * many-sorts: collection of sort algorithms
 *
 * this file contains a couple of sorting algorithms written in C and when
 * compiled will test (some) of them for their performance and ability to
 * sort out a random array of int type characters. Size of array can be altered
 * before compiling with the constant ARRAY_SZ
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* change at compile time for greater array size */
#ifndef ARRAY_SZ
#   define ARRAY_SZ (1 << 15)
#endif

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
bool run_and_check(size_t s, int arr[s], sort_func f, int *sort)
{
    memcpy(sort, arr, ARRAY_SZ * sizeof(int));

    f(s, sort);

    for (size_t i = 0; i < s - 1; i++) {
        if (sort[i] > sort[i + 1])
            return false;
    }

    return true;
}

/* macro to test a sort algorithm and print its usage time */
#define TEST_SORT(A)                                        \
    {                                                       \
        time_t t = time(NULL);                              \
        if (!run_and_check(ARRAY_SZ, array, A, arr_cp)) {    \
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
 * GNOME SORT
 */
void gnome_sort(size_t s, int arr[s])
{
    size_t i = 0;

    while(i < s) {
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

int main(void)
{
    int array[ARRAY_SZ]; /* constant array */
    int arr_cp[ARRAY_SZ]; /* copy to be sorted */

    time_t t = time(NULL);
    fill_array(ARRAY_SZ, array);
    time_t t2 = time(NULL);
    t = t2 - t;
    print_func_time("time to fill array", t);
    memcpy(arr_cp, array, ARRAY_SZ * sizeof(int));

    TEST_SORT(qsort_wrapper);
    if (ARRAY_SZ > (1 << 14))
        printf("bubblesort: not testing (array too large for a feasible time)\n");
    else
        TEST_SORT(bubblesort);
    TEST_SORT(insertion_sort);
    TEST_SORT(gnome_sort);

    return EXIT_SUCCESS;
}
