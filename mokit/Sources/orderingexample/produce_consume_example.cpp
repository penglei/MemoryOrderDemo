#include <iostream>
#include "produce_consume_example.h"

ProducerConsumerReorderingTester::ProducerConsumerReorderingTester():
    thread_producer_([this](){ thread1FuncProduce();}),
    thread_consumer_([this](){ thread2FuncConsume();})
{
    uint64_t detected = 0;
    for (uint64_t iterations = 1; ; iterations++)
    {
        data1[4096] = 0;
        result.store(false, std::memory_order_seq_cst);
        flag.store(false, std::memory_order_seq_cst);

        // Signal both threads
        producerSema_.Signal();
        consumerSema_.Signal();

        // Wait for both threads
        completeSema_.Wait();
        completeSema_.Wait();

        //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        if (result.load(std::memory_order_acquire))
        {
            detected++;
            std::cout << detected << " reorders detected after " << iterations << " iterations\n";
        }

    }
}

void ProducerConsumerReorderingTester::thread1FuncProduce()
{
    MersenneTwister random(1);
    while(true) {
        producerSema_.Wait();
        while (random.integer() % 8 != 0) {}  // Random delay

        // ----- THE TRANSACTION! -----
        data1[4096] = 2333;

#if PREVENT_STORE_STORE_REORDERING
        flag.store(true, std::memory_order_release);
        //flag = true;
        //asm volatile("mfence" ::: "memory");  // Prevent CPU reordering
        //asm volatile("dmb sy" ::: "memory");
#else
        flag.store(true, std::memory_order_relaxed);
#endif

        completeSema_.Signal(); // Notify transaction complete
    }
    // Never returns
}

void ProducerConsumerReorderingTester::thread2FuncConsume()
{
    MersenneTwister random(2);
    for (;;)
    {
        consumerSema_.Wait();
        while (random.integer() % 8 != 0) {}  // Random delay

#if PREVENT_LOAD_LOAD_REORDERING
        while (!flag.load(std::memory_order_acquire)) {}
#else
        while (!flag.load(std::memory_order_relaxed)) {}
#endif
        //while (!flag) {
            //asm volatile("dmb sy" ::: "memory");
            //asm volatile("mfence" ::: "memory");
        //}
        auto d = data1[4096];

        if (d == 0) {
            //reorder detected
            result.store(true, std::memory_order_release);
        }

        completeSema_.Signal(); // Notify transaction complete
    }
    // Never returns
}
