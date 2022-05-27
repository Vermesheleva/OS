#include <iostream>
#include <fstream>
#include <windows.h>
#include "Order.cpp"

#pragma warning(disable : 4996)

using namespace std;

char READ = 'r';
char MODIFY = 'm';
char ORDER_NOT_FOUND = 'n';
char ORDER_FOUND = 'f';
char EXIT = 'e';
char END_OPERATION = 'd';
char END_MODIFY = 'n';
char filename[20];

volatile int readerCount = 0;
CRITICAL_SECTION cs;
HANDLE semaphore;
int numOfOrders;

void createBinaryFile() {
	ofstream fout(filename, ios::binary);

	cout << "Enter the number of orders: " << endl;;
	cin >> numOfOrders;

	for (int i = 0; i < numOfOrders; i++) {
		Order order;
		cout << "Enter order number: " << endl;
		cin >> order.n;

		cout << "Enter order name: " << endl;
		cin >> order.name;

		cout << "Enter product count: " << endl;
		cin >> order.amount;

		cout << "Enter product price: " << endl;
		cin >> order.price;

		fout.write((char*)&order, sizeof(Order));
		system("cls");
	}

	fout.close();
}

void readDataInBinaryFile()
{
	ifstream fin(filename, ios::binary);

	cout << "The content of binary file: " << endl;

	for(int i = 0; i < numOfOrders; i++){
		Order order;
		fin.read((char*)&order, sizeof(Order));
		cout << order.n << " " << order.name << " " << order.amount << " " << order.price << " " << endl;
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
		cout << "Create pipe failed.\n";
		system("pause");
		return 0;
	}

	if (!CreatePipe(&clientReadPipe, &writePipe, &sa, 0)) {
		cout << "Create pipe failed.\n";
		system("pause");
		return 0;
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	/*char dataForClient[200] = "Client ";
	strcat(dataForClient, (const char*)clientWritePipe);
	strcat(dataForClient, " ");
	strcat(dataForClient, (const char*)clientReadPipe);
	*/
	char clientCommandLine[200];
	sprintf(clientCommandLine, "Client %d %d", (int)clientWritePipe, (int)clientReadPipe);

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, clientCommandLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		cout << "The client process is not created. Error code : " << GetLastError() << endl;

		return 0;
	}

	while (true) {
		DWORD readBytes;
		DWORD writeBytes;

		char inputCommand;
		ReadFile(readPipe, &inputCommand, sizeof(inputCommand), &readBytes, NULL);

		if (inputCommand == READ) {
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
		else if (inputCommand == MODIFY) {
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
	return 0;
}




void main() {
	InitializeCriticalSection(&cs);
	semaphore = CreateSemaphore(NULL, 1, 1, NULL);

	cout << "Enter file order name:\n";
	cin >> filename;

	createBinaryFile();
	readDataInBinaryFile();

	cout << "Enter client count:\n";
	int clientCount;
	cin >> clientCount;
	HANDLE* handles = new HANDLE[clientCount];
	DWORD* ID = new DWORD[clientCount];
	for (int i = 0; i < clientCount; i++) {
		handles[i] = CreateThread(NULL, NULL, client, (void*)i, 0, &ID[i]);
	}

	WaitForMultipleObjects(clientCount, handles, TRUE, INFINITE);
	readDataInBinaryFile();

	for (int i = 0; i < clientCount; i++) {
		CloseHandle(handles[i]);
	}

	system("pause");
}