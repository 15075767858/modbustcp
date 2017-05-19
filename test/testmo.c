#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    char a[100];
    a = (char *)calloc(100, 1);

    memset(a,0,110);
    printf("%s \n",a);

    return 0;
}