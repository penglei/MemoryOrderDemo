//
//  Created by penglei on 2020/2/11.
//
#include <iostream>
#include <math.h>
#include <unistd.h>
#include "mutual_exclusion_example.h"
extern "C" void cacheline_flush(void *p);

#ifdef __arm64__
    #ifdef __ARM_ACLE
    #include <arm_acle.h>
    #endif

    //13:LD 11:ISH 10:ISHST 9:ISHLD
    #define DSB(x) __builtin_arm_dsb(x);
    #define DMB(x) __builtin_arm_dmb(x);
    #define DMB_LD DMB(9)      //Load-Load barrier
    #define CL_FLUSH(addr) cacheline_flush(addr)
#endif

template<typename T>
static void inline cl_flush_var(T x) {
#ifdef __arm64__
    CL_FLUSH(x);
#endif
}

size_t vm_page_size_ = 4096;

uint8_t * cache_detector = NULL;     //[256 * CACHE_STRIDE + CACHE_EVICT_SIZE]
uint8_t * cache_flush_area = NULL;   //[CACHE_EVICT_SIZE] after the end of detector

#define FLUSH_VAR(X) cl_flush_var((void *)(&(X)))

#define CACHE_STRIDE        (1<<CACHE_STRIDE_SHIFT)
#define CACHE_LINE_OFFSET   (1<<CACHE_LINE_OFFSET_SHIFT)
#define CACHE_LINE_SIZE     64

#define CACHE_EVICT_SIZE    (EVICT_CACHE * 32*1024*1024)
#define TEST_AREA_SIZE      (16*1024)

#define DETECTOR_PTR(byte, type) ((type*)(&cache_detector[byte*(CACHE_STRIDE + CACHE_LINE_OFFSET) + CACHE_LINE_SPLIT]))
#define DETECTOR_BYTEPTR(byte) DETECTOR_PTR(byte, uint8_t)

#define countof(arr) (sizeof(arr) / sizeof(arr[0]))
#define POW2_ALIGN(p, a)        ((p + (a - 1)) & ~(a - 1))
#define POW2_ALIGN_PTR(t, p, a) ((t)((uintptr_t(p) + (a - 1)) & ~(a - 1)))


#include <mach/vm_map.h>
#include <mach/mach_error.h>
#include <mach/mach_time.h>
#include <mach/mach_init.h>
uint8_t *phys_alloc(size_t size)
{
    uint8_t *addr;
    kern_return_t ret;

    if ((ret = vm_allocate(mach_task_self(), (vm_address_t*)&addr, size, TRUE)) != KERN_SUCCESS)
    {
        mach_error("vm_allocate failed: ", ret);
        exit(1);
    }

    memset(addr, 0xff, size);

    for (size_t i = 0; i < size; i+= CACHE_LINE_SIZE)
        addr[i] = i;

    return addr;
}

void phys_free(uint8_t *addr, size_t size)
{
    kern_return_t ret;
    if ((ret = vm_deallocate(mach_task_self(), (vm_address_t)addr, size)) != KERN_SUCCESS)
    {
        mach_error("vm_deallocate failed: ", ret);
        exit(1);
    }
}
MutualExclusionReorderingTester::MutualExclusionReorderingTester():
thread1_([this](){ thread1Func();}),
thread2_([this](){ thread2Func();})
{

    vm_page_size_ = sysconf(_SC_PAGESIZE);
    //printf("Page size %d\n", (int)vm_page_size_);

    size_t detector_size = POW2_ALIGN(256 * CACHE_STRIDE, vm_page_size_);
    cache_detector   = phys_alloc(detector_size + CACHE_EVICT_SIZE + vm_page_size_);
    cache_flush_area = cache_detector + detector_size;

#ifdef __arm64__
    printf("running on arm64\n");
#endif
    uint64_t detected = 0;
    for (uint64_t iterations = 1; ; iterations++)
    {
        M_X = 0;
        M_Y = 0;
        M_R1 = 0;
        M_R2 = 0;

        // Signal both threads
        sema1_.Signal();
        sema2_.Signal();

        // Wait for both threads
        completeSema_.Wait();
        completeSema_.Wait();
        //std::cout << "result pair:" << M_R1 << ":" << M_R2 << std::endl;
        if (M_R1 == 0 && M_R2 == 0) {
            detected += 1;
            std::cout << detected << " reorders detected after " << iterations << " iterations\n";
        }
    }
}

void MutualExclusionReorderingTester::thread1Func() {
    MersenneTwister random(1);
    while(true) {

        sema1_.Wait();
#ifdef __arm64__
        for (int i = 0; i < 256; i++)
            CL_FLUSH(DETECTOR_PTR(i, uint8_t));
#endif
        while (random.integer() % 8 != 0) {}  // Random delay

        M_X = 233;
        asm volatile("" ::: "memory");
        //std::atomic_thread_fence(std::memory_order_seq_cst);
        M_R1 = M_Y;

        completeSema_.Signal();
    }
}

void MutualExclusionReorderingTester::thread2Func() {
    MersenneTwister random(2);
    while(true) {

        sema2_.Wait();
#ifdef __arm64__
        for (int i = 0; i < 256; i++)
            CL_FLUSH(DETECTOR_PTR(i, uint8_t));
#endif
        while (random.integer() % 4 != 0) {}  // Random delay

        M_Y = 233;
        asm volatile("" ::: "memory");
        //std::atomic_thread_fence(std::memory_order_seq_cst);
        M_R2 = M_X;

        completeSema_.Signal();
    }
}
