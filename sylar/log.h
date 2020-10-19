#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__
#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include<time.h>
#include "sigleton.h"
#include "util.h"

/********************************************************************************************************************************************
 * ******************************************************************************************************************************************/
//宏定义

#pragma region
#define SYLAR_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, sylar::GetThreadId(),\
                sylar::GetFiberId(), time(0)))).getSS()
/*#define SYLAR_LOG_LEVEL(logger,level) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::Logevent(logger,level,__FILE__,__LINE__,0,sylar::GetThreadId(),\
                sylar::GetFiberId(),time(0)))).getSS()*/

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger,sylar::LogLevel::FATAL)


#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, sylar::GetThreadId(),\
                sylar::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)


#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...)  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...)  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

#pragma endregion

namespace sylar{
class Logger;




/*************************************************************************************************************************************
 * *************************************************************************************************************************************/
//日志级别

#pragma region 日志级别
class LogLevel{
public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
    //转化为字符串
    static const char* ToString(LogLevel::Level Level);
};
#pragma endregion 日志级别

/*************************************************************************************************************************************
 * *************************************************************************************************************************************/
//日志事件

#pragma region
class LogEvent{
public:
    //智能指针，C++中用来管理内存的方式，share_ptr允许多个指针指向同一个对象，unique_ptr则独占对象
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,const char* file,int32_t m_line,uint32_t elapse,uint32_t thread_id,uint32_t fiber_id,uint64_t time);
    //~LogEvent();
    const char* getFile() const{return m_file;}
    int32_t getLine() const{return m_line;}
    uint32_t getElapse() const {return m_elapse;}
    uint32_t getThreadId() const{return m_threadId;}
    uint32_t getFiberId() const {return m_fiberId;}         
    uint64_t getTime() const {return m_time;}                
    std::string getContent() const {return m_ss.str();} 
    std::shared_ptr<Logger> getLogger() const {return m_logger;}

    LogLevel::Level getLevel() const {return m_level;}

    std::stringstream& getSS() {return m_ss;}

    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
private:
    const char* m_file = nullptr;   //文件名
    int32_t m_line = 0;             //行号
    uint32_t m_elapse = 0;          //程序启动到现在的毫秒数
    uint32_t m_threadId = 0;        //线程号
    uint32_t m_fiberId = 0;         //协程号
    uint64_t m_time;                //时间
    std::stringstream m_ss;          //消息

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
};


class LogEventWrap{
public:
    LogEventWrap(LogEvent::ptr e);
    //在析构的时候打印消息
    ~LogEventWrap();
    LogEvent::ptr getEvent() const{return m_event;}
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;
};

#pragma endregion

/****************************************************************************************************************************************
 * *************************************************************************************************************************************/
//日志格式器

#pragma region

class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string &pattern);
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event);
    //解析日志格式
    class FormatItem{
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            //FormatItem(const std::string& fmt = ""){};
            virtual ~FormatItem(){}
            virtual void format( std::ostream &os,std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) = 0; //直接输出到流中
    };
    void init();
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};
#pragma endregion


/********************************************************************************************************************************************
 * ******************************************************************************************************************************************/

//日志输出的地方
#pragma region
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){}
    //纯虚函数是在声明虚函数时被“初始化”为0的函数。声明纯虚函数的一般形式是 virtual 函数类型 函数名 (参数表列) =0;
    /*
    注意: ①纯虚函数没有函数体；

          ②最后面的“=0”并不表示函数返回值为0，它只起形式上的作用，告诉编译系统“这是纯虚函数”;

          ③这是一个声明语句，最后应有分号。

纯虚函数只有函数的名字而不具备函数的功能，不能被调用。它只是通知编译系统: “在这里声明一个虚函数，留待派生类中定义”。在派生类中对此函数提供定义后，它才能具备函数的功能，可被调用。
    */
   //子类必须实现纯虚函数
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    void setFormatter(LogFormatter::ptr val){
        m_formatter = val;
    }
    LogFormatter::ptr getFormatter() const{
        return m_formatter;
    }
    LogLevel::Level getLevel() const{return m_level;}
    void setLevel(LogLevel::Level val){m_level = val;}
//子类可能要用到level
protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    LogFormatter::ptr m_formatter;
};
#pragma endregion


/*****************************************************************************************************************************************
 * ***************************************************************************************************************************************/
//日志器
#pragma region
class Logger : public std::enable_shared_from_this<Logger>{
public:
    typedef std::shared_ptr<Logger> ptr;
    //日志
    Logger(const std::string &name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const {
        return m_level;
    }
    void setLevel(LogLevel::Level val){
        m_level = val;
    }
    const std::string getName() const{return m_name;}
private:
    
    std::string m_name;         //名字
    LogLevel::Level m_level;
    std::list<LogAppender::ptr> m_appender;     //appender列表
    LogFormatter::ptr m_formatter;
};
#pragma endregion

/*******************************************************************************************************************************************
 * ****************************************************************************************************************************************/
//输出到控制台的Appender
#pragma region

class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    //override 说明该纯虚函数被实现了
    virtual void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

};
#pragma endregion


/*******************************************************************************************************************************************
 * ****************************************************************************************************************************************/
//输出到文件的Appender
#pragma region
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    //输出到文件的日志输出器需要一个文件名
    FileLogAppender(const std::string &filename);
    virtual void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
    
    bool reopen();      //打开文件
private:
    std::string m_filename;
    std::ofstream m_filestream;

};
#pragma endregion


/*********************************************************************************************************************************************
 * ******************************************************************************************************************************************/
//日志格式解析
#pragma region
class MessageFormatItem : public LogFormatter::FormatItem{
public:
    MessageFormatItem(const std::string& str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override{
        os<<event->getContent();
    } //直接输出到流中
};

class LevelFormatItem:public LogFormatter::FormatItem{
public:
    LevelFormatItem(const std::string& str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        os<<LogLevel::ToString(level);
    } //直接输出到流中
};

class ElapseFormatItem:public LogFormatter::FormatItem{
public:
    ElapseFormatItem(const std::string& str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        os<<event->getElapse();
    } //直接输出到流中
};
class NameFormatItem:public LogFormatter::FormatItem{
public:
    NameFormatItem(const std::string& str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        os<<logger->getName();
    } //直接输出到流中
};

class ThreadIdFormatItem:public LogFormatter::FormatItem{
public:
    ThreadIdFormatItem(const std::string& str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        os<<event->getThreadId();
    } //直接输出到流中
};

class FiberIdFormatItem:public LogFormatter::FormatItem{
public:
    FiberIdFormatItem(const std::string& str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        os<<event->getFiberId();
    } //直接输出到流中
};
class DateTimeFormatItem:public LogFormatter::FormatItem{
public:
    //DateTimeFormatItem(const std::string& str = ""){}
    DateTimeFormatItem(const std::string format = "%Y-%m-%d %H:%M:%S")
        :m_format(format){
            if(m_format.empty()){
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time,&tm);
        char buf[64];
        strftime(buf,sizeof(buf),m_format.c_str(),&tm);
        os<<buf;
    } //直接输出到流中
private:
    std::string m_format;
};


class FilenameFormatItem:public LogFormatter::FormatItem{
public:
    FilenameFormatItem(const std::string& str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        os<<event->getFile();
    } //直接输出到流中
};

class LineFormatItem:public LogFormatter::FormatItem{
public:
    LineFormatItem(const std::string& str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        os<<event->getLine();
    } //直接输出到流中
};
class NewLineFormatItem:public LogFormatter::FormatItem{
public:
    NewLineFormatItem(const std::string& str = ""){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        os<<std::endl;
    } //直接输出到流中
};
class StringFormatItem:public LogFormatter::FormatItem{
public:
    StringFormatItem(const std::string& str)
        :m_string(str){}
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        os<<m_string;
    } //直接输出到流中
private:
    std::string m_string;
};

class TabFormatItem:public LogFormatter::FormatItem{
public:
    TabFormatItem(const std::string& str = ""){} 
    void format(std::ostream &os, std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override{
        os<<"\t";
    } //直接输出到流中
};
#pragma endregion

//日志管理器
/******************************************************************************************************************************************
 * ******************************************************************************************************************************************/
#pragma region
class LoggerManager{
public:
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);

    void init();

    Logger::ptr getRoot() const {return m_root;}
private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;

};

typedef sylar::Singleton<LoggerManager> LoggerMgr;
#pragma endregion

}



#endif
