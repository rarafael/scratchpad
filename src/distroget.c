/*
 * a neofetch-like program for Linux/MacOS
 *
 * Usage: distroget
 *
 * does not display used memory on MacOS.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if (defined(__linux__) || defined(linux))
#include <sys/sysinfo.h>
#else /* MacOS */
#include <sys/sysctl.h>
#endif /* Linux */

#include <sys/utsname.h>
#include <pwd.h>

#define PERR_AND_EXIT(S)        \
    {                           \
        perror(S);              \
        exit(EXIT_FAILURE);     \
    }

#define DEFAULT_STRSIZE (1 << 7)

#define GENTOO_COLOUR   "\033[38;5;5m"
#define GENERIC_COLOUR  "\033[38;5;15m"
#define MACOS_COLOUR    "\033[38;5;13m"
#define RESET_COLOUR    "\033[0m"

static struct {
    char os[DEFAULT_STRSIZE];
    char uptime[DEFAULT_STRSIZE];
    char user[DEFAULT_STRSIZE];
    char shell[DEFAULT_STRSIZE];
    char hostname[DEFAULT_STRSIZE];
    unsigned long mem_total;
    unsigned long mem_used;
#if (defined(__linux__) || defined(linux))
    char distro[DEFAULT_STRSIZE];
#endif /* Linux */
    char colour[DEFAULT_STRSIZE];
} fetch;

#if (defined(__linux__) || defined(linux))
void getdistro(char *namestr, char *colourstr)
#else /* MacOS */
void getdistro(char *colourstr)
#endif
{
#if (defined(__linux__) || defined(linux))
    FILE *fp = fopen("/etc/os-release", "r");
    if (!fp)
        PERR_AND_EXIT("distroget: getdistro");

    if (!fscanf(fp, "%*[^P]PRETTY_NAME=\"%[^\"]", namestr)) {
        fclose(fp);
        PERR_AND_EXIT("distroget: getdistro");
    }

    fclose(fp);
    
    if (!strcmp(namestr, "Gentoo Linux")) {
        strncpy(colourstr, GENTOO_COLOUR, DEFAULT_STRSIZE);
        return;
    }

    strncpy(colourstr, GENERIC_COLOUR, DEFAULT_STRSIZE);
#endif /* Linux */
    strncpy(colourstr, MACOS_COLOUR, DEFAULT_STRSIZE);
}

void getuptime(char *uptimestr)
{
    long uptime = 0;
#if (defined(__linux__) || defined(linux))
    struct sysinfo si;
    if (sysinfo(&si))
        PERR_AND_EXIT("distroget: getuptime");
    uptime = si.uptime;
#else /* MacOS */
    size_t long_size = sizeof(long);
    if (sysctlbyname("kern.boottime", &uptime, &long_size, NULL, 0))
        PERR_AND_EXIT("distroget: getuptime");
#endif /* Linux */

    struct tm *b_time = gmtime(&uptime);

    if (b_time->tm_hour) {
        if (b_time->tm_yday) {
            snprintf(uptimestr, DEFAULT_STRSIZE, "%dd %dh %dm",
                     b_time->tm_yday, b_time->tm_hour, b_time->tm_min);
            return;
        }
        snprintf(uptimestr, DEFAULT_STRSIZE, "%dh %dm",
                 b_time->tm_hour, b_time->tm_min);
        return;
    }
    snprintf(uptimestr, DEFAULT_STRSIZE, "%dm",
             b_time->tm_min);
}

void getos(char *namestr, char *hostnamestr)
{
    struct utsname un;
    if (uname(&un))
        PERR_AND_EXIT("distroget: getos");
   
    char *p = un.sysname;
    while (*p != '\0')
        *namestr++ = *p++;
    *namestr++ = ' ';
    p = un.release;
    while (*p != '\0')
        *namestr++ = *p++;

    p = un.nodename;
    while (*p != '\0')
        *hostnamestr++ =  *p++;
}

void getpwd(char *namestr, char *shellstr)
{
    struct passwd *pw = getpwuid(getuid());

    char *p = pw->pw_shell;
    while (*p != '\0')
        *shellstr++ = *p++;

    p = pw->pw_name;
    while (*p != '\0')
        *namestr++ = *p++;
}

void getmem(unsigned long *total, unsigned long *used)
{
#if (defined(__linux__) || defined(linux))
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fscanf(fp, "MemTotal:\t%lu", total)) {
        fclose(fp);
        PERR_AND_EXIT("distroget: getmem");
    }
    *total >>= 10;

    if (!fscanf(fp, "%*[^A]Available:\t%lu", used)) {
        fclose(fp);
        PERR_AND_EXIT("distroget: getmem");
    }
    *used >>= 10;

    *used = *total - *used;
    fclose(fp);
#else /* MacOS */
    size_t long_size = sizeof(long);
    if (sysctlbyname("hw.memsize", total, &long_size, NULL, 0))
        PERR_AND_EXIT("distroget: getmem");
    *total >>= 20;
    *used = 0; /* no way to get memory usage on MacOS? */
#endif /* Linux */
}

int main(void)
{
    getos(fetch.os, fetch.hostname);
    getuptime(fetch.uptime);
    getpwd(fetch.user, fetch.shell);
    getmem(&fetch.mem_total, &fetch.mem_used);

#if (defined(__linux__) || defined(linux))
    getdistro(fetch.distro, fetch.colour);
#else /* MacOS */
    getdistro(fetch.colour);
#endif /* Linux */

    fprintf(stdout,
    "%s%13s%s@%s%s\n"
    "          %sOS%s: %s\n"
#if (defined(__linux__) || defined(linux))
    "      %sDistro%s: %s\n"
#endif /* Linux */
    "      %sUptime%s: %s\n"
    "  %sUser Shell%s: %s\n"
    "      %sMemory%s: %luMiB/%luMiB (%ld%%)\n",

    fetch.colour, fetch.user, RESET_COLOUR, fetch.colour, fetch.hostname,
    fetch.colour, RESET_COLOUR, fetch.os,
#if (defined(__linux__) || defined(linux))
    fetch.colour, RESET_COLOUR, fetch.distro,
#endif /* Linux */
    fetch.colour, RESET_COLOUR, fetch.uptime,
    fetch.colour, RESET_COLOUR, fetch.shell,
    fetch.colour, RESET_COLOUR, fetch.mem_used, fetch.mem_total,
    (long)(((float)(fetch.mem_used) / (float)(fetch.mem_total)) * 100));

    return EXIT_SUCCESS;
}
