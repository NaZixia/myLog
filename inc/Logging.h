#pragma ocne
#include "LogStream.h"
#include "AsyncLogging.h"
#include <string>


// 使用时直接LOG<<"..."
#define LOG Logger(__FILE__, __LINE__).stream()
//用前先初始化 
//Logger::initAsyncLogging(文件名);

//Log的对外接口
//其他程序使用本类写日志
class Logger {
private:                      
    class Impl {
        public:
            Impl(const std::string filename, int line);
            ~Impl(){};
            void formatTime();

            LogStream _stream;
            int _line;
            std::string _fileName;
    };

    Impl _impl;
    static std::string _logFileName;    //文件名
    static std::shared_ptr<AsyncLogging> _asyncLogging; //用于写日志的静态类
public:
    Logger(const std::string filename, int line);
    ~Logger();

    LogStream &stream() { return _impl._stream; }

    inline static void setLogFileName(const std::string fileName) { _logFileName = fileName; }
    inline static std::string getLogFileName() { return _logFileName; }
    

    //静态 AsyncLogging初始化
    inline static void initAsyncLogging
    (const std::string fileName,const uint32_t flushInterval=2)
    {
        if(_asyncLogging==nullptr){
                std::unique_ptr<AsyncLogging> temp(new AsyncLogging(fileName,flushInterval));
                _asyncLogging=move(temp);
                _asyncLogging->start();
        }
        else{
            perror("LOG asyncLogging is init ,intit error");
        }
    }

    // 获取静态 AsyncLogging变量
    inline static std::shared_ptr<AsyncLogging> getAsyncLogging(){return _asyncLogging;}
};


void output(const std::string &msg);