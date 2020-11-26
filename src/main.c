/*
C program which emulates SPI data transfer
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void sender(bool isMsb);
void print(char* input, int fileSize);
void receiver(char* input, bool isMsb);

void *new_memset(void *v, int n, int len);
void stringcat(char* dest, const char src);
size_t str_len (const char *str);
char* copy_string(char *dest, const char *src, int n);
void reverse_string(char *head);
char* merge_chars(char* dest, const char* src, size_t n);
void print_file(char *filename);
void copy_file(char* src, char* dest);

int main(){
    
    bool isMsb = false; //Change this variable for MSB or LSB

    if (isMsb == false){
        printf("--lsb first--\n");
    }else{
        printf("--msb first--\n");
    }
    
    sender(isMsb);

    return 0;
}

/* Implementation of memset */

void *new_memset(void *v, int n, int len){
    unsigned char *p = v;
    while(len > 0){
        *p = n;
        p++;
        len--;
    }

    return v;
}

/*Print contents of file*/

void print_file(char *filename){
     //Print contents of file (this is after conversion has already been done)
    FILE *fptr; 
    char c;
    fptr = fopen(filename, "r"); 
    if (fptr == NULL) 
    { 
        printf("Cannot open file \n"); 
        exit(0); 
    } 
    c = fgetc(fptr); 
    while (c != EOF) 
    { 
        printf ("%c", c); 
        c = fgetc(fptr); 
    } 
  
    fclose(fptr);

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
size_t str_len (const char *str){
    for (size_t len = 0;;++len){
        if (str[len]==0){
            return len;
        } 
    } 
}

/* Copy string from dest to src */
char* copy_string(char *dest, const char *src, int n){
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

/* Reverse every bit in input char*/
char* reverse_bytes(char* input){
    int i;
    char new[1024] = {0};
    char reversed[1024] = {0};

    char *ptr = input;
    for (i = 0; i < str_len(ptr) + 16; i++, ptr += 8){
        copy_string(new, ptr, 8);
        reverse_string(new);
        merge_chars(reversed, new, sizeof(reversed));   
    }    
    input = reversed;
    return input;
}

/* Read data from file, convert every 20 bytes into binary and send it 'receiver' function */
void sender(bool isMsb){

    int x, numRead;
    unsigned char buffer[20];

    printf("\nInput data: ");
    print_file("../data/input.txt");
    printf("\n\n");

    FILE *fp = fopen("../data/input.txt", "rb");
    FILE *fq = fopen("../data/temp.txt", "w"); //clearing contents of file
    
    fseek(fp, 0, SEEK_END);
    int fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    
    char* read_data = (char*)malloc(fileSize * sizeof(char));
    
    for(x = 0; x < fileSize; x += 20) {
        numRead = fread(buffer, 1, 20, fp);
        if (numRead < 1) {
            printf("error\n");
            break;
        }
        if (numRead < 20) {
            new_memset(&buffer[numRead], 0, 20-numRead);
        }
        
        char *ptr = buffer;
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
            copy_string(binaryNum, reversed, sizeof(binaryNum));
            binaryNum[sizeof(binaryNum) - 1] = '\0';
        }
        
        if (isMsb == true){
            receiver(binaryNum, true);
        }else{
            receiver(binaryNum, false);
        }
        
        merge_chars(read_data, binaryNum, sizeof(read_data));   
    }
    print(read_data, fileSize);
    fclose(fp);
    fclose(fq);  

    //Copy contents of output file into original file
    copy_file("../data/temp.txt", "../data/input.txt");
    remove("../data/temp.txt");
}

/* Print CS, CLK and MOSI values */
void print(char* input, int fileSize){

    char *ptr = input;
    char clk[fileSize * 8];
    char cs[fileSize * 8];
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
    printf("\nData read (MISO): ");
    print_file("../data/input.txt");
    printf("\n");
}

void copy_file(char* src, char* dest){
    FILE *fp1, *fp2;
    int a;

    fp1 = fopen(src, "r");
    if (fp1 == NULL) {
      printf("Error: File not found.");
      exit(0);
    }

    fp2 = fopen(dest, "w");
    if (fp2 == NULL) {
      printf("Error: File not found.");
      fclose(fp1);
      exit(0);
    }
    while( (a = fgetc(fp1)) != EOF )
    {
      fputc(a, fp2);
    }

    fclose(fp1);
    fclose(fp2);
}

/* Receive binary number, convert it into char and write to file */
void receiver(char* input, bool isMsb)  {

    FILE *fp;
    int i;
    char c;
    char new[1024] = {0};
    char output[1024] = {0};

    char *reversed = NULL;
    char *ptr = NULL;

    if (isMsb == true){
        //Reverse every 8 bits (1 byte)
        reversed = reverse_bytes(input);
        ptr = reversed;
    }else{
        ptr = input;
    }

    // Converting binary back to char
    for (i = 0; i < str_len(ptr) + 16; i++, ptr += 8){
        copy_string(new, ptr, 8);
        c = strtol(new, 0, 2);
        stringcat(output, c);
    }
    
    fp = fopen("../data/temp.txt", "a");
    fputs(output, fp);
    fclose(fp);
}
