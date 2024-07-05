#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100
#define FLAG_SIZE 41

int main(int argc, char *argv[]) {
    FILE *file;
    char buffer[BUFFER_SIZE] = {0};
    char flag[FLAG_SIZE] = {0};
    char input[BUFFER_SIZE] = {0};
    size_t bytes_read;

    file = fopen("flag.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open flag.txt\n");
        exit(1);
    }

    bytes_read = fread(flag, 1, FLAG_SIZE - 1, file);
    flag[strcspn(flag, "\n")] = 0;

    if (bytes_read != FLAG_SIZE - 1) {
        fprintf(stderr, "Error: Unable to read flag\n");
        fprintf(stderr, "Error: Unable to read flag\n");
        exit(1);
    }

    fclose(file);

    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");

    printf("--[ Username: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    printf("--[ Password: ");
    fgets(input, BUFFER_SIZE, stdin);
    input[strcspn(input, "\n")] = 0;

    puts("*****************************************");

    if (strncmp(input, flag, FLAG_SIZE - 1) == 0) {
		printf("Greetings, %s!\n", buffer);
		system("/bin/sh");
        return 0;
    } else {
        printf(buffer);
        puts(" does not have access!");
        exit(1);
    }
}

