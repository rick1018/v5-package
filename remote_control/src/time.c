#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int time_number (void)
{
    time_t t1 = time(NULL);
    struct tm *ptr = localtime(&t1);
    //int year = ptr->tm_year + 1900;
    //int month = ptr->tm_mon + 1;
    int day = (ptr->tm_mday*24*60);
    int hour = (ptr->tm_hour*60);
    int min = ptr->tm_min;
    return (day + hour + min);
}

void getNowTime (char *val)
{
    time_t t1 = time(NULL);
    struct tm *ptr = localtime(&t1);
    int year = ptr->tm_year + 1900;
    int month = ptr->tm_mon + 1;
    int day = ptr->tm_mday;
    int hour = ptr->tm_hour;
    int min = ptr->tm_min;
    int sec = ptr->tm_sec;
    sprintf(val,"%d%02d%02d:%02d%02d%02d", year, month, day, hour, min, sec);
}

int getTimeBySec (void)
{
    time_t t1 = time(NULL);
    return t1;
}
