#include "AsyncLogging.h"
#include "LogStream.h"
#include <cassert>
using namespace std;

AsyncLogging::AsyncLogging(const std::string filename,const uint32_t flushInterval):
_filename(filename),
_flushInterval(flushInterval),
_mutex(),
_cond(),
_running(false),
_thread(threadFunc,this),       //线程需要在_mutex _running初始化后创建
_curBuffer(new Buffer()),
_nextBuffer(new Buffer()),
_buffers(),
_logfile(filename)
{
    assert(filename.size()>1);
    _buffers.reserve(16);
    _buffers.resize(0);
}



void AsyncLogging::append(const std::string &logline){             //向缓冲添加数据
    lock_guard<mutex> lock(_mutex);
    if(_curBuffer->avail()>logline.size()){               //缓冲容量够
        _curBuffer->append(logline);
    }
    else{                                                   //当前缓冲不够,将缓冲内容送到_buffers
        _buffers.push_back(_curBuffer);                     //重置缓冲后添加
        if(_nextBuffer){
            _curBuffer.reset();
            _curBuffer=move(_nextBuffer);
        }
        else
            _curBuffer.reset(new Buffer);

        _curBuffer->append(logline);
        _cond.notify_all();
    }

}
void AsyncLogging::append(const char* logline, int len){
    lock_guard<mutex> lock(_mutex);
    if(_curBuffer->avail()>len){               //缓冲容量够
        _curBuffer->append(logline);
    }
    else{                                                   //当前缓冲不够,将当前缓冲内容送到_buffers
        _buffers.push_back(_curBuffer);                     //重置缓冲后添加
        _curBuffer.reset();
        if(_nextBuffer){
            _curBuffer.reset();
            _curBuffer=move(_nextBuffer);
        }
        else
            _curBuffer.reset(new Buffer);
            
        _curBuffer->append(logline,len);
        _cond.notify_all();
    }
}


void AsyncLogging::threadFunc(AsyncLogging *Logging){          //工作线程
    std::unique_lock<mutex> temp(Logging->_mutex);
    while(!Logging->_running){
        Logging->_cond.wait(temp);
    }
    temp.unlock();

    BufferPtr newBuffer1(new Buffer);                       //用于转移数据后写入文件
    BufferPtr newBuffer2(new Buffer);
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);

    printf("work thread start work\n");
    while(Logging->_running){
        std::unique_lock<mutex> lock(Logging->_mutex);
        assert(newBuffer1 && newBuffer1->length() == 0);    //每次开始写,这些都应该是空的
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        //等到vec buf有数据或者超时
        Logging->_cond.wait_for(lock, std::chrono::seconds(Logging->_flushInterval),  
            [&] { return !Logging->_buffers.empty() &&!Logging->_running; }); 

        if(!Logging->_running)
            break;


        Logging->_buffers.push_back(Logging->_curBuffer);           //将缓冲内容添加到vec buf
        Logging->_curBuffer.reset();
        Logging->_curBuffer = move(newBuffer1);


        buffersToWrite.swap(Logging->_buffers);                             //交换数据并写入
        if (!Logging->_nextBuffer) {
            Logging->_nextBuffer = std::move(newBuffer2);
        }

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25) {                               //数据太多
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for (size_t i = 0; i < buffersToWrite.size(); ++i) {
        // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            Logging->_logfile.append(move(buffersToWrite[i]->data()));
        }

        if (buffersToWrite.size() > 2) {
        // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        //重置newBuffer1
        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        Logging->_logfile.flush();
    }
    Logging->_logfile.flush();
}