#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__
#include<memory>
#include<ucontext.h>
#include<functional>
#include "thread.h"
namespace sylar{
//需要返回this指针时，需要继承enable_shared_from_this
class Fiber : public std::enable_shared_from_this<Fiber>{
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State{
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };
private:
    Fiber();
public:
//functional解决了很多函数指针不适用的地方
    Fiber(std::function<void()> cb, size_t stacksize = 0);
    ~Fiber();
    //重置协程函数
    void reset(std::function<void()> cb);
    //切换到当前协程执行
    void swapIn();
    //切换到后台执行
    void swapOut();
    uint64_t GetId()const{return m_id;};
public:
    static void SetThis(Fiber* f);
    static Fiber::ptr GetThis();
    //协程切换到后台，并且设置成ready状态
    static void YieldToReady();
    //协程切换到后台，并且设置成hold状态
    static void YieldToHold();
    //总协程数
    static uint64_t TotalFibers();
    static void MainFunc();
    static uint64_t GetFiberId();
private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;
    ucontext_t m_ctx;
    void* m_stack = nullptr;
    std::function<void()> m_cb;
};

}


#endif