#ifndef SERVER_DISCOVER_UTILS_H
#define SERVER_DISCOVER_UTILS_H
#include <iostream>
#include "../3rd/json/json.h"

namespace Utils {
    unsigned int GetCurTimeStamp();
    size_t getFileLength(std::ifstream &_f);
    size_t readAll(std::ifstream &_ifs, char* _buff, int _buff_len);
    size_t readTextFile(const char* path, std::string& buf);
    //bool readJson(const char* filename, Json::Value& json);
    //bool parseJson(const std::string& str, Json::Value& json);

};


#endif //SERVER_DISCOVER_UTILS_H
