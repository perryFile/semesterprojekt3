//queue.cpp 
#include "queue.h"

Queue::Queue(){}

void Queue::Push(const std::vector<std::int32_t>& data){
    std::unique_lock<std::mutex> lock(d_mutex);
    d_queue.push_back(data);
    d_condition.notify_one();
}

std::optional<std::vector<std::int32_t>> Queue::Pop(){
    std::unique_lock<std::mutex> lock(d_mutex);
    // Wait until the queue is not empty
    d_condition.wait(lock, [this](){ return !d_queue.empty(); });
    std::vector<std::int32_t> data = d_queue.back();
    d_queue.pop_back();
    return data;
}


~Queue::Queue(){}