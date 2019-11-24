#pragma once

#include "iQueue.h"

using namespace std;

void runQueue(IQueue &q, int p_n, int c_n);
void runDynamicQueue();
void runFixedMutexQueue();
void runFixedAtomicQueue();