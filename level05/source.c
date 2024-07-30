#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char buffer[40];
    int index = 0;
    int length = 0xFFFFFFFF;

    fgets(buffer, 100, stdin);
    index = 0;

    while (index < 40) {
        char ch = buffer[index];
        if (ch <= 0x40) {
            break;
        }
        if (ch > 0x5A) {
            break;
        }
        buffer[index] ^= 0x20;
        index++;
    }

    length = strlen(buffer);
    printf(buffer);
    exit(0);
}

