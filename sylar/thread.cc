//#include "thread.h"
#include "log.h"
//#include "util.h"

namespace sylar {
//线程局部存储
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");


   Semaphore::Semaphore(uint32_t count){
       if(sem_init(&m_semaphore,0,count)){
           throw std::logic_error("sem_init error");
       }
   }
    Semaphore::~Semaphore(){
        sem_destroy(&m_semaphore);
    }

    void Semaphore::wait(){
            //它的作用是从信号量的值减去一个“1”，但它永远会先等待该信号量为一个非零值才开始做减法。也就是说，如果你对一个值为2的信号量调用sem_wait()，线程将会继续执行，将信号量的值将减到1。
        if(sem_wait(&m_semaphore)){
            throw std::logic_error("sem_wait error");
        }
    }
    void Semaphore::notify(){
        if(sem_post(&m_semaphore)){
            throw std::logic_error("sem_post error");
        }
    }

Thread* Thread::GetThis() {
    return t_thread;
}

const std::string& Thread::GetName() {
    return t_thread_name;
}

void Thread::SetName(const std::string& name) {
    if(name.empty()) {
        return;
    }
    if(t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name)
    :m_cb(cb), m_name(name) {
    if(name.empty()) {
        m_name = "UNKNOW";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(rt) {
        SYLAR_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt
            << " name=" << name;
        throw std::logic_error("pthread_create error");
    }
    //因为有可能返回整个构造函数后这个线程还没有开始执行，因此需要在此等待
    m_semaphore.wait();
}

Thread::~Thread() {
    if(m_thread) {
        //即主线程与子线程分离，子线程结束后，资源自动回收
        pthread_detach(m_thread);
    }
}

void Thread::join() {
    if(m_thread) {
        /*
        pthread_join使一个线程等待另一个线程结束。

代码中如果没有pthread_join主线程会很快结束从而使整个进程结束，从而使创建的线程没有机会开始执行就结束了。加入pthread_join后，主线程会一直等待直到等待的线程结束自己才结束，使创建的线程有机会执行。

所有线程都有一个线程号，也就是Thread ID。其类型为pthread_t。通过调用pthread_self()函数可以获得自身的线程号。

如果你的主线程，也就是main函数执行的那个线程，在你其他线程退出之前就已经退出，那么带来的bug则不可估量。通过pthread_join函数会让主线程阻塞，直到所有线程都已经退出。
        */
        int rt = pthread_join(m_thread, nullptr);
        if(rt) {
            SYLAR_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt
                << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
        //std::cout << "join name= "<<m_name<<std::endl;
        //此时线程已经结束
        m_thread = 0;
    }
}

void* Thread::run(void* arg) {
    Thread* thread = (Thread*)arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = sylar::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_cb);
    //唤醒
    thread->m_semaphore.notify();
    cb();
    return 0;
}

}