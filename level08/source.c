#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

void log_wrapper(FILE *file, const char *format, const char *arg) {
    char buffer[256];
    
    strcpy(buffer, format);
    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), arg);
    
    buffer[strcspn(buffer, "\n")] = 0;  // Remove newline if present
    
    fprintf(file, "%s", buffer);
}

int main(int argc, char *argv[]) {
    char buffer[100];
    int fd = -1;
    FILE *log_file, *input_file;
    char c;

    if (argc != 2) {
        printf("Usage: %s filename\n", argv[0]);
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
    strcat(buffer, argv[1]);

    // Create directories if they don't exist
    char *dir = strdup(buffer);
    char *last_slash = strrchr(dir, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
        mkdir(dir, 0777);
    }
    free(dir);

    fd = open(buffer, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (fd < 0) {
        printf("ERROR: Failed to open %s\n", buffer);
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