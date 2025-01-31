# myLog
多线程Log打印系统

本项目为仿照muduo库线程安全的Log
https://github.com/chenshuo/muduo


## 编译
    make -j8
    
## 运行自带测试
    ./myLog

## 使用
    1.包含头"Logging.h"

    2.使用函数初始化
        Logger::initAsyncLogging("testlog.log");  

    3.使用      
        LOG<<"log消息";
        



## 类说明
* 与Log相关的类包括FileUtil、LogFile、AsyncLogging、LogStream、Logging。
其中前4个类每一个类都含有一个append函数，Log的设计也是主要围绕这个append函数展开的。

* FileUtil是最底层的文件类，封装了Log文件的打开、写入并在类析构的时候关闭文件，底层使用了标准IO，该append函数直接向文件写。

* LogFile进一步封装了FileUtil，并设置了一个循环次数，没过这么多次就flush一次。

* AsyncLogging是核心，它负责启动一个log线程，专门用来将log写入LogFile，应用了“双缓冲技术”，其实有4个以上的缓冲区，但思想是一样的。
* AsyncLogging负责(定时到或被填满时)将缓冲区中的数据写入LogFile中。

* LogStream主要用来格式化输出，重载了<<运算符，同时也有自己的一块缓冲区，这里缓冲区的存在是为了缓存一行，把多个<<的结果连成一块。

* Logging是对外接口，Logging类内涵一个LogStream对象，主要是为了每次打log的时候在log之前和之后加上固定的格式化的信息，比如打log的行、文件名等信息。
