#include <vector>
#include <omp.h>
#include <chrono>
#include <iostream>
#include <cmath>

using namespace std;

int* conv(int* x, int N, int* h, int M)
{
	int * result = new int[N + M - 1];
	memset(result, 0, sizeof(int) * (N + M - 1));

	for (int i = 0; i < N + M - 1; i++)
	{
		for (int j = 0; j < M; j++)
		{
			if (i - j >= 0 && i - j < N)
				result[i] += x[i - j] * h[j];
		}
	}
	return result;
}

int* parallelConv(int* x, int N, int* h, int M)
{
	int * result = new int[N + M - 1];
	memset(result, 0, sizeof(int) * (N + M - 1));

#pragma omp parallel for
	for (int i = 0; i < N + M - 1; i++)
	{
		for (int j = 0; j <= i && j < M; j++)
		{
			if (i - j >= 0 && i - j < N)
				result[i] += x[i - j] * h[j];
		}
	}
	return result;
}

int main() {

	int n = 1 << 10;
	int m = 1 << 15;

	int *a = new int[n];
	for (int i = 0; i < n; i++)
	{
		a[i] = 13;
	}
	int *b = new int[m];
	for (int i = 0; i < m; i++)
	{
		b[i] = 21;
	}
	
	int start = clock();
	int* linear_res = conv(a, n, b, m);
	int time_linear = clock() - start;
	cout << "Non-parallel convolution: " << time_linear << " ms" << endl;
	/*for (int i = 0; i < n+m-1; i++)
	{
		cout << res[i] << " ";
	}
	cout << endl;*/

	start = clock();
	int* parallel_res = parallelConv(a, n, b, m);
	int time_parallel = clock() - start;
	cout << "Parallel convolution: " << time_parallel << " ms" << endl;
	/*for (int i = 0; i < n + m - 1; i++)
	{
		cout << parallelRes[i] << " ";
	}
	cout << endl;*/
	cout << "Speed up: " << (double)time_linear/time_parallel << endl;

	for (int i = 0; i < n + m - 1; i++)
	{
		if (linear_res[i] != parallel_res[i]) 
		{
			cout << "Houston, we have a problem" << endl;
		}
	}
	system("pause");
	return 0;
}