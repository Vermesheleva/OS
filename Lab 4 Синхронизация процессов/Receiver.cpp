#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include "Message.cpp"

#pragma warning(disable : 4996)

using std::iostream;
using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::ios;
using std::ofstream;
using std::fstream;
using std::to_string;

HANDLE canRead;
HANDLE canWrite;
HANDLE mutex;
HANDLE* sendersThreads;
HANDLE* sendersProcesses;

int head = 0;
int tail = 0;
int queueSize;
int numOfSenders;


void createEmptyFile(string filename, int queueSize) {
	ofstream fout(filename, ios::binary);

	Message* mess = new Message(" - ", " - ");
	for (int i = 0; i < queueSize; i++) {
		fout.write((char*)&mess, sizeof(Message));
	}

	fout.close();
}

void readMessege(string filename) {
	WaitForSingleObject(canRead, INFINITE);

	fstream fin(filename, ios::binary | ios::in | ios::out);
	if (!fin.is_open()) {
		cout << "File is not open." << endl;

		CloseHandle(canRead);
		CloseHandle(canWrite);
		CloseHandle(mutex);

		for (int i = numOfSenders - 1; i >= 0; i--) {
			CloseHandle(sendersThreads[i]);
			CloseHandle(sendersProcesses[i]);
		}

		exit(0);
	}

	Message* mess = new Message();
	char p[10];
	int pos = sizeof(p) + sizeof(Message) * head;

	fin.seekg(pos, ios::beg);
	fin.read((char*)mess, sizeof(Message));
	cout << mess->name << " " << mess->text << endl;

	fin.seekp(pos, ios::beg);
	mess = new Message(" - ", " - ");
	fin.write((char*)mess, sizeof(Message));

	head++;
	if (head == queueSize) {
		head = 0;
	}

	fin.close();

	ReleaseSemaphore(canWrite, 1, NULL);
}

void runSenderProcess(string filename, int ind) {
	string sender = "Sender " + filename + " " + to_string(queueSize);
	char* commandLine = new char[sender.length() + 1];
	strcpy(commandLine, sender.c_str());

	STARTUPINFO si;
	PROCESS_INFORMATION send;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &send)){

		cout << "The sender process is not created. Error code: " << GetLastError() << endl;

		for (int i = ind; i >= 0; i--) {
			CloseHandle(sendersThreads[i]);
			CloseHandle(sendersProcesses[i]);
		}
		exit(0);
	}
	sendersThreads[ind] = send.hThread;
	sendersProcesses[ind] = send.hProcess;
}

int main() {
	string filename;
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
	sendersThreads = new HANDLE[numOfSenders];
	sendersProcesses = new HANDLE[numOfSenders];

	for (int i = 0; i < numOfSenders; i++) {
		runSenderProcess(filename, i);
	}

	system("cls");

	while (true) {
		cout << "Enter the command key: 1 for read, 2 for exit" << endl;
		cin >> key;
		if (key == 1) {
			readMessege(filename);
		}
		else if (key == 2) {
			break;
		}
		else {
			cout << "Wrong key." << endl;
			break;		
		}
	}

	CloseHandle(canRead);
	CloseHandle(canWrite);
	CloseHandle(mutex);

	for (int i = numOfSenders - 1; i >= 0; i--) {
		CloseHandle(sendersThreads[i]);
		CloseHandle(sendersProcesses[i]);
	}
}
