#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <time.h>
#include <errno.h>

// Adjust this path as needed
#define RTC_DEVICE "/dev/rtc0"

//Print time current
void print_time(struct rtc_time *rtc_tm) {
    printf("Current RTC time: %04d-%02d-%02d %02d:%02d:%02d\n",
           rtc_tm->tm_year + 1900, rtc_tm->tm_mon + 1, rtc_tm->tm_mday,
           rtc_tm->tm_hour, rtc_tm->tm_min, rtc_tm->tm_sec);
}

//Get time rtc
int get_rtc_time(int fd) {
    struct rtc_time rtc_tm;
    if (ioctl(fd, RTC_RD_TIME, &rtc_tm) == -1) {
        perror("RTC_RD_TIME ioctl failed");
        return -1;
    }

    print_time(&rtc_tm);
    return 0;
}

//Set time rtc
int set_rtc_time(int fd) {
    struct tm *tm_info;
    time_t t = time(NULL);
    struct rtc_time rtc_tm;

    tm_info = localtime(&t);

    rtc_tm.tm_year = tm_info->tm_year;
    rtc_tm.tm_mon = tm_info->tm_mon;
    rtc_tm.tm_mday = tm_info->tm_mday;
    rtc_tm.tm_hour = tm_info->tm_hour;
    rtc_tm.tm_min = tm_info->tm_min;
    rtc_tm.tm_sec = tm_info->tm_sec;

    if (ioctl(fd, RTC_SET_TIME, &rtc_tm) == -1) {
        perror("RTC_SET_TIME ioctl failed");
        return -1;
    }

    printf("RTC time set successfully.\n");
    return 0;
}

int main(int argc, char *argv[]) {
    int fd = open(RTC_DEVICE, O_RDWR);
    if (fd == -1) {
        perror("Unable to open RTC device");
        return -1;
    }

    if (argc != 2) {
        fprintf(stderr, "Usage: %s [get|set]\n", argv[0]);
        close(fd);
        return -1;
    }

    if (strcmp(argv[1], "get") == 0) {
        if (get_rtc_time(fd) == -1) {
            close(fd);
            return -1;
        }
    } else if (strcmp(argv[1], "set") == 0) {
        if (set_rtc_time(fd) == -1) {
            close(fd);
            return -1;
        }
    } else {
        fprintf(stderr, "Invalid argument: %s\n", argv[1]);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}