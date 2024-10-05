#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 256

void get_memory_info() {
    int meminfo = open("/proc/meminfo", O_RDONLY);
    if (meminfo == -1) {
        perror("Failed to open /proc/meminfo");
        return;
    }

    char line[BUFFER_SIZE];
    int total_memory = 0;
    int free_memory = 0;

    ssize_t n;
    while ((n = read(meminfo, line, sizeof(line) - 1)) > 0) {
        line[n] = '\0';
        char *line_ptr = strtok(line, "\n");
        while (line_ptr != NULL) {
            if (strncmp(line_ptr, "MemTotal:", 9) == 0) {
                sscanf(line_ptr + 9, "%d", &total_memory);
            } else if (strncmp(line_ptr, "MemAvailable:", 13) == 0) {
                sscanf(line_ptr + 13, "%d", &free_memory);
            }
            line_ptr = strtok(NULL, "\n");
        }
    }
    close(meminfo);

    // Display the memory information
    printf("Total Memory: %.2f GiB\n", total_memory / 1048576.0);
    printf("Available Memory: %.2f GiB\n", free_memory / 1048576.0);
    printf("Used Memory: %.2f GiB\n", (total_memory - free_memory) / 1048576.0);
}

int main() {
    get_memory_info();
    return 0;
}
