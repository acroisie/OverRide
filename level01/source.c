#include <iostream>
#include <cstdio>
#include <cstring>

char a_user_name[0x100];

int verify_user_name() {
    return strncmp("dat_wil", a_user_name, 7);
}

int verify_user_pass() {
    return 1;
}

int main() {
    char buffer[0x40];
    memset(buffer, 0, sizeof(buffer));

    printf("Enter your username: ");
    fgets(a_user_name, 0x100, stdin);
    
    int user_name_verified = verify_user_name();
    if (user_name_verified == 0) {
        puts("Username verification failed.");
        return 1;
    }

    puts("Enter your password: ");
    printf("Enter your password: ");
    fgets(buffer, 0x64, stdin);
    
    int user_pass_verified = verify_user_pass();
    if (user_pass_verified == 0) {
        puts("Password verification failed.");
        return 1;
    }

    return 0;
}
