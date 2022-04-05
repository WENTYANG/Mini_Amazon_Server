#include "threadpool.h"
#include <iostream>
#include <memory>
#define _CRTDBG_MAP_ALLOC

using namespace std;

Threadpool::Threadpool(int max_thds, int max_tasks):max_threads(max_thds),max_tasks(max_tasks),run_flag(true){
    if (threads.size()==0){
        unique_lock<mutex> lck(m_lock);
        if (threads.size()==0){
            init_threads();
        }
    }
}

Threadpool * Threadpool::get_pool(){
    return this;
}



void Threadpool::run_task(){
    while(run_flag==true||!m_tasks.empty()){
        function<void()> task;
        unique_lock<mutex> lck(m_lock);
        if (run_flag == false && m_tasks.empty()) {
          return;
        }
        while(m_tasks.empty() && run_flag != false){
          //  std::cout << "waiting!!" <<std::endl;
            has_task.wait(lck);
        }
        if (m_tasks.empty()) {
          return;
        }
        task=m_tasks.front();
        m_tasks.pop();
        lck.unlock();
        task();
    }
}

bool Threadpool::init_threads(){
    for (int i=0;i<max_threads;i++){
        threads.push_back(thread(&Threadpool::run_task,this));
    }
    return true;
}

bool Threadpool::assign_task(function<void()> t){
    {
        unique_lock<mutex> lck(m_lock);
        if (m_tasks.size()<(unsigned int)max_tasks){
            m_tasks.push(t);
        }
        else{
            return false;
        }
        has_task.notify_one();
        //this_thread::sleep_for(300ms);
    }
    //has_task.notify_one();
    return true;
}

Threadpool::~Threadpool(){
    {
        unique_lock<mutex> ul(m_lock);
        run_flag=false;
    }
    has_task.notify_all();
    for (auto &t:threads){
        t.join();
    }
    threads.clear();
}
