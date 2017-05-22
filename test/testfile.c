#include <stdio.h>

int main()
{

    FILE *fp;
    fp = fopen("message.text", "a+");
    fprintf(fp, "This is testing for fprintf...\n");
    fclose(fp);
    return 0;
}