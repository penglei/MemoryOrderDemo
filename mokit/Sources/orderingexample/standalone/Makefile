.PHONY: ordering-mutual-exclusion  ordering-produce-consume

all: ordering-mutual-exclusion ordering-produce-consume

ordering-mutual-exclusion:
	clang++ -std=c++14 -lpthread -o $@ -O2 -I ../include mutual_exclusion_main.cpp ../mutual_exclusion_example.cpp ../mersenne_twister.cpp

ordering-produce-consume:
	clang++ -std=c++14 -lpthread -o $@ -O2 -I ../include produce_consume_main.cpp ../produce_consume_example.cpp ../mersenne_twister.cpp

