#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Need to change the correct SPI in hardware.
#define DEVICE_PATH "/sys/class/spi_master/spi0/spi0.0"

/*
 * Read value from channel ADC.
 * -1: error, value: Value ADC.
*/
int read_channel(int channel) 
{
    int value;
    char path[128];
    snprintf(path, sizeof(path), "%s/in_voltage%d_raw", DEVICE_PATH, channel);

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open %s\n", path);
        return -1;
    }

    fscanf(fp, "%d", &value);
    fclose(fp);

    // printf("Channel %d: %d\n", channel, value);

    return value;
}

int main() {
    int adc = 0;
    for (int channel = 0; channel < 8; ++channel) {
        adc = read_channel(channel);
        printf("Channel %d: %d\n", channel, adc);
        usleep(10000);//10ms
    }
    return 0;
}