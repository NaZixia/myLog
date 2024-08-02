#pragma once
#include <cstdint>
#include <string>
#include <memory> 
#include <mutex>
#include "FileUtil.h"

//log的底2层,封装了FileUtil
class LogFile                   
{
private:
    std::string _filename;               //文件名

    const uint32_t _flushEveryN;         //刷新时次数
    uint32_t _count;                     //记录写入次数
    std::unique_ptr<std::mutex> _mutex;  //互斥锁
    std::unique_ptr<FileUtil>   _file;   //用于写入数据到文件的类
public:
    LogFile(const std::string &path,const uint32_t flushEveryN=1024);
    ~LogFile();
    
    void append(const std::string&& logline);                //添加数据,有锁
    void append(const char* logline, int len);              //添加数据,有锁
    void flush();                                           //刷新缓存
private:
    void append_unlocked(const char* logline, int len);         //添加数据,无锁接口
    void prise_FileName(const std::string &filename);               //解析文件名,路径名
};



inline LogFile::LogFile(const std::string &filename,const uint32_t flushEveryN):
_filename(filename),
_flushEveryN(flushEveryN),
_count(0),
_mutex(new std::mutex()),
_file(new FileUtil(filename.c_str()))
{
    
}


inline LogFile::~LogFile()
{

}


inline void LogFile::append_unlocked(const char* logline, int len){
    _file->append(logline,len);
    ++_count;
    if(_count>=_flushEveryN){
        _file->flush();
        _count=0;
    }
}


inline void LogFile::append(const char* logline, int len){
    std::lock_guard<std::mutex> templock(*_mutex);
    this->append_unlocked(logline,len);
}

inline void LogFile::append(const std::string&& logline){
    std::lock_guard<std::mutex> templock(*_mutex);
    this->append_unlocked(logline.c_str(),logline.size());
}


inline void LogFile::flush(){
    std::lock_guard<std::mutex> templock(*_mutex);
    _file->flush();
}
