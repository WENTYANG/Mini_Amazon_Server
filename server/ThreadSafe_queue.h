#ifndef _ThreadSafe_queue_H
#define _ThreadSafe_queue_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

using namespace std;

//Problem: push and pop will copy the object multiple times.

template<typename T>
class ThreadSafe_queue {
 private:
  queue<T> data_queue;
  mutex mtx;
  condition_variable cv;

 public:
  ThreadSafe_queue();

  ThreadSafe_queue(ThreadSafe_queue const & other) {
    lock_guard<mutex> lock(other.mtx);
    data_queue = other.data_queue;
  }

  void push(const T & newValue) {
    lock_guard<mutex> lock(mtx);
    data_queue.push(newValue);
    cv.notify_one();
  }

  /*
    block current thread until it get an value from the queue.
  */
  void wait_and_pop(T & value) {
    unique_lock<mutex> lock(mtx);
    while (data_queue.empty()) {
      cv.wait(lock);
    }
    value = data_queue.front();
    data_queue.pop();
  }

  /*
    try to pop an value from the queue. If queue is empty,
    it will return false, else return true. 
  */
  bool try_pop(T & value) {
    lock_guard<mutex> lock(mtx);
    if (data_queue.empty())
      return false;
    value = data_queue.front();
    data_queue.pop();
    return true;
  }

  bool empty() const {
    lock_guard<mutex> lock(mtx);
    return data_queue.empty();
  }

  size_T size() const() {
    lock_guard<mutex> lock(mtx);
    return data_queue.size();
  }
};

#endif