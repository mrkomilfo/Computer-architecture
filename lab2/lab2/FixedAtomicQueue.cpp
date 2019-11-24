#include <thread>
#include "fixedAtomicQueue.h"

void FixedAtomicQueue::push(uint8_t val) 
{
	while (true) 
	{
		int tail = push_counter.load();
		if (tail == pop_counter + capacity) //if queue is full
		{
			continue;
		}
		if (push_counter.compare_exchange_strong(tail, tail + 1)) 
		{
			arr[tail % capacity] = val;
			return;
		}
	}
}

bool FixedAtomicQueue::pop(uint8_t &val) 
{
	while (true) 
	{
		int head = pop_counter.load();
		if (push_counter == head) //if queue is empty
		{
			this_thread::sleep_for(chrono::milliseconds(1));
			head = pop_counter.load();
			if (push_counter == head) 
			{
				return false;
			}
		}
		if (pop_counter.compare_exchange_strong(head, head + 1)) 
		{
			val = arr[head % capacity];
			return true;
		}
	}
}

