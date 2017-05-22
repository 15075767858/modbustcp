
#include <stdio.h>
#include <signal.h>
#include "../memwatch-2.71/memwatch.h"

#ifndef SIGSEGV
#error "SIGNAL.H does not define SIGSEGV; running this program WILL cause a core dump/crash!"
#endif

#ifndef MEMWATCH
#error "You really, really don't want to run this without memwatch. Trust me."
#endif

#if !defined(MW_STDIO) && !defined(MEMWATCH_STDIO)
#error "Define MW_STDIO and try again, please."
#endif

int main()
{
    char *p;

    p = malloc(300);
    
    return 0;
}