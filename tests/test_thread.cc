#include "sylar/sylar.h"
sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
sylar::Mutex s_mutex;
int count = 0;
void fun1(){
    //sylar::RWMutex::WriteLock lock(s_mutex);
    sylar::Mutex::Lock lock(s_mutex);
    SYLAR_LOG_INFO(g_logger)<< "thread_name" <<sylar::Thread::GetName()
                            << " this.name:" << sylar::Thread::GetThis()->GetName()
                            <<" id: "<< sylar::GetThreadId()
                            <<"this.id: " <<sylar::Thread::GetThis()->getId();
    for(int i = 0; i < 100000; ++i){
        
        ++count;
    }
}
void fun2(){
    while(true){
        SYLAR_LOG_INFO(g_logger) << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    }
}
void fun3(){
    while (true){
        SYLAR_LOG_INFO(g_logger) << "==============================";
    }
}
int main(){
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/root/syalr/bin/conf/log2.yml");
    sylar::Config::LoadFromYaml(root);
    std::vector<sylar::Thread::ptr> thrs;
    for(int i = 0;i < 2; ++i){
        sylar::Thread::ptr thr1(new sylar::Thread(&fun2,"name_" + std::to_string(i * 2)));
         sylar::Thread::ptr thr2(new sylar::Thread(&fun3,"name_" + std::to_string(i * 2+1)));
        thrs.push_back(thr1);
        thrs.push_back(thr2);
    }
    
    for(size_t i = 0; i < thrs.size(); ++i){
        thrs[i]->join();
    }
    SYLAR_LOG_INFO(g_logger) << "thread test end"<<std::endl;
    SYLAR_LOG_INFO(g_logger) << "count = "<<count;
    return 0;
}
