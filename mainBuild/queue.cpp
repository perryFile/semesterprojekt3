//queue.cpp 
#include "queue.h"

Queue::Queue(){}

void Queue::Push(const std::vector<std::int32_t>& data){
    std::unique_lock<std::mutex> lock(d_mutex);
    d_queue.push_back(data);
    d_condition.notify_one();
}

//Use for now :)
std::optional<std::vector<std::int32_t>> Queue::TryPop(){
    std::unique_lock<std::mutex> lock(d_mutex);
    if (d_queue.empty()) {
        return std::nullopt; // Return empty optional if the queue is empty
    }
    std::vector<std::int32_t> data = d_queue.front();
    d_queue.pop_front();
    return data;
}

//use for when threads is incorporated
std::optional<std::vector<std::int32_t>> Queue::Pop(){
    std::unique_lock<std::mutex> lock(d_mutex);
    // Wait until the queue is not empty
    d_condition.wait(lock, [this](){ return !d_queue.empty(); });
    std::vector<std::int32_t> data = d_queue.front();
    d_queue.pop_front();
    return data;
}


Queue::Queue(){}