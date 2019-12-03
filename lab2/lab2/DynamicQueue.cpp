#include "dynamicQueue.h"

void DynamicQueue::push(uint8_t val) {
	lock_guard<mutex> lock(mtx);
	queue.push(val);
}

bool DynamicQueue::pop(uint8_t &val) {
	mtx.lock();
	if (queue.empty()) {
		mtx.unlock();
		this_thread::sleep_for(chrono::milliseconds(1));
		mtx.lock();
		if (queue.empty()) 
		{
			mtx.unlock();
			return false;
		}
	}
	val = queue.front();
	queue.pop();
	mtx.unlock();
	return true;
}