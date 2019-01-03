#include "log/log_print.h"
#include "log/glog_helper.h"

#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>

#define XPOSTO(x)   "\033[" #x "D\033[" #x "C"

static GLogHelper *gh = NULL;

void log_init(const char *program)
{
    if (gh) return;

    gh = new GLogHelper(program);
}

void log_quit()
{
    if (gh) {
        delete gh;
        gh = NULL;
    }
}

int log_printf(const char *file, const char *func, int line, const int level, const char *format, ...)
{
    int result = 0;
    char buffer[128] = {0};
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (level == _GLOG_INFO)
        google::LogMessage(file, line).stream()
            << XPOSTO(60)
            << "<" << func << ">"
            << XPOSTO(90)
            << buffer;
    else if (level == _GLOG_WARN)
        google::LogMessage(file, line, google::GLOG_WARNING).stream()
            << XPOSTO(60)
            << "<" << func << ">"
            << XPOSTO(90)
            << buffer;
    else if (level == _GLOG_ERROR)
        google::LogMessage(file, line, google::GLOG_ERROR).stream()
            << XPOSTO(60)
            << "<" << func << ">"
            << XPOSTO(90)
            << buffer;
    else if (level == _GLOG_FATAL)
        google::LogMessage(file, line, google::GLOG_FATAL).stream()
            << XPOSTO(60)
            << "<" << func << ">"
            << XPOSTO(90)
            << buffer;
    else
        google::LogMessage(file, line).stream()
            << XPOSTO(60)
            << "<" << func << ">"
            << XPOSTO(90)
            << buffer;

    return result;
}
