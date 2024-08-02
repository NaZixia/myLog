#include "LogStream.h"

#include <algorithm>
using namespace std;

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

template <typename T>
void convert(FixedBuffer &buf, T value) {
    T i = value;
    if (value < 0) {
        buf.append("-");
    }
    do {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        buf.append({zero[lsd]});
    } while (i != 0);
}

template <typename T>
void LogStream::formatInteger(T v) {
    // buffer容不下kMaxNumericSize个字符的话会被直接丢弃
    if (buffer_.avail() >= kMaxNumericSize) {
        convert(buffer_, v);
    }
}


LogStream::LogStream():
buffer_(kSmallBuffer)
{

}
LogStream::~LogStream(){

}


LogStream& LogStream::operator<<(short v) {
  *this << static_cast<int>(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
  *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream& LogStream::operator<<(int v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned int v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
  formatInteger(v);
  return *this;
}


LogStream& LogStream::operator<<(double v) {
  if (buffer_.avail() >= kMaxNumericSize) {
    buffer_.append(to_string(v));
  }
  return *this;
}

LogStream& LogStream::operator<<(long double v) {
  if (buffer_.avail() >= kMaxNumericSize) {
    buffer_.append(to_string(v));
  }
  return *this;
}