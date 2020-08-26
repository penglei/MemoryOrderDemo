#pragma once

#include <atomic>
#include <chrono>
#include <thread>
#include "mersenne_twister.h"
#include "semaphore.h"
#include "config.h"

#define M_X *(cache_detector + 4096)
#define M_Y cache_detector[4096 * 3]
#define M_R1 cache_detector[4096 * 4]
#define M_R2 cache_detector[4096 * (4 + 3)]

class MutualExclusionReorderingTester {
public:
    MutualExclusionReorderingTester();
private:
    int data1[4096 * 8];
    //std::atomic<int> X;
    //std::atomic<int> Y;

    Semaphore sema1_;
    Semaphore sema2_;
    Semaphore completeSema_;

    std::thread thread1_;
    std::thread thread2_;

    void thread1Func();
    void thread2Func();
};
