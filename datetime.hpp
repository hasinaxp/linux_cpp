#pragma once
#include <string>
#include <ctime>

namespace sp {

    static double datetime() {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec + ts.tv_nsec * 1e-9;
    }

    static std::string ISODateString(time_t timestamp = time(NULL)) {
        char buffer[30];

        struct tm timeInfo; 
        gmtime_r(&timestamp, &timeInfo);

        strftime(buffer, 30, "%Y-%m-%dT%H:%M:%SZ", &timeInfo);

        return std::string(buffer);
    }


    /*
     * Returns a date string in the format YYYY-MM-DD
     * Format table
     *  %a - Abbreviated weekday name
     *  %A - Full weekday name
     *  %b - Abbreviated month name
     *  %B - Full month name
     *  %c - Date and time representation appropriate for locale
     *  %d - Day of month as decimal number (01 - 31)
     *  %H - Hour in 24-hour format (00 - 23)
     *  %I - Hour in 12-hour format (01 - 12)
     *  %j - Day of year as decimal number (001 - 366)
     *  %m - Month as decimal number (01 - 12)
     *  %M - Minute as decimal number (00 - 59)
     *  %p - Current locale's A.M./P.M. indicator for 12-hour clock
     *  %S - Second as decimal number (00 - 59)
     *  %U - Week of year as decimal number, with Sunday as first day of week (00 - 53)
     *  %w - Weekday as decimal number (0 - 6; Sunday is 0)
     * */
    static std::string datestr(time_t timestamp = time(NULL), const char* format = "%Y-%m-%d") {
        char buffer[16];

        struct tm timeInfo; 
        gmtime_r(&timestamp, &timeInfo); 
        strftime(buffer, 30, format, &timeInfo);

        return std::string(buffer);
    }

    static std::string timestr(time_t timestamp = time(NULL)) {
        char buffer[16];

        struct tm timeInfo; 
        gmtime_r(&timestamp, &timeInfo);

        strftime(buffer, 30, "%H:%M:%S", &timeInfo);

        return std::string(buffer);
    }

    static time_t ISODateToTimestamp(const char* isoDateString) {
        struct tm timeInfo;

        if (strptime(isoDateString, "%Y-%m-%dT%H:%M:%SZ", &timeInfo) == NULL) {
            return -1;
        }

        time_t timestamp = mktime(&timeInfo);
        return timestamp;
    }

    static time_t __LAST_TIMESTAMP = 0;

    static double timelap() {
        double currentTime = datetime();
        double timeSinceLastCall = currentTime - __LAST_TIMESTAMP;
        __LAST_TIMESTAMP = currentTime;
        return timeSinceLastCall;
    }

    static double timelap_s() {
        return timelap();
    }

    static double timelap_ms() {
        return timelap() * 1000;
    }

};


