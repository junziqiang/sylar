#include "log.h"
#include<functional>
#include<stdarg.h>
namespace sylar{


/*****************************************************************************************************************************************
******************************************************************************************************************************************/
//LogEvent
#pragma region
LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* file,int32_t m_line,uint32_t elapse,uint32_t thread_id,uint32_t fiber_id,uint64_t time)
    :m_logger(logger),m_level(level), m_file(file),m_line(m_line),m_elapse(elapse),m_threadId(thread_id),m_fiberId(fiber_id),m_time(time){}


const char* LogLevel::ToString(LogLevel::Level level){
    switch(level){
#define XX(name) \
        case LogLevel::name: \
            return #name; \
            break;
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
#undef XX
        default:
            return "UNKNOW";
    }
    return "UNKNOW";
}

// 当函数的入参个数不确定时，使用va_list函数进行动态处理，增加编程的灵活性。
void LogEvent::format(const char* fmt, ...){
    va_list al;
    va_start(al,fmt);
    format(fmt,al);
    va_end(al);
}
void LogEvent::format(const char* fmt,va_list al){
    char* buf = nullptr;
    int len = vasprintf(&buf,fmt,al);
    if(len !=-1){
        m_ss <<std::string(buf,len);
        free(buf);
    }

}

#pragma endregion

/*****************************************************************************************************************************************
******************************************************************************************************************************************/
//LogWrap
#pragma region
LogEventWrap::LogEventWrap(LogEvent::ptr e)
    :m_event(e) {
}
LogEventWrap::~LogEventWrap(){
    m_event->getLogger()->log(m_event->getLevel(),m_event);
}
std::stringstream& LogEventWrap::getSS(){
    return m_event->getSS();
}

#pragma endregion


/*****************************************************************************************************************************************
******************************************************************************************************************************************/
//Logger
#pragma region
    Logger::Logger(const std::string &name)
    :m_name(name),m_level(LogLevel::DEBUG){
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }
    //添加日志输出地
    void Logger::addAppender(LogAppender::ptr appender){
        if(!appender->getFormatter()){
            appender->setFormatter(m_formatter);
        }
         m_appender.push_back(appender);
     }
    void Logger::delAppender(LogAppender::ptr appender){
        //会使后续迭代器失效
        for(auto it = m_appender.begin();it != m_appender.end(); ++it){
            if(*it == appender){
                m_appender.erase(it);
                break;
            }
        }
    }
    void Logger::log( LogLevel::Level level, LogEvent::ptr event){
        if (level >= m_level)
        {
            auto self = shared_from_this();
            for (auto& i : m_appender)
            {
                i->log(self,level,event);
            }
        }
        
    }

    void Logger::debug(LogEvent::ptr event){
        log(LogLevel::DEBUG,event);
    }
    void Logger::info(LogEvent::ptr event){
        log(LogLevel::INFO,event);
    }
    void Logger::warn(LogEvent::ptr event){
        log(LogLevel::WARN,event);
    }
    void Logger::error(LogEvent::ptr event){
        log(LogLevel::ERROR,event);
    }
    void Logger::fatal(LogEvent::ptr event){
        log(LogLevel::FATAL,event);
    }

#pragma endregion
/*******************************************************************************************************************************************
 * ****************************************************************************************************************************************/
//StdoutLogeAppender
#pragma region
    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
        if (level>=m_level)
        {
           
            std::cout << m_formatter->format(logger,level,event);
            /* code */
        }
        
    }

#pragma endregion
/********************************************************************************************************************************************
 * *****************************************************************************************************************************************/
//FileLogAppender
#pragma region
    bool FileLogAppender::reopen(){
        if (m_filestream)
        {
            m_filestream.close();
            /* code */
        }
        m_filestream.open(m_filename);
        return !!m_filestream;//两个取非才能表示正确打开文件
        
    }
    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
        if(level >=m_level){
            m_filestream<< m_formatter->format(logger,level,event);
        }
    }
    //构造函数初始化
    FileLogAppender::FileLogAppender(const std::string &filename)
        :m_filename(filename){

    }
#pragma endregion

/*******************************************************************************************************************************************
 * ****************************************************************************************************************************************/
//LogFormatter
#pragma region
    LogFormatter::LogFormatter(const std::string &pattern)
        :m_pattern(pattern){
            init();
    }
    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event){
        std::stringstream ss;
        for (auto &i : m_items)
        {
            i->format(ss,logger,level,event);    //使用了多态
            /* code */
        }
        return ss.str();
        
    }
    //解析日志格式
    //一共有三种格式%xxx,%xxx{xxx},%%(输出%号)
void LogFormatter::init() {
    //str, format, type
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            //m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElapseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIdFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FilenameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        XX(T, TabFormatItem),               //T:Tab
        XX(F, FiberIdFormatItem),           //F:协程id
        //XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
               // m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }

        //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    //std::cout << m_items.size() << std::endl;
}
#pragma endregion

/*******************************************************************************************************************************************
 * ****************************************************************************************************************************************/
//LogManager

#pragma region
LoggerManager::LoggerManager(){
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
}
Logger::ptr LoggerManager::getLogger(const std::string& name){
    auto it = m_loggers.find(name);
    return it ==m_loggers.end() ? m_root: it->second;
}

void LoggerManager::init(){

}

#pragma endregion

}