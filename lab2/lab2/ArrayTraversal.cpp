#include <thread>
#include <chrono>
#include <iostream>
#include "arrayTraversal.h"

void incMutex(vector<int>& arr, int& index, mutex& mtx, int pause) 
{
	int size = arr.size();
	while (true) 
	{
		unique_lock<mutex> ul(mtx);
		int old_index;
		if (index >= size)
		{
			return;
		}
		old_index = index++;
		ul.unlock();
		arr[old_index]++;
		this_thread::sleep_for(chrono::nanoseconds(pause));
	}
}

void incAtomic(vector<int>& arr, atomic_int& index, int pause) 
{
	while (true)
	{
		int old_index = index++;
		if (old_index >= arr.size())
		{
			return;
		}
		arr[old_index]++;
		this_thread::sleep_for(chrono::nanoseconds(pause));
	}
}

void startMutex(int tasks, int threads_num, int pause) 
{
	vector<int> arr(tasks, 0);
	vector<thread> threads(threads_num);
	int index = 0;
	mutex mtx;

	for (thread& thread : threads) 
	{
		thread = std::thread(incMutex, ref(arr), ref(index), ref(mtx), pause);
	}
	for (thread& thread : threads)
	{ 
		thread.join();
	}
	for (int num : arr)
	{
		if (num != 1)
		{
			cout << "Houston, we have a problem" << endl;
			return;
		}
	}
	cout << "Mutex is OK" << endl;
}

void startAtomic(int tasks, int threads_num, int sleep) 
{
	vector<int> arr(tasks, 0);
	atomic_int index(0);
	vector<thread> threads(threads_num);

	
	for (thread& t : threads) 
	{
		t = thread(incAtomic, ref(arr), ref(index), sleep);
	}

	for (thread& thread : threads) 
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
	for (int num : arr)
	{
		if (num != 1)
		{
			cout << "Houston, we have a problem" << endl;
			return;
		}
	}
	cout << "Atomic is OK" << endl;
}

void startArrayTraversal() {
	vector<int> pauses = { 0, 10 };
	vector<int> threads_nums = { 4, 8, 16, 32 };
	int tasks = 1024*1024;

	for (int pause : pauses)
	{
		for (int threads_num : threads_nums)
		{
			cout << "Threads: " << threads_num << endl;
			cout << "Pause: " << pause << " ns" << endl;

			int start = clock();
			startMutex(tasks, threads_num, pause);
			cout << "Mutex time: " << clock() - start << " ms" << endl;

			start = clock();
			startAtomic(tasks, threads_num, pause);
			cout << "Atomic time: " << clock() - start << " ms" << endl << endl;
		}
	}
}