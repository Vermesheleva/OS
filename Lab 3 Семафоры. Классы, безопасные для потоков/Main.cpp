#include <Windows.h>
#include <iostream>
#include "SyncQueue.cpp"

HANDLE start;
int sleepTime = 100;

using std::cout;
using std::cin;
using std::endl;


struct DataForProducer {
	int number;
	int kolvo;
	SyncQueue* queue;
};

DWORD WINAPI producer(LPVOID param) {
	WaitForSingleObject(start, INFINITE);

	DataForProducer* data = (DataForProducer*)param;

	for (int i = 0; i < data->kolvo; i++) {
		data->queue->Insert(data->number);
		Sleep(sleepTime);
	}

	return 0;
};

struct DataForConsumer {
	int kolvo;
	SyncQueue* queue;
};

DWORD WINAPI consumer(LPVOID param) {
	WaitForSingleObject(start, INFINITE);

	DataForConsumer* data = (DataForConsumer*)param;

	for (int i = 0; i < data->kolvo; i++) {
		int num = data->queue->Remove();
		Sleep(sleepTime);
	}

	return 0;
};

DataForProducer* createDataForProducers(int n, SyncQueue* queue) {
	DataForProducer* data = new DataForProducer[n];

	for (int i = 0; i < n; i++) {
		data[i].number = i + 1;
		cout << "Enter the amound of numbers to produce in thread " << (i + 1) << endl;
		int kolvo;
		cin >> kolvo;
		data[i].kolvo = kolvo;
		data[i].queue = *&queue;
	}
	system("cls");

	return data;
};

DataForConsumer* createDataForConsumers(int n, SyncQueue* queue) {
	DataForConsumer* data = new DataForConsumer[n];

	for (int i = 0; i < n; i++) {
		cout << "Enter the amound of numbers to consume in thread " << (i + 1) << endl;
		int kolvo;
		cin >> kolvo;
		data[i].kolvo = kolvo;
		data[i].queue = *&queue;
	}
	system("cls");

	return data;
};

HANDLE* createThreads(DataForProducer* data1, DataForConsumer* data2, int n1, int n2) {
	HANDLE* handles = new HANDLE[n1 + n2];
	DWORD* IDs = new DWORD[n1 + n2];


	for (int i = 0; i < n1; i++) {
		handles[i] = CreateThread(NULL, 0, producer, (void*)&data1[i], 0, &IDs[i]);

		if (handles[i] == NULL) {
			cout << "The " << (i + 1) << " producer thread is not created." << endl;

			for (int j = i; j >= 0; j--) {
				CloseHandle(handles[j]);
			}
			exit(0);
		}
	}

	for (int i = n1; i < n1 + n2; i++) {
		handles[i] = CreateThread(NULL, 0, consumer, (void*)&data2[i - n1], 0, &IDs[i]);

		if (handles[i] == NULL) {
			cout << "The " << (i - n1 + 1) << " consumer thread is not created." << endl;

			for (int j = i; j >= 0; j--) {
				CloseHandle(handles[j]);
			}
			exit(0);
		}
	}

	return handles;
}


void CloseHandles(HANDLE* handles, int size) {

	for (int i = 0; i < size; i++) {
		CloseHandle(handles[i]);
	}
};

int main() {
	int queueSize;
	int numOfProducers;
	int numOfConsumers;
	int handlesSize;
	DataForProducer* dataForProducers;
	DataForConsumer* dataForConsumers;

	start = CreateEvent(NULL, TRUE, FALSE, NULL);

	cout << "Enter the size of queue: " << endl;
	cin >> queueSize;
	cout << "Enter the number of producers: " << endl;
	cin >> numOfProducers;
	cout << "Enter the number of consumers: " << endl;
	cin >> numOfConsumers;
	system("cls");

	SyncQueue* queue = new SyncQueue(queueSize);

	dataForProducers = createDataForProducers(numOfProducers, queue);

	DWORD* IDs = new DWORD[numOfConsumers + numOfProducers];
	HANDLE* handles = new HANDLE[numOfConsumers + numOfProducers];

	dataForConsumers = createDataForConsumers(numOfConsumers, queue);
	handlesSize = numOfConsumers + numOfProducers;

	handles = createThreads(dataForProducers, dataForConsumers, numOfProducers, numOfConsumers);
	SetEvent(start);

	WaitForMultipleObjects(handlesSize, handles, TRUE, INFINITE);

	CloseHandle(start);
	CloseHandles(handles, handlesSize);

	return 0;
}
