#pragma once

#include <vector>
#include <cstddef>
#include <mutex>
#include <condition_variable>

template<typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity);

    bool push(const T& item);
    bool pop(T& item);

    void shutdown();

private:
    std::vector<T> buffer_;
    size_t capacity_;
    std::mutex mutex_;
    std::condition_variable cv_not_full_;
    std::condition_variable cv_not_empty_;

    bool shutdown_;

    size_t head_;
    size_t tail_;
};

template<typename T>
RingBuffer<T>::RingBuffer(size_t capacity) 
    : buffer_(capacity), head_(0), tail_(0), capacity_(capacity), shutdown_(false) {}

template<typename T>
bool RingBuffer<T>::push(const T& item) {
    std::unique_lock<std::mutex> lock(mutex_);  
    
    cv_not_full_.wait(lock, [this]() {
        size_t next_head = (head_ + 1) % capacity_;
        return next_head != tail_ || shutdown_; 
    });

    if(shutdown_) return false;

    buffer_[head_] = item;
    head_ = (head_ + 1) % capacity_;

    lock.unlock();
    cv_not_empty_.notify_one();

    return true;
}

template<typename T>
bool RingBuffer<T>::pop(T& item) {
    std::unique_lock<std::mutex> lock(mutex_);  

    cv_not_empty_.wait(lock, [this]() { 
        return head_ != tail_ || shutdown_;
    });

    if(shutdown_ && head_ == tail_) {
        return false;
    }

    item = buffer_[tail_];
    tail_ = (tail_ + 1) % capacity_;

    lock.unlock(); 
    cv_not_full_.notify_one();

    return true;
}

template<typename T>
void RingBuffer<T>::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    shutdown_ = true;
    cv_not_full_.notify_all();
    cv_not_empty_.notify_all();
}