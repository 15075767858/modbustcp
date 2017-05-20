#include <stdio.h>
#include <stdint.h>
#include <string.h>
int main()
{
    union {
        uint8_t byte[8];
        float real_value;
    } my_data;
    memset(my_data.byte, 0, 8);

    my_data.byte[3] = 0x43;
    my_data.byte[2] = 0x6b;
    my_data.byte[1] = 0xb3;
    my_data.byte[0] = 0x33;
    printf("f = (%f)\n",my_data.real_value);
    
    float a = 235.4;
    my_data.real_value = a;
    int i;
    printf("data = ( \n");
    for (i = 0; i < 8; i++)
    {
        printf("%d,", my_data.byte[i]);
    }
    printf(")\n");
    return 0;
}