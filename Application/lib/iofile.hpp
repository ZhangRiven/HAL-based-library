/*
 * @Author       : ZhangRiven
 * @Date         : 2023-06-25 21:30:06
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-07-30 13:59:30
 * @FilePath     : \HAL_F407ZGT6\Application\lib\iofile.hpp
 * @Description  : 
 */
#pragma once

namespace lib {

class IFile {
public:
    virtual int read_char() { return -1; }
    virtual int read_data(char *, int) { return -1; }
};

class OFile {
public:
    virtual void write_char(char) {}
    virtual void write_data(const char *data, int len) {
        while (len-- > 0) write_char(*data++);
    }
};

class IOFile : public IFile, public OFile {
};

}
