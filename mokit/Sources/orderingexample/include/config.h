#pragma once

//asm volatile("dmb sy" ::: "memory");
//asm volatile("mfence" ::: "memory");

#define PREVENT_STORE_STORE_REORDERING         0
#define PREVENT_LOAD_LOAD_REORDERING           0

// load-store reorder detect
#define CACHE_STRIDE_SHIFT          15  //32768
#define CACHE_LINE_OFFSET_SHIFT     7   //128
#define CACHE_LINE_SPLIT            60

#define EVICT_CACHE              0  //evict cache without cache flush instructions
