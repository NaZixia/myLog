#include "Logging.h"
#include <sys/time.h>

using namespace std;



std::shared_ptr<AsyncLogging> Logger::_asyncLogging; //用于写日志的静态类

string Logger::_logFileName;                        //

void output(const std::string &msg)
{
    Logger::getAsyncLogging()->append(msg);
}

Logger::Logger(const std::string filename, int line):
_impl(filename,line)
{
    setLogFileName(filename);
}

Logger::~Logger()
{
    _impl._stream << " -- " << _impl._fileName << ':' << _impl._line << '\n';
    const string  &buf=_impl._stream.buffer().data();
    output(buf);
}


Logger::Impl::Impl(const std::string filename, int line)
  : _stream(),
    _line(line),
    _fileName(filename)
{
    formatTime();
}

//获取当前按时间并写入缓冲
void Logger::Impl::formatTime()
{
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);   
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    _stream << str_t;
}