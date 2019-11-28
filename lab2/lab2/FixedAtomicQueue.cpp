#include <thread>
#include <mutex>
#include <chrono>
#include "fixedAtomicQueue.h"

void FixedAtomicQueue::push(uint8_t val) 
{
	/*while (true) 
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
	}*/

	while (true)
	{
		size_t tail = push_counter.load();
		auto tail_val = arr[tail % capacity].load(memory_order_acquire);
		if (tail != push_counter.load())
		{
			continue;
		}
		if (tail == pop_counter.load(memory_order_acquire) + capacity) //if queue is full
		{
			unique_lock<mutex> lock(push_mtx);
			push_cv.wait(lock);
			continue;
		}
		if (tail_val == 0)
		{
			if (arr[tail % capacity].compare_exchange_weak(tail_val, 1, memory_order_release)) {
				if (push_counter.compare_exchange_strong(tail, tail + 1, memory_order_release))
				{
					pop_cv.notify_one();
				}
				break;
			}
		}
		else {
			push_counter.compare_exchange_strong(tail, tail + 1, memory_order_release);
		}
	}
}

bool FixedAtomicQueue::pop(uint8_t &val) 
{
	/*while (true) 
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
			arr[head % capacity] = 0;
			return true;
		}
	}*/

	bool wait = true;
	while (true)
	{
		size_t head = pop_counter.load();
		auto head_val = arr[head % capacity].load(memory_order_acquire);
		if (head != pop_counter.load())
		{
			continue;
		}
		if (head == push_counter.load(memory_order_acquire)) {
			push_cv.notify_one();
			if (wait) {
				unique_lock<mutex> lock(pop_mtx);
				pop_cv.wait_for(lock, chrono::milliseconds(1));
				wait = false;
				continue;
			}
			return false;
		}

		if (head_val != 0)
		{
			if (arr[head % capacity].compare_exchange_weak(head_val, 0, memory_order_release)) {
				pop_counter.compare_exchange_strong(head, head + 1, memory_order_release);
				val = head_val;
				push_cv.notify_one();
				return true;
			}
		}
		else 
		{
			pop_counter.compare_exchange_strong(head, head + 1, memory_order_release);
		}
	}
}

