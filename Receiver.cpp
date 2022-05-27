#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Message.cpp"

#pragma warning(disable : 4996)

using namespace std;

HANDLE canRead;
HANDLE canWrite;
HANDLE mutex;
int head = 0;
int tail = 0;
int queueSize;


void createEmptyFile(char* filename, int queueSize) {
	ofstream fout(filename, ios::binary);

	Message* mess = new Message("empty", "empty");
	for (int i = 0; i < queueSize; i++) {
		fout.write((char*)&mess, sizeof(Message));
	}

	fout.close();
}

void readMessege(char* filename) {
	WaitForSingleObject(canRead, INFINITE);
	
	fstream fin(filename, ios::binary | ios::in | ios::out);
	if (!fin.is_open()) {
		cout << "File is not open." << endl;;
		return;
	}

	Message* mess = new Message();
	char p[10];
	int pos = sizeof(p) + sizeof(Message) * head;

	fin.seekg(pos, ios::beg);
	fin.read((char*)mess, sizeof(Message));
	cout << mess->name << " " << mess->text << endl;

	fin.seekp(pos, ios::beg);
	mess = new Message("deleted", "deleted");
	fin.write((char*)mess, sizeof(Message));

	head++;
	if (head == queueSize) {
		head = 0;
	}

	fin.close();

	ReleaseSemaphore(canWrite, 1, NULL);
}

void runSenderProcess(char* filename) {
	char data[50] = "Sender ";
	strcat(data, filename);
	strcat(data, " ");
	strcat(data, to_string(queueSize).c_str());
	strcat(data, " ");

	STARTUPINFO si;
	PROCESS_INFORMATION send;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, data, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &send))
	{
		cout << "The sender process is not created. Error code: " << GetLastError() << endl;
	}

	else cout << "The Sender process is created" << endl;
}

int main() {
	char filename[200];
	int numOfSenders;
	int key;

	cout << "Enter the name of binary file: " << endl;
	cin >> filename;
	cout << "Enter the number of messeges: " << endl;
	cin >> queueSize;
	cout << "Enter the number of senders: " << endl;
	cin >> numOfSenders;
	
	canRead = CreateSemaphore(NULL, 0, queueSize, "Queue is full");
	canWrite = CreateSemaphore(NULL, queueSize, queueSize, "Queue is empty");
	mutex = CreateMutex(NULL, FALSE, "Mutex");

	createEmptyFile(filename, queueSize);

	for (int i = 0; i < numOfSenders; i++) {
		runSenderProcess(filename);
	}
	

	while (true) {
		cout << "Enter the command key: 1 for read, 2 for exit" << endl;
		cin >> key;
		if (key == 1) {
			readMessege(filename);
		}
		if (key == 2) {
			return 0;
		}
	}

	CloseHandle(canRead);
	CloseHandle(canWrite);
	CloseHandle(mutex);
}
