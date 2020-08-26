//ARM64
//r0-7 input/res
//r8   indirect result location
//r9-r15 tmp
//r16-r18 IPx
//r19-r28 callee-saved
//r29-30 FP LR
#include "config.h"

.macro ALIGN_FUNC
     .align 4
.endm
.macro ALIGN_LOOP
     .align 4
.endm
.macro ALIGN_CACHELINE
     .align 6
.endm

.macro DEFUN name
     ALIGN_FUNC
     .global    \name
\name:
.endm

.macro DEFUNT name
ALIGN_FUNC
    .global     \name
    .thumb_func \name
    .code       16
\name:
.endm

#if __arm64__
.arch       armv8-a

DEFUN _cacheline_flush
    dc      civac,x0
    dmb     ish
    ret
#endif
