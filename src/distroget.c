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

#define DEFAULT_STRSIZE (1 << 7)

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
} fetch;

#if (defined(__linux__) || defined(linux))
void getdistro(char *namestr)
{
    FILE *fp = fopen("/etc/os-release", "r");
    if (!fp)
        perror("distroget: getdistro");

    if (!fscanf(fp, "%*[^P]PRETTY_NAME=\"%[^\"]", namestr)) {
        fclose(fp);
        perror("distroget: getdistro");
    }

    fclose(fp);
}
#endif /* Linux */

void getuptime(char *uptimestr)
{
    long uptime = 0;
#if (defined(__linux__) || defined(linux))
    struct sysinfo si;
    if (sysinfo(&si))
        perror("distroget: getuptime");
    uptime = si.uptime;
#else /* MacOS */
    size_t long_size = sizeof(long);
    if (sysctlbyname("kern.boottime", &uptime, &long_size, NULL, 0))
        perror("distroget: getuptime");
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
        perror("distroget: getos");
   
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
    struct sysinfo si;
    if (sysinfo(&si))
        perror("distroget: getmem");
    *total = si.totalram >> 20;
    *used = *total - ((si.totalram - si.freeram) >> 20);
#else /* MacOS */
    size_t long_size = sizeof(long);
    if (sysctlbyname("hw.memsize", total, &long_size, NULL, 0))
        perror("distroget: getmem");
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
    getdistro(fetch.distro);
#endif
    fprintf(stdout,
    "%13s@%s\n"
    "          OS: %s\n"
#if (defined(__linux__) || defined(linux))
    "      Distro: %s\n"
#endif
    "      Uptime: %s\n"
    "  User Shell: %s\n"
    "      Memory: %luMiB/%luMiB (%ld%%)\n",
    fetch.user, fetch.hostname, fetch.os,
#if (defined(__linux__) || defined(linux))
    fetch.distro,
#endif
    fetch.uptime, fetch.shell,
    fetch.mem_total, fetch.mem_used,
    fetch.mem_total / fetch.mem_used);
    return EXIT_SUCCESS;
}
