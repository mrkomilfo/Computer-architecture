#include "dynamicQueue.h"

void DynamicQueue::push(uint8_t val) {
	lock_guard<mutex> lock(mtx);
	queue.push(val);
}

bool DynamicQueue::pop(uint8_t &val) {
	lock_guard<mutex> lock(mtx);
	bool result = false;
	if (!queue.empty()) {
		val = queue.front();
		queue.pop();
		result = true;
	}
	else
	{
		this_thread::sleep_for(chrono::milliseconds(1));
		{
			if (!queue.empty()) {
				val = queue.front();
				queue.pop();
				result = true;
			}
		}
	}
	return result;
}