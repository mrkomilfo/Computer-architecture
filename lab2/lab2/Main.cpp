#include <iostream>
#include <conio.h>
#include "arrayTraversal.h"
#include "queueExecutor.h"

using namespace std;

int main() 
{
	char task;
	bool again = true;
	while (again) 
	{
		cout << "1 - task 1\n2 - task 2.1\n3 - task 2.2\n4 - task 2.3\nother - exit\n\n";
		task = _getch();
		switch (task) 
		{
			case '1':
				startArrayTraversal();
				break;
			case '2':
				runDynamicQueue();
				break;
			case '3':
				runFixedMutexQueue();
				break;
			case '4':
				runFixedAtomicQueue();
				break;
			default:
				again = false;
		}
	}
	system("pause");
	return 0;
}