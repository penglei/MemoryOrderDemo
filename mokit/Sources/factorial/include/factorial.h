#pragma once

long factorial(int n);

class Factorial {
public:
    Factorial(int n):n_(n){}
    long calc();
private:
    int n_;
};

