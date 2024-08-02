#pragma once
#include <string>
#include <memory>
#include <cstring>
#include <vector>

const uint64_t kSmallBuffer = 4096;
const uint64_t kLargeBuffer = 4096 * 1000;
class FixedBuffer;

using Buffer=FixedBuffer;
using BufferPtr=std::shared_ptr<FixedBuffer>;  
using BufferVector=std::vector<BufferPtr>;

//缓冲区封装
class FixedBuffer{
private:
    size_t _capacity;                                   //容量
    std::unique_ptr<std::vector<char>> _data;          //数据缓冲区

public:

    //默认使用大容量缓冲
    FixedBuffer(size_t capacity=kLargeBuffer):
    _capacity(capacity),
    _data(new std::vector<char>())
    {
        _data->reserve(capacity);
        _data->resize(0);
    }

    ~FixedBuffer(){}

    //向缓冲区添加数据 string
    void append(const std::string &buf){
        if(buf.size()<avail()){
            _data->insert(_data->end(),buf.begin(),buf.end());
        }
    }

    //向缓冲区添加数据 char*
    void append(const char* buf,size_t len){
        if(len<avail()){
            _data->insert(_data->end(),buf,buf+len);
        }
    }

    //返回可用容量
    size_t avail() const { return _capacity-_data->size(); } 

    //返回缓冲区数据长度
    size_t length(){return _data->size();}                   

    //重置缓冲 
    void reset(){_data->clear();}                            

    //返回缓冲区最后的位置 无数据
    std::vector<char>::iterator current(){return _data->end();};

    //返回data数据 string类型
    const std::string data(){
        return {_data->begin(),_data->end()};
    }

};



//LogStream主要用来格式化输出，重载了<<运算符，
//同时也有自己的一块缓冲区，这里缓冲区的存在是
//为了缓存一行，把多个<<的结果连成一块
class LogStream
{
private:
    void staticCheck();
    template <typename T>
    void formatInteger(T);

    Buffer buffer_;                           //行缓冲
    static const int kMaxNumericSize = 32;    //行缓冲大小

public:
    LogStream();
    ~LogStream();

    LogStream& operator<<(bool v) {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }

    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);
    LogStream& operator<<(const void*);

    LogStream& operator<<(float v) {
        *this << static_cast<double>(v);
        return *this;
    }
    LogStream& operator<<(double);
    LogStream& operator<<(long double);

    LogStream& operator<<(char v) {
        buffer_.append(&v, 1);
        return *this;
    }

    LogStream& operator<<(const char* str) {
        if (str)
            buffer_.append(str, strlen(str));
        else
            buffer_.append("(null)", 6);
        return *this;
    }

    LogStream& operator<<(const unsigned char* str) {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    LogStream& operator<<(const std::string& v) {
        buffer_.append(v);
        return *this;
    }

    
    void append(const std::string& data) { buffer_.append(data); }
    void append(const char* data, int len) { buffer_.append(data, len); }

    //返回缓冲区
    Buffer& buffer() { return buffer_; }

    //重置缓冲内容为空
    void resetBuffer() { buffer_.reset(); }
};