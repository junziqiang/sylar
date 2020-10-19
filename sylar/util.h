#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__
#include<pthread.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<cstdint>
namespace sylar{

    pid_t GetThreadId();    //线程ID
    uint32_t GetFiberId();


}

#endif