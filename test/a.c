#include "b.h"
#include "a.h"
#include <stdio.h>
int main(){
    printf("%s\n",a.buffer);
    add();
    printf("%s\n",a.buffer);
    return 0;
}