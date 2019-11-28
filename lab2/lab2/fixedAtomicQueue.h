#pragma once

#include <atomic>
#include "iQueue.h"

using namespace std;

class FixedAtomicQueue : public IQueue {
private:
	atomic_uint8_t* arr;
	size_t capacity;
	alignas(128) atomic_size_t pop_counter;
	alignas(128) atomic_size_t push_counter;
	//atomic_int pop_counter;
	//atomic_int push_counter;
	mutex push_mtx;
	mutex pop_mtx;
	condition_variable push_cv;
	condition_variable pop_cv;

public:
	FixedAtomicQueue(int capacity) : capacity(capacity), pop_counter{ 0 }, push_counter{ 0 } 
	{
		arr = new atomic_uint8_t[capacity];
		for (int i = 0; i < capacity; i++)
			arr[i] = 0;
	}
	void push(uint8_t val);
	bool pop(uint8_t &val);

	~FixedAtomicQueue()
	{
		delete[] arr;
	}
};