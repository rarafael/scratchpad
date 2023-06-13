/*
 * Tool to convert between number radix.
 *
 * Usage: bconv [BASE] [NUMBER]
 *
 * BASE may be in the rage of 2-36, and NUMBER may not exceeded 2^64 - 1
 */

#include <stdio.h>
#include <stdlib.h>

/* symbol list up to base 36 */
const char *const SYMBOLS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void bconv(unsigned long base, long long num, char *p)
{
    unsigned long long n; 

    if (num < 0 && base != 2) {
        n = (~num) + 1;
        *p++ = '-';
    } else
        n = (unsigned long long)num;

    char *start = p;

    while (n >= base) {
        *p++ = SYMBOLS[n % base];
        n /= base;
    }
    *p = SYMBOLS[n % base];

    char *end = p;
    /* reverse the string */
    while (start < end) {
        char c = *start;
        *start++ = *end--;
        *(end + 1) = c;
    }
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fputs("bconv: [base] [number]\n", stderr);
        return EXIT_FAILURE;
    }

    char printbuf[256] = {0}; /* hard to think of a 256 digit long number */
    long base = strtol(argv[1], NULL, 10);

    if (base < 2 || base > 36) {
        fputs("bconv: invalid base, try in the range 2-36\n", stderr);
        return EXIT_FAILURE;
    }

    long long num = strtoll(argv[2], NULL, 0);
    
    bconv(base, num, &printbuf[0]); 

    puts(printbuf);
    return EXIT_SUCCESS;
}
