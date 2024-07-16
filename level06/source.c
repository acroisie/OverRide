#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>

#define MAX_INPUT_LENGTH 32

int auth(char *username, int serial) {
    int username_len;
    int i;
    unsigned int hash;

    // Remove newline character from username
    username[strcspn(username, "\n")] = '\0';

    // Get username length
    username_len = strnlen(username, MAX_INPUT_LENGTH);

    // Check if username is too short
    if (username_len <= 5) {
        return 1;
    }

    // Check for debugger
	if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) {
		puts("\033[32m.---------------------------.");
		puts("\033[31m| !! TAMPERING DETECTED !!  |");
		puts("\033[32m'---------------------------'");
		return 1;
	}

    // Calculate hash
    hash = ((username[3] & 0xff) ^ 0x1337) + 0x5eeded;

    // Process username
    for (i = 0; i < username_len; i++) {
        if (username[i] <= 31) {
            return 1;
        }
        hash += ((hash ^ (username[i] & 0xff)) % 1337);
    }

    // Compare hash with serial
    return (hash != serial);
}

int main(int argc, char *argv[]) {
    char username[MAX_INPUT_LENGTH];
    int serial;

    puts("***********************************");
    puts("*\t\tlevel06\t\t  *");
    puts("***********************************");

    printf("-> Enter Login: ");
    if (fgets(username, MAX_INPUT_LENGTH, stdin) == NULL) {
        return 1;
    }

    puts("***********************************");
    puts("***** NEW ACCOUNT DETECTED ********");
    puts("***********************************");

    printf("-> Enter Serial: ");
    if (scanf("%d", &serial) != 1) {
        return 1;
    }

    if (auth(username, serial) == 0) {
        puts("Authenticated!");
        system("/bin/sh");
        return 0;
    }

    return 1;
}


