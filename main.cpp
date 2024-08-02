#include "Logging.h"
#include <iostream>
#include <unistd.h>
#include <thread>
using namespace std;

void test01();
void test02();
void test03();
void test04();

int main(){
    // Logger::initAsyncLogging("./testlog.log");
    // 
    // for(int i=0;i<10000;++i)
    //     LOG<<s;

    //test01();
    //test02();
    //test03();
    test04();
    return 0;
}


void test01(){
    cout<<"test01"<<endl;
    LogFile logfile("testlog.log");
    for(int i=0;i<10;++i)
        logfile.append("test01\n");
        sleep(1);
}

void test02(){
    cout<<"test02"<<endl;
    AsyncLogging asyncLogging("testlog.log");
    asyncLogging.start();
    for(int i=0;i<10;++i){
        asyncLogging.append("test02\n");
        sleep(1);
    }
}

void test03(){
    cout<<"test03"<<endl;
    Logger::initAsyncLogging("testlog.log");

    for(int i=0;i<30;++i){
        LOG<<"ERROR to dddddddddddddddddddddddddddddddddd";
        sleep(1);
    }
}

void test041(){
    for(int i=0;i<30;++i){
        LOG<<"ERROR to dddddddddddddddddddddddddddddddddd";
        sleep(1);
    }
}

void test04(){
    cout<<"test04"<<endl;
    Logger::initAsyncLogging("testlog.log");
    

    for(int i=0;i<1000;++i){
        thread t(test041);
        t.detach();
    }
    sleep(30);
}

