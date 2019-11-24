#pragma once

#include <atomic>
#include "iQueue.h"

using namespace std;

class FixedAtomicQueue : public IQueue {
private:
	uint8_t* arr;
	int capacity;
	alignas(128) atomic_int pop_counter;
	alignas(128) atomic_int push_counter;
	//atomic_int pop_counter;
	//atomic_int push_counter;

public:
	FixedAtomicQueue(int capacity) : capacity(capacity), pop_counter{ 0 }, push_counter{ 0 } 
	{
		arr = new uint8_t[capacity];
	}
	void push(uint8_t val);
	bool pop(uint8_t &val);

	~FixedAtomicQueue()
	{
		delete[] arr;
	}
};