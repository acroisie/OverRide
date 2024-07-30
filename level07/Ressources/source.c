#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define STORAGE_SIZE 100

void clear_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

unsigned int get_unum(void) {
    unsigned int num;
    fflush(stdout);
    scanf("%u", &num);
    clear_stdin();
    return num;
}

int store_number(unsigned int *data) {
    unsigned int number;
    unsigned int index;
    
    printf("Number: ");
    number = get_unum();
    printf("Index: ");
    index = get_unum();
    
    if ((index % 3 == 0) || (number >> 24 == 0xb7)) { // number >> 24 means the first byte of number and 0xb7 = 183
        puts("*** ERROR! ***");
        puts("This index is reserved for wil!");
        puts("*** ERROR! ***");
        return 1;
    }
    
    data[index] = number;
    return 0;
}

int read_number(unsigned int *data) {
    unsigned int index;
    
    printf("Index: ");
    index = get_unum();
    printf("Number at data[%u] is %u\n", index, data[index]);
    return 0;
}

int main(int argc, char *argv[], char *envp[]) {
    char cmd[20];
    unsigned int data[STORAGE_SIZE] = {0};
    
    while (1) {
        printf("Input command: ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0;  // Remove newline
        
        if (strncmp(cmd, "store", 5) == 0)
            store_number(data);
        else if (strncmp(cmd, "read", 4) == 0)
            read_number(data);
        else if (strncmp(cmd, "quit", 4) == 0)
            break;
        else
            printf("Invalid command.\n");
    }
    
    return 0;
}