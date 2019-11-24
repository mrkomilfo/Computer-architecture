#pragma once

#include <vector>
#include <mutex>
#include <atomic>

using namespace std;

void incMutex(vector<int>& arr, int& index, mutex& mtx, int pause);
void incAtomic(vector<int>& arr, atomic_int& index, int pause);
void startMutex(int tasks, int threads_num, int pause);
void startAtomic(int tasks, int threads_num, int pause);
void startArrayTraversal();