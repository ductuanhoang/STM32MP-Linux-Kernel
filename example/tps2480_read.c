#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Adjust this path as needed
#define HWMON_PATH "/sys/class/hwmon/hwmon0"

// Function to read a sensor value from a given file
int read_sensor_value(const char *filename) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", HWMON_PATH, filename);
    
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open %s\n", path);
        return -1;
    }

    int value;
    if (fscanf(fp, "%d", &value) != 1) {
        fclose(fp);
        fprintf(stderr, "Failed to read value from %s\n", path);
        return -1;
    }
    
    fclose(fp);
    return value;
}

int main() {
    while (1) {
        int in0 = read_sensor_value("in0_input");
        int in1 = read_sensor_value("in1_input");
        int curr1 = read_sensor_value("curr1_input");
        int power1 = read_sensor_value("power1_input");

        if (in0 != -1) printf("Voltage Input 0: %d\n", in0);
        if (in1 != -1) printf("Voltage Input 1: %d\n", in1);
        if (curr1 != -1) printf("Current Input 1: %d\n", curr1);
        if (power1 != -1) printf("Power Input 1: %d\n", power1);

        sleep(1);
    }

    return 0;
}