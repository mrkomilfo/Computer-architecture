#include "fixedMutexQueue.h"

void FixedMutexQueue::push(uint8_t val) 
{
	unique_lock<mutex> lock(mtx);
	cv.wait(lock, [&] { return size < capacity; });
	arr[tail] = val;
	tail = (tail + 1) % capacity;
	size++;
	cv.notify_one();
}

bool FixedMutexQueue::pop(uint8_t &val) 
{
	unique_lock<mutex> lock(mtx);
	cv.notify_all();
	if (cv.wait_for(lock, chrono::milliseconds(1), [&] {return size > 0; })) 
	{
		val = arr[head];
		head = (head + 1) % capacity;
		size--;
		cv.notify_one();
		return true;
	} 
	else
	{
		return false;
	}
}