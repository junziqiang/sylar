#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__
#pragma once
#include<pthread.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<cstdint>
#include<vector>
#include<string>
namespace sylar{

    pid_t GetThreadId();    //线程ID
    uint32_t GetFiberId();

void Backtrace(std::vector<std::string>& bt , int size,int skip = 1);
std::string BacktraceToString(int size,int skip = 2, const std::string& prefix = "");

}

#endif