#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

void decrypt(int key, unsigned char *data, size_t data_len) {
    unsigned char encrypted_data[] = "Q}|u`sfg~sf{}|a3";
    char *correct_string = "Congratulations!";
    size_t len = strlen((char *)encrypted_data);

    for (size_t i = 0; i < len; i++) {
        encrypted_data[i] ^= (unsigned char)key;
    }

    if (strncmp((char *)encrypted_data, correct_string, len) == 0) {
        system("/bin/sh");
    } else {
        puts("\nInvalid Password");
    }
}

void test(int param_1, int param_2) {
    int key = param_2 - param_1;
    unsigned char dummy_data[] = {0};

    switch (key) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
            decrypt(key, dummy_data, sizeof(dummy_data));
            break;
        default:
            key = rand();
            decrypt(key, dummy_data, sizeof(dummy_data));
            break;
    }
}

int main(void) {
    int input;

    srand((unsigned int)time(NULL));

    puts("***********************************");
    puts("*\t\tlevel03\t\t**");
    puts("***********************************");
    printf("Password:");
    
    scanf("%d", &input);

    test(input, 0x1337d00d);

    return 0;
}