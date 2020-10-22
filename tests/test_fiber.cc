#include "sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run_in_fiber(){
    SYLAR_LOG_INFO(g_logger)<<" run in fiber begin";
    sylar::Fiber::YieldToHold();
    SYLAR_LOG_INFO(g_logger)<<" run in fiber end";
}
int main(){
    SYLAR_LOG_INFO(g_logger) <<" main begin -1";
    {
        sylar::Fiber::GetThis();
        SYLAR_LOG_INFO(g_logger)<<" main begin";
        sylar::Fiber::ptr fiber(new sylar::Fiber(run_in_fiber));
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger)<<" main after swapIn";
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger)<<" main after end";
    }
    SYLAR_LOG_INFO(g_logger)<<" main after end2";
    return 0;
}
