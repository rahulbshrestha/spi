#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void sender(bool isLsb);
void print(char* input);
void receiver(char* input, bool isLsb);

int main(){
    bool isLsb = false;
    sender(false);
    return 0;
}

void sender(bool isLsb)
{   
    FILE *fp;
    char buff[255];

    fp = fopen("../data/input.txt", "r");
    fgets(buff, sizeof(buff), fp);
    printf("Input data: %s\n", buff);
    
    char *ptr = buff;
    char *binaryNum = malloc( sizeof(char) * 100);
    int i;
    for(; *ptr != 0; ++ptr)
    {
        // Do bitwise AND for every bit of the character
        for(i = 7; i >= 0; --i){
            if (*ptr & 1 << i){
                strncat(binaryNum, "1", 1);
            }else
            {
                strncat(binaryNum, "0", 1);
            }
        } 
    }

    print(binaryNum);
    receiver(binaryNum, false);
    free(binaryNum);
}

void print(char* input)
{   
    char *ptr = input;
    char *clk  = malloc( sizeof(char) * 100);
    char *cs = malloc(sizeof(char) * 100);
    bool is0 = false;
    int i;
    
    //Setting values for CS
    
    for (i = 0; i < 2; i++){ //First three bits should be 110
        strncat(cs, "1", 1);
    }
    strncat(cs, "0", 1);

    for (; *ptr != 0; ++ptr)
    {
        strncat(cs, "0", 1);
    }

    strncat(cs, "0", 1);
    for (i = 0; i < 2; i++){ //Last three bits should be 011
        strncat(cs, "1", 1);
    }
    
    //Setting values for CLK

    ptr = input;
    for (; *ptr != 0; ++ptr)
    {
        if (is0){
            strncat(clk, "1", 1);
            is0 = false;
        }else{
            strncat(clk, "0", 1);
            is0 = true;
        }
    }
    
    printf("CS:        %s\n", cs);
    printf("CLK:       %s\n", clk);
    printf("MOSI:      %s\n", input);

}

void receiver(char* input, bool isLsb)  {
    FILE *fp;
    char buff[255];
    
    printf("\nData read (MISO): %s\n", input);
    
    fp = fopen("../data/output.txt", "w");
    fputs(input, fp);
    fclose(fp);
}
