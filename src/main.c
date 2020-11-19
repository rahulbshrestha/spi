#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* sender(char* input)
{
    char *ptr = input;
    char *output = malloc( sizeof(char) * 100);
    int i;
    for(; *ptr != 0; ++ptr)
    {
        /* perform bitwise AND for every bit of the character */
        for(i = 7; i >= 0; --i){
            if (*ptr & 1 << i){
                strncat(output, "1", 1);
            }else
            {
                strncat(output, "0", 1);
            }
        } 
    }
    return output;
}

void print(char* input)
{   
    printf("CS:        %s\n", input);
    printf("CLK:       %s\n", input);
    printf("MOSI:      %s\n", input);

}

void receiver(){


}
int main(){
    char ascii_str[] = "Hello world";
    printf("\nBinary representation: %s\n", sender(ascii_str));

    // printf("ascii_str: %s\n", ascii_str);
    // printf("byte array is...\n");
    // for(i=0; i<len; i++)
    // {
    //     printf("%c - %d\n", ascii_str[i], arr[i]);
    // }
    // printf("\n");
    
    return 0;
}