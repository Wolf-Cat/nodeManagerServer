#include "utils.h"

namespace Utils {
    unsigned int GetCurTimeStamp()
    {
        time_t t = time(NULL);
        return t;
    }

    size_t getFileLength(std::ifstream &_f)
    {
        using namespace std;
        ifstream::pos_type old = _f.tellg();
        _f.seekg(0, ifstream::beg);
        ifstream::pos_type start = _f.tellg();
        _f.seekg(0, ifstream::end);
        ifstream::pos_type end = _f.tellg();
        _f.seekg(old);
        return end - start;
    }

    size_t readAll(std::ifstream &_ifs, char* _buff, int _buff_len)
    {
        std::filebuf* pbuf = _ifs.rdbuf();

        std::size_t size = pbuf->pubseekoff(0, _ifs.end, _ifs.in);
        pbuf->pubseekpos(0, _ifs.in);
        if (size > _buff_len)
        {
            return -1;
        }
        pbuf->sgetn(_buff, size);
        return size;
    }

    size_t readTextFile(const char* path, std::string& buf)
    {
        std::ifstream ifile(path);
        if (!ifile)
        {
            return 0;
        }
        size_t len = getFileLength(ifile);
        char* cbuf = (char*)malloc(len);
        int ret_len = readAll(ifile, cbuf, len);
        buf.append(cbuf, cbuf + ret_len);
        free(cbuf);
        return ret_len;
    }

    bool readJson(const char* filename, Json::Value& json)
    {
        Json::Reader reader(Json::Features::strictMode());
        std::string buf;
        size_t len = readTextFile(filename, buf);
        if (len <= 0)
        {
            return false;
        }
        //处理带BOM的UTF-8编码
        if ((0xef == (unsigned char)buf.at(0)) && (0xbb == (unsigned char)buf.at(1)) && (0xbf == (unsigned char)buf.at(2)))
        {
            buf = buf.substr(3, buf.size() - 3);
        }
        if (len <= 0)
        {
            return false;
        }
        else
        {
            if (reader.parse(buf.c_str(), json))
            {
                return true;
            }
        }
        return false;
    }

    bool parseJson(const std::string& str, Json::Value& json)
    {
        Json::Reader reader(Json::Features::strictMode());
        if (reader.parse(str.c_str(), json))
        {
            return true;
        }
        return false;
    }
}