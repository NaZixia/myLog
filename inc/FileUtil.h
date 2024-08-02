#pragma once
#include <cstdio>
#include <cstdlib>
#include <string>

//log的最底层,写入数据到文件的封装

class FileUtil
{
private:
    static const int BUFFSIZE=64*1024;                      //缓冲大小,用于初始化
private:
    FILE *_file;
    char _buffer[BUFFSIZE];                                 //缓冲区
    size_t write(const char *logline, size_t len);          //向文件写入数据
public:
    FileUtil();
    explicit FileUtil(const char *filename);
    ~FileUtil();
    void openFile(const std::string& file);

    void append(const char *logline, const size_t len);     //缓冲添加数据
    void flush();                                           //刷新缓冲数据到文件
};

inline FileUtil::FileUtil():
_file(nullptr)
{

}

inline FileUtil::FileUtil(const char *filename):
_file(fopen(filename, "a"))                                     //追加方式打开
{
    setbuffer(_file, _buffer,BUFFSIZE);                         //手动设置_file缓冲区
}

inline FileUtil::~FileUtil()
{
    fclose(_file);
}

inline void FileUtil::openFile(const std::string& file){
    _file=fopen(file.c_str(), "a");
    if(_file==nullptr){
        perror("open error");
        exit(1);
    }
    setbuffer(_file, _buffer,BUFFSIZE);
}

inline size_t FileUtil::write(const char* logline, size_t len) {           //向文件写入数据
    return fwrite_unlocked(logline, 1, len, _file);                        //线程不安全的写入
}

inline void FileUtil::append(const char *logline, const size_t len){              //缓冲添加数据
    size_t remain=len;              //未写入数据的长度
    size_t n=0;                     //未写入数据的索引位置
    while(remain>0){
        size_t x = this->write(logline+n, remain);
        if (x == 0) {
            int err = ferror(_file);
            if (err) fprintf(stderr, "FileUtil::append() failed !\n");
            break;
        }
        remain-=x;
        n+=x;
    }
}             


inline void FileUtil::flush(){                                                    //刷新缓冲数据到文件
    fflush(_file);
}                                                  

