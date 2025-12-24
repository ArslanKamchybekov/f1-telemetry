#pragma once

#include <vector>
#include <cstddef>
#include <mutex>

using namespace std;

template<typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity);

    bool push(const T& item);   // returns false if full
    bool pop(T& item);          // returns false if empty

private:
    vector<T> buffer_;
    size_t capacity_;
    mutex mutex_;

    size_t head_;
    size_t tail_;
};

template<typename T>

RingBuffer<T>::RingBuffer(size_t capacity) : buffer_(capacity), head_(0), tail_(0), capacity_(capacity) {}

template<typename T>
bool RingBuffer<T>::push(const T& item) {
    lock_guard<mutex> lock(mutex_);
    size_t current_head = head_;
    size_t next_head = (current_head + 1) % capacity_;

    if(next_head == tail_) return false;

    buffer_[current_head] = item;
    head_ = next_head;
    return true;
}

template<typename T>
bool RingBuffer<T>::pop(T& item) {
    lock_guard<mutex> lock(mutex_);
    size_t current_tail = tail_;
    size_t next_tail = (current_tail + 1) % capacity_;

    if(current_tail == head_) return false;

    item = buffer_[current_tail];
    tail_ = next_tail;
    return true;
}