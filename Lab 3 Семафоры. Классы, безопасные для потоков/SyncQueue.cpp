#include <iostream>
#include <Windows.h>

using std::cout;
using std::cin;
using std::endl;

class SyncQueue {
private:
	int size;
	int* data;
	int head;
	int tail;
	HANDLE canRemove;
	HANDLE canAdd;
	CRITICAL_SECTION criticalSection;

public:
	SyncQueue(int size) {
		this->size = size;
		data = new int[size];
		head = 0;
		tail = 0;
		canRemove = CreateSemaphore(NULL, 0, size, NULL);
		canAdd = CreateSemaphore(NULL, size, size, NULL);
		InitializeCriticalSection(&criticalSection);
	}

	~SyncQueue() {
		
		DeleteCriticalSection(&criticalSection);
		CloseHandle(canAdd);
		CloseHandle(canRemove);
		delete[] data;
	}

	bool isEmpty() {
		if (head == tail) {
			return true;
		}
		else {
			return false;
		}
	}

	bool isFull() {
		if (tail == size) {
			return true;
		}
		else {
			return false;
		}
	}

	void Insert(int elem) {
		WaitForSingleObject(canAdd, INFINITE);

		EnterCriticalSection(&criticalSection);
		
		data[tail] = elem;
		tail++;

		cout << "The number " << tail << " is produced." << endl;

		if (isFull()) {
			tail = 0;
		}
		
		LeaveCriticalSection(&criticalSection);
		ReleaseSemaphore(canRemove, 1, NULL);
		
	}

	int Remove() {
	    WaitForSingleObject(canRemove, INFINITE);
		
		EnterCriticalSection(&criticalSection);
		
		int elem = data[head];
		head++;

		cout << "The number " << head << " is consumed." << endl;

		if (isEmpty()) {
			head = 0;
		}
		
		LeaveCriticalSection(&criticalSection);
		ReleaseSemaphore(canAdd, 1, NULL);
		

		return elem;
	}
};


