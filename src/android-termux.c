#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 256

char layout[][64] = {
    "\x1b[0m\x1b[43m   .-.  \x1b[0m dis \x1b[1;33m| ",
    "\x1b[0m\x1b[43m   oo|  \x1b[0m ker \x1b[1;33m| ",
    "\x1b[0m\x1b[46m  /`'\\  \x1b[0m upt \x1b[1;36m| ",
    "\x1b[0m\x1b[46m (\\_;/) \x1b[0m mem \x1b[1;36m| "
};

int main() {
    struct utsname kernel;
    uname(&kernel);

    char output[1024] = {0};
    
    // dist
    int osrelease = open("/data/data/com.termux/files/usr/etc/os-release", O_RDONLY);
    if (osrelease != -1) {
        char osline[BUFFER_SIZE];
        ssize_t n;
        while ((n = read(osrelease, osline, sizeof(osline) - 1)) > 0) {
            osline[n] = '\0';
            char *line = strtok(osline, "\n");
            while (line != NULL) {
                if (strstr(line, "ID=") && !strstr(line, "\"")) {
                    char *distro = strtok(line, "=");
                    distro = strtok(NULL, "=");
                    strcat(output, "\n");
                    strcat(output, layout[0]);
                    strcat(output, distro);
                    break;
                }
                line = strtok(NULL, "\n");
            }
        }
        close(osrelease);
    }

    // kern
    strcat(output, "\n");
    strcat(output, layout[1]);
    strcat(output, kernel.release);
    strcat(output, "\n");

    // up
    strcat(output, layout[2]);
    int puptime = open("/proc/uptime", O_RDONLY);
    if (puptime != -1) {
        char uptime_buf[128];
        double uptime;
        read(puptime, uptime_buf, sizeof(uptime_buf));
        sscanf(uptime_buf, "%lf", &uptime);
        close(puptime);

        uint8_t days = (int)uptime / (60 * 60 * 24);
        uint8_t hours = ((int)uptime % (60 * 60 * 24)) / (60 * 60);
        uint8_t minutes = ((int)uptime % (60 * 60)) / 60;
        char uptime_str[64];
        if (days > 0) {
            snprintf(uptime_str, sizeof(uptime_str), "%dd ", days);
            strcat(output, uptime_str);
        }
        if (hours > 0) {
            snprintf(uptime_str, sizeof(uptime_str), "%dh ", hours);
            strcat(output, uptime_str);
        }
        snprintf(uptime_str, sizeof(uptime_str), "%dm\n", minutes);
        strcat(output, uptime_str);
    }

    // mem
    int meminfo = open("/proc/meminfo", O_RDONLY);
    if (meminfo != -1) {
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

        char mem_str[128];
        snprintf(mem_str, sizeof(mem_str), "%s%.2fGiB / %.2fGiB\n\n\x1b[0m", layout[3], 
                 (double)(total_memory - free_memory) / 1048576, 
                 (double)total_memory / 1048576);
        strcat(output, mem_str);
    }

    // Print output
    printf("%s", output);
    
    return 0;
}
