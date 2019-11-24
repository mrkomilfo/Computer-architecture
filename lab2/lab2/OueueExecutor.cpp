#include<thread>
#include<queue>
#include<vector>
#include<iostream>
#include "dynamicQueue.h"
#include "fixedMutexQueue.h"
#include "fixedAtomicQueue.h"
#include "iQueue.h"

using namespace std;

const int TASKS_NUM = 1024 * 1024;
const vector<int> producers_num = { 1, 2, 4 };
const vector<int> consumers_num = { 1, 2, 4 };
const vector<int> queue_capacity = { 1, 4, 16 };

void runQueue(IQueue &q, int p_n, int c_n)
{
	int t_n = TASKS_NUM / p_n;
	atomic_int sum = 0;
	vector<thread> producers(p_n);
	vector<thread> consumers(c_n);
	int start = clock();
	for (thread& p_thread : producers)
	{
		p_thread = thread([&t_n, &q]() 
		{
			for (int i = 0; i < t_n; ++i)
			{
				q.push(1);
			}
		});
	}
	for (thread& c_thread : consumers)
	{
		c_thread = thread([&sum, &q, t_n, p_n]() 
		{
			uint8_t val;
			while (sum.load() < p_n * t_n) 
			{
				sum.fetch_add(q.pop(val));
			}
		});
	}
	for (thread& p_thread : producers) 
	{
		if (p_thread.joinable())
		{
			p_thread.join();
		}
	}
	for (thread& c_thread : consumers) 
	{
		if (c_thread.joinable())
		{
			c_thread.join();
		}
	}
	int duration = clock() - start;
	cout << "Sum: " << sum.load() << endl 
		<< "Duration: " << duration << " ms" << endl << endl;
}

void runDynamicQueue()
{	
	for (int producer_num : producers_num)
	{
		for (int consumer_num : consumers_num)
		{
			for (int q_c : queue_capacity)
			{
				cout << "Producers: " << producer_num << endl
					<< "Consumers: " << consumer_num << endl
					<< "Queue capacity: " << q_c << endl;
				FixedAtomicQueue q(q_c);
				runQueue(q, producer_num, consumer_num);
			}
		}
	}
}

void runFixedMutexQueue()
{
	for (int producer_num : producers_num)
	{
		for (int consumer_num : consumers_num)
		{
			for (int q_c : queue_capacity)
			{
				cout << "Producers: " << producer_num << endl
					<< "Consumers: " << consumer_num << endl
					<< "Queue capacity: " << q_c << endl;
				FixedMutexQueue q(q_c);
				runQueue(q, producer_num, consumer_num);
			}
		}
	}
}

void runFixedAtomicQueue()
{
	for (int producer_num : producers_num)
	{
		for (int consumer_num : consumers_num)
		{
			for (int q_c : queue_capacity)
			{
				cout << "Producers: " << producer_num << endl
					<< "Consumers: " << consumer_num << endl
					<< "Queue capacity: " << q_c << endl;
				FixedAtomicQueue q(q_c);
				runQueue(q, producer_num, consumer_num);
			}
		}
	}
}