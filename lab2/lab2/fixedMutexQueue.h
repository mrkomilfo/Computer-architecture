#pragma once

#include <mutex>
#include <condition_variable>
#include "iQueue.h"

using namespace std;

class FixedMutexQueue : public IQueue {
private:
	uint8_t* arr;
	uint8_t head;
	uint8_t tail;

	int capacity;
	int size;

	mutex mtx;
	condition_variable cv;

public:
	FixedMutexQueue(int capacity) : head(0), tail(0), capacity(capacity), size(0) 
	{
		arr = new uint8_t[capacity];
	}
	void push(uint8_t val);
	bool pop(uint8_t &val);
	~FixedMutexQueue()
	{
		delete[] arr;
	}
};