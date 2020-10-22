#include "sylar/sylar.h"
#include <assert.h>
sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
void test_assert(){
    SYLAR_LOG_INFO(g_logger) <<sylar::BacktraceToString(10, 2, "    ");
    //assert(1==2);
    SYLAR_ASSERT2(false,"asd xx");
}

int main(){
    //assert(0);
    test_assert();
    return 0;
}