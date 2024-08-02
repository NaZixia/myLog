#pragma once
#include "LogFile.h"
#include <thread>
#include <vector>
#include <condition_variable>
#include "LogStream.h"

class FixedBuffer;                      //LogStream.h中的缓冲


//日志核心 负责将缓冲区内容写到文件
//会新建一个工作线程进行日志内容写入
class AsyncLogging
{
private:
    std::string _filename;                 //文件名
    const uint32_t _flushInterval;         //刷新时间 每n秒将数据写入到文件

    std::mutex _mutex;                      //互斥锁
    std::condition_variable _cond;          //条件变量
    bool _running;                          //用于子线程启动/关闭
    std::thread _thread;                    //线程

    BufferPtr _curBuffer;                   //当前缓冲
    BufferPtr _nextBuffer;                  //下一个缓冲
    BufferVector _buffers;                  //用于保存简要写的数据

    LogFile _logfile;                       //封装好的写数据文件

public:
    AsyncLogging(const std::string filename,const uint32_t flushInterval=2);
    ~AsyncLogging(){
        if(_running)stop();
    }

    void start() {                  //开始工作
        std::lock_guard<std::mutex> lock(_mutex);
        _running = true;
        _cond.notify_all();
    }

    void stop() {                   //停止工作
        _running = false;
        _cond.notify_all();
        _thread.join();
    }

    void append(const std::string &logline);
    void append(const char* logline, int len);
private:
    static void threadFunc(AsyncLogging *Logging);           //工作线程,负责将buf数据写到文件
};

