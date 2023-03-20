#include "mainLog.h"

void MainLog::init() {
    using namespace zsummer::log4z;

    std::string strName = "serd";
    std::string strPath = "log";

    ILog4zManager::getRef().enableLogger(LOG4Z_MAIN_LOGGER_ID, true);
    ILog4zManager::getRef().setLoggerFileLine(LOG4Z_MAIN_LOGGER_ID, true);
    ILog4zManager::getRef().setLoggerDisplay(LOG4Z_MAIN_LOGGER_ID, true);
    ILog4zManager::getRef().setLoggerOutFile(LOG4Z_MAIN_LOGGER_ID, true);
    ILog4zManager::getRef().setLoggerName(LOG4Z_MAIN_LOGGER_ID, strName.c_str());
    ILog4zManager::getRef().setLoggerPath(LOG4Z_MAIN_LOGGER_ID, strPath.c_str());
    ILog4zManager::getRef().setLoggerLevel(LOG4Z_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);

    ILog4zManager::getRef().start();
}
