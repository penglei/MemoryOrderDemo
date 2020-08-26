#include "cwrapper.h"

//these header files shouldn't be contained in the cwrapper.h (because contain C++ syntax)
#include "factorial.h"
#include "produce_consume_example.h"
#include "mutual_exclusion_example.h"

long calc_factorial(int n){
    Factorial f(n);
    return f.calc();
}

void run_produce_consume_reorder_example() {
    ProducerConsumerReorderingTester tester;
}

void run_mutual_exclusion_reorder_example() {
    MutualExclusionReorderingTester tester;
}
