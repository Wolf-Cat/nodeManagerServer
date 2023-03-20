#ifndef SERVER_DISCOVER_MAINLOG_H
#define SERVER_DISCOVER_MAINLOG_H

#include "log4z.h"

#define log_debug(format, ...) LOG_FORMAT(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define log_info(format, ...) LOG_FORMAT(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define log_warning(format, ...) LOG_FORMAT(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define log_error(format, ...) LOG_FORMAT(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

class MainLog {
public:
    void init();
};


#endif //SERVER_DISCOVER_MAINLOG_H
