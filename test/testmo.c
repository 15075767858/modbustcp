#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{

    while (1)
    {
        char *a;
        a = (char *)calloc(100, 1);
        //printf("%s \n", a);
        a = realloc(a, 200);
        free(a);
    }
    return 0;
}