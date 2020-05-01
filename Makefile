CPP=g++
CPPFLAGS=-Wall -std=gnu++11

all: ca_evol_leadership
ca_evol_leadership: main.cpp ca_evolution.hpp ca_simulator.hpp additional.hpp
	$(CPP) $(CPPFLAGS) -o $@ $^