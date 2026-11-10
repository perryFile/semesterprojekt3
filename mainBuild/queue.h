// queue.h - Hjælpefunktion til lagdeling
#ifndef QUEUE_H
#define QUEUE_H
#include <cstdint>
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <deque>

class Queue{
    public: 
        Queue();
        void push(const std::vector<std::int32_t>& data);
        std::optional<std::vector<std::int32_t>> pop();

        ~Queue();

    private:
    std::mutex d_mutex;
    std::condition_variable d_condition;
    std::deque<std::vector<std::int32_t>> d_queue;

}; 

#endif // QUEUE_H