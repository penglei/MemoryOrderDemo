#pragma once

#include <atomic>
#include <chrono>
#include <thread>
#include "mersenne_twister.h"
#include "semaphore.h"
#include "config.h"


class ProducerConsumerReorderingTester {
public:
    ProducerConsumerReorderingTester();
private:
    int data1[4096 * 3];
    std::atomic<bool> result;
    std::atomic<bool> flag;

    Semaphore producerSema_;
    Semaphore consumerSema_;
    Semaphore completeSema_;

    std::thread thread_producer_;
    std::thread thread_consumer_;

    void thread1FuncProduce();
    void thread2FuncConsume();
};
