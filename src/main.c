/*
C program which emulates SPI data transfer
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void sender(bool isMsb);
void print(char* input);
void receiver(char* input, bool isMsb);

void stringcat(char* dest, const char src);
size_t str_len (const char *str);
char* copy_string(char *dest, const char *src, int n);
char* append_strings(char* dest, const char* src, int n);
void reverse_string(char *head);
char* merge_chars(char* dest, const char* src, size_t n);

int main(){
    
    bool isMsb = true; //Change this variable for MSB or LSB

    if (isMsb == false){
        printf("--lsb first--\n");
    }else{
        printf("--msb first--\n");
    }
    
    sender(isMsb);
    return 0;
}

/* Reverse string */
void reverse_string(char *head){
  char *tail = head;
  while(*tail){ // Find end null 
      tail++;
  }    
  tail--;  //Tail points to last character
  for( ; head < tail; head++, tail--) {
      // Swapping until both pointers meet in the middle
      char h = *head, t = *tail;
      *head = t;           
      *tail = h;
  }
}

/* Calculate length of string */
size_t str_len (const char *str)
{
    for (size_t len = 0;;++len){
        if (str[len]==0){
            return len;
        } 
    } 
}

/* Append one string to another string */
char* append_strings(char* dest, const char* src, int n){
	
    char* ptr = dest;

	while (*src && n--){
		*dest = *src;
		dest++;
		src++;
	}
    
    *dest = '\0';
	return ptr;
}

/* Copy string from dest to src */
char* copy_string(char *dest, const char *src, int n) {
    int i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    while (i < n) {
        dest[i++] = '\0';
    }
    return dest;
}

/* Concatenate a character to a string */
void stringcat(char* dest, const char src){

    int i;
    for( i = 0; dest[i] != '\0'; i++);
    dest[i] = src;
    dest[i+1] = '\0';
}

/* Implementing own version of strncat to merge two char arrays */
char* merge_chars(char* dest, const char* src, size_t n)
{
	// make ptr point to the end of destination string
	char* ptr = dest + strlen(dest);

	// Appends characters of source to the destination string
	while (*src != '\0' && n--)
		*ptr++ = *src++;


	*ptr = '\0';
	return dest;
}

/* Reverse every byte*/
char* reverse_bytes(char* input){
    int i;
    char new[1024] = {0};
    char reversed[1024] = {0};

    char *ptr = input;
    for (i = 0; i < str_len(ptr)+8; i++, ptr += 8){
        copy_string(new, ptr, 8);
        reverse_string(new);
        merge_chars(reversed, new, sizeof(reversed));   
    }    
    input = reversed;
    return input;
}

/* Read data from file, convert into binary and call 'receiver' function */
void sender(bool isMsb){

    FILE *fp;
    char buff[1024] = {0};

    fp = fopen("../data/input.txt", "r");
    fgets(buff, sizeof(buff), fp);
    printf("Input data: %s\n\n", buff);

    char *ptr = buff;
    char binaryNum[1024] = {0};
    int i;

    //Converting ASCII characters to binary
    for(; *ptr != 0; ++ptr){

        // Do bitwise AND for every bit of the character
        for(i = 7; i >= 0; --i){
            if (*ptr & 1 << i){
                stringcat(binaryNum, '1');
            }else
            {
                stringcat(binaryNum, '0');
            }
        } 
    }
    char *reversed = NULL;
    if (isMsb == true){
        //Reverse every 8 bits (1 byte)
        reversed = reverse_bytes(binaryNum);
        
        //Copying pointer to char array
        append_strings(binaryNum, reversed, sizeof(binaryNum));
        binaryNum[sizeof(binaryNum) - 1] = '\0';
    }

    print(binaryNum);

    if (isMsb == true){
        receiver(binaryNum, true);
    }else{
        receiver(binaryNum, false);
    }
    
}

/* Print CS, CLK and MOSI values */
void print(char* input){

    char *ptr = input;
    char clk[1024] = {0};
    char cs[1024] = {0};
    bool is0 = false;
    int i, spaceCount;
    
    //Setting values for CS
    for (i = 0; i < 2; i++){ //First three bits should be 110
        stringcat(cs, '1');
    }
    stringcat(cs, '0');
    for (; *ptr != 0; ++ptr){
        stringcat(cs, '0');
    }

    stringcat(cs, '0');
    for (i = 0; i < 2; i++){ //Last three bits should be 011
        stringcat(cs, '1');
    }
    
    //Setting values for CLK
    ptr = input;
    for (; *ptr != 0; ++ptr){
        if (is0){
            stringcat(clk, '1');
            is0 = false;
        }else{
            stringcat(clk, '0');
            is0 = true;
        }
    }

    //Printing CS values
    printf("CS:    ");
    for (i = 0; i < 3; i++){
        printf("%c", cs[i]);
    }
    printf(" ");
    for (i = 0; i < str_len(cs); i++){
        if (i % 8 == 0 && i != 0){
            printf(" ");
        }
        printf("%c", cs[i+3]);
    }
    printf(" ");
    printf("\n");

    //Printing CLK values
    printf("CLK:       ");
    for (i = 0; i < str_len(clk); i++){
        if (i % 8 == 0 && i != 0){
            printf(" ");
        }
        printf("%c", clk[i]);
    }
    printf("\n");

    //Printing MOSI values
    printf("MOSI:      ");
    for (i = 0; i < str_len(input); i++){
        if (i % 8 == 0 && i != 0){
            printf(" ");
        }
        printf("%c", input[i]);
    }
    printf("\n");
}

/* Receive binary number, convert it into char and write to file */
void receiver(char* input, bool isMsb)  {

    FILE *fp;
    int i;
    char c;
    char new[1024] = {0};
    char output[1024] = {0};

    char *reversed = NULL;
    char *ptr;

    if (isMsb == true){
        //Reverse every 8 bits (1 byte)
        reversed = reverse_bytes(input);
        ptr = reversed;
    }else{
        ptr = input;
    }

    // Converting binary back to char
    for (i = 0; i < str_len(ptr)+8; i++, ptr += 8){
        copy_string(new, ptr, 8);
        c = strtol(new, 0, 2);
        stringcat(output, c);
    }
    
    printf("\nData read (MISO): %s\n", output);
    
    fp = fopen("../data/input.txt", "w");
    fputs(output, fp);
    fclose(fp);
}
