#include <stdio.h>
#include <string.h>

struct s_msg {
    char text[140];
    char username[40];
    int len;
} t_msg;

void secret_backdoor() {
    char buffer[128];

    fgets(buffer, 128, stdin);
    system(buffer);
}

void set_msg(char *msg) {
    char buffer[1024];
    
    memset(buffer, 0, 128);

    puts(">: Msg @Unix-Dude");
    printf(">>: ");

    fgets(local_buffer, 0x400, stdin);
    strncpy(msg->text, local_buffer, strlen(msg->len));
}

void set_username(char *msg) {
    char buffer[128];
    int index = 0;

    for (int i = 0; i < 16; i++) {
        buffer[i] = 0;
    }

    puts(">: Enter your username");

    fgets(buffer, 128, stdin);

    while (index < 41) {
        char ch = buffer[index];
        if (ch == '\0') {
            break;
        }
        username[index] = ch;
        index++;
    }

    printf(">: Welcome, %s", msg->username);
}

void handle_msg() {
    t_msg msg;

    message.len = 0x8c;
    set_username(&msg);
    set_msg(&msg);

    puts(">: Msg sent!");
}

int main() {
    puts("--------------------------------------------\n|   ~Welcome to l33t-m$n ~    v1337        |\n--------------------------------------------");
    handle_msg();
    return 0;
}

