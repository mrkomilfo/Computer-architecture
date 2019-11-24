#pragma once

#include <mutex>
#include "iQueue.h"

using namespace std;

class DynamicQueue : public IQueue{
private:
	queue<uint8_t> queue;
	mutex mtx;

public:
	DynamicQueue() {}
	void push(uint8_t val);
	bool pop(uint8_t &val);
};