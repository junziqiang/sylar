#include<iostream>
#include "../sylar/log.h"
#include "../sylar/util.h"
int main(int argc,char** argv){
    //std::cout<<"hello world"<<std::endl;
    sylar::Logger::ptr logger(new sylar::Logger);
    //std::cout << logger<<std::endl;
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));
    sylar::LogEvent::ptr event(new sylar::LogEvent(logger,sylar::LogLevel::DEBUG,__FILE__, __LINE__, 0, sylar::GetThreadId(), 2, time(0),sylar::Thread::GetName()));

    event->getSS()<<"get ss"<<std::endl;


    logger->log(sylar::LogLevel::ERROR,event);
    std::cout<<"hello"<<std::endl;
    
    //SYLAR_LOG_FMT_ERROR(logger,"test macor error %s","aa");

    //auto l = sylar::LoggerMgr::GetInstance()->getLogger("xx");
    //SYLAR_LOG_INFO(l) << "xxxxx";
    return 0;
}