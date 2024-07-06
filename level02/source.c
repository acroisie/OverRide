#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 100
#define FLAG_SIZE 42

int main(int argc, char *argv[]) {
    FILE *file;
    char flag[FLAG_SIZE] = {0};
    char username[BUFFER_SIZE] = {0};
    char password[BUFFER_SIZE] = {0};
    size_t bytes_read;

    // Open and read the flag file
    file = fopen("/home/users/level03/.pass", "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open /home/users/level03/.pass\n");
        exit(1);
    }
    
    bytes_read = fread(flag, 1, FLAG_SIZE - 1, file);
    if (bytes_read != FLAG_SIZE - 1) {
        fprintf(stderr, "Error: Unable to read flag\n");
        exit(1);
    }
    fclose(file);

    // Print the login prompt
    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");

    // Get username input
    printf("--[ Username: ");
    fgets(username, BUFFER_SIZE, stdin);
    username[strcspn(username, "\n")] = 0;

    // Get password input
    printf("--[ Password: ");
    fgets(password, BUFFER_SIZE, stdin);
    password[strcspn(password, "\n")] = 0;

    puts("*****************************************");

    // Vulnerable printf call
    printf(username);
    puts(" does not have access!");

    return 0;
}