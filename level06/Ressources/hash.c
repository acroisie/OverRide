#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <username>\n", argv[0]);
        return 1;
    }

    char* username = argv[1];
    int username_len = strlen(username);
    unsigned int hash;

    if (username_len <= 5) {
        printf("Username must be longer than 5 characters\n");
        return 1;
    }

    // Calculate initial hash
    hash = ((username[3] & 0xff) ^ 0x1337) + 0x5eeded;

    // Process username
    for (int i = 0; i < username_len; i++) {
        if (username[i] <= 31) {
            printf("Invalid character in username\n");
            return 1;
        }
        hash += ((hash ^ (username[i] & 0xff)) % 1337);
    }

    printf("Username: %s\n", username);
    printf("Hash: %u\n", hash);
    return 0;
}