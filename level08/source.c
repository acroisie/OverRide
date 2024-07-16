#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void log_wrapper(FILE *file, const char *format, const char *arg);

int main(int argc, char *argv[]) {
    char buffer[100];
    int fd = -1;
    FILE *log_file, *input_file;
    char c = 0xFF;

    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    log_file = fopen("./backups/.log", "w");
    if (log_file == NULL) {
        printf("ERROR: Failed to open %s\n", "./backups/.log");
        exit(1);
    }

    log_wrapper(log_file, "Starting back up: %s\n", argv[1]);

    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        printf("ERROR: Failed to open %s\n", argv[1]);
        exit(1);
    }

    strcpy(buffer, "./backups/");
    strncat(buffer, argv[1], 99 - strlen(buffer));

    fd = open(buffer, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (fd < 0) {
        printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
        exit(1);
    }

    while ((c = fgetc(input_file)) != EOF) {
        write(fd, &c, 1);
    }

    log_wrapper(log_file, "Finished back up %s\n", argv[1]);

    fclose(input_file);
    close(fd);

    return 0;
}

