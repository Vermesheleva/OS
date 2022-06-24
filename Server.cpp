#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include "Order.cpp"
#include "Keys.h"

#pragma warning(disable : 4996)

using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::string;
using std::to_string;
using std::ifstream;
using std::ios;

volatile int readerCount = 0;
CRITICAL_SECTION cs;
HANDLE semaphore;
int numOfOrders;
string filename;

void createBinaryFile() {
	ofstream fout(filename, ios::binary);

	cout << "Enter the number of orders: " << endl;;
	cin >> numOfOrders;
	system("cls");

	for (int i = 0; i < numOfOrders; i++) {
		Order order;
		cout << "Enter order number: " << endl;
		cin >> order.n;

		cout << "Enter order name: " << endl;
		cin >> order.name;

		cout << "Enter the amount of product: " << endl;
		cin >> order.amount;

		cout << "Enter product price: " << endl;
		cin >> order.price;

		fout.write((char*)&order, sizeof(Order));
		system("cls");
	}

	fout.close();
}

void readDataInBinaryFile(){
	ifstream fin(filename, ios::binary);

	cout << "The content of binary file: " << endl;

	for(int i = 0; i < numOfOrders; i++){
		Order order;
		fin.read((char*)&order, sizeof(Order));
		cout << order.toString() << endl;
	}

	fin.close();
}


Order* findOrder(int num) {
	ifstream fin(filename, ios::binary);

	while (!fin.eof()) {

		Order* order = new Order;
		fin.read((char*)order, sizeof(Order));

		if (order->n == num) {
			fin.close();

			return order;
		}
	}

	cout << "Your order is not found." << endl;
	fin.close();

	return nullptr;
}

void modifyOrder(Order modifiedOrder) {
	fstream fst(filename, ios::binary | ios::in | ios::out);
	int pos = 0;
	
	while (!fst.eof()){
		Order order;

		fst.read((char*)&order, sizeof(Order));

		if (order.n == modifiedOrder.n) {
			fst.seekp(pos * sizeof(Order), ios::beg);
			fst.write((char*)&modifiedOrder, sizeof(Order));
			fst.close();

			return;
		}
		else {
			pos++;
		}
	}
}



DWORD WINAPI client(LPVOID data) {
	HANDLE writePipe;
	HANDLE readPipe;
	HANDLE clientReadPipe;
	HANDLE clientWritePipe;
	SECURITY_ATTRIBUTES sa;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&readPipe, &clientWritePipe, &sa, 0)) {
		cout << "Pipe is not created." << endl;

		CloseHandle(readPipe);
		CloseHandle(clientWritePipe);

		system("pause");
		exit(0);
	}

	if (!CreatePipe(&clientReadPipe, &writePipe, &sa, 0)) {
		cout << "Pipe is not created." << endl;

		CloseHandle(readPipe);
		CloseHandle(clientWritePipe);
		CloseHandle(writePipe);
		CloseHandle(clientReadPipe);

		system("pause");
		exit(0);
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;


	string client = "Client " + to_string((int)clientWritePipe) + " " + to_string((int)clientReadPipe);
	char* clientCommandLine = new char[client.length() + 1];
	strcpy(clientCommandLine, client.c_str());

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, clientCommandLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		cout << "The client process is not created. Error code : " << GetLastError() << endl;
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		CloseHandle(readPipe);
		CloseHandle(clientWritePipe);
		CloseHandle(writePipe);
		CloseHandle(clientReadPipe);

		system("pause");
		exit(0);
	}

	while (true) {
		DWORD readBytes;
		DWORD writeBytes;

		char inputCommand;
		ReadFile(readPipe, &inputCommand, sizeof(inputCommand), &readBytes, NULL);

		if (inputCommand == TO_READ) {
			EnterCriticalSection(&cs);
			++readerCount;
			if (readerCount == 1) {
				WaitForSingleObject(semaphore, INFINITE);
			}
			LeaveCriticalSection(&cs);

			int orderNumber;
			ReadFile(readPipe, &orderNumber, sizeof(int), &readBytes, NULL);
			Order* order = findOrder(orderNumber);

			if (order != nullptr) {
				WriteFile(writePipe, &ORDER_FOUND, sizeof(ORDER_FOUND), &writeBytes, NULL);
				WriteFile(writePipe, order, sizeof(Order), &writeBytes, NULL);
			}
			else {
				WriteFile(writePipe, &ORDER_NOT_FOUND, sizeof(ORDER_NOT_FOUND), &writeBytes, NULL);
			}

			char end;
			ReadFile(readPipe, &end, sizeof(end), &readBytes, NULL);

			EnterCriticalSection(&cs);
			--readerCount;
			if (readerCount == 0) {
				ReleaseSemaphore(semaphore, 1, NULL);
			}
			LeaveCriticalSection(&cs);
		}
		else if (inputCommand == TO_MODIFY) {
			WaitForSingleObject(semaphore, INFINITE);

			Order order;
			ReadFile(readPipe, &order, sizeof(Order), &readBytes, NULL);
			modifyOrder(order);
			WriteFile(writePipe, &END_MODIFY, sizeof(END_MODIFY), &writeBytes, NULL);

			char end;
			ReadFile(readPipe, &end, sizeof(end), &readBytes, NULL);

			ReleaseSemaphore(semaphore, 1, NULL);
		} else {
			break;
		}
	}

	CloseHandle(readPipe);
	CloseHandle(clientWritePipe);
	CloseHandle(writePipe);
	CloseHandle(clientReadPipe);
	return 0;
}




int main() {
	int numOfClients;
	InitializeCriticalSection(&cs);
	semaphore = CreateSemaphore(NULL, 1, 1, NULL);

	cout << "Enter file order name: " << endl;;
	cin >> filename;

	createBinaryFile();
	readDataInBinaryFile();

	cout << "Enter the amount of clients: " << endl;;
	cin >> numOfClients;

	HANDLE* handles = new HANDLE[numOfClients];
	DWORD* ID = new DWORD[numOfClients];

	for (int i = 0; i < numOfClients; i++) {
		handles[i] = CreateThread(NULL, NULL, client, (void*)i, 0, &ID[i]);
	}

	WaitForMultipleObjects(numOfClients, handles, TRUE, INFINITE);
	readDataInBinaryFile();

	for (int i = 0; i < numOfClients; i++) {
		CloseHandle(handles[i]);
	}

	system("pause");
	return 0;
}