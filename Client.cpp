#include <windows.h>
#include <conio.h>
#include <iostream>
#include "Order.cpp"

using namespace std;

char READ = 'r';
char MODIFY = 'm';
char ORDER_NOT_FOUND = 'n';
char ORDER_FOUND = 'f';
char EXIT = 'e';
char END_OPERATION = 'd';
char END_MODIFY = 'n';


int main(int argc, char* argv[]) {
	HANDLE writePipe = (HANDLE)atoi(argv[1]);
	HANDLE readPipe = (HANDLE)atoi(argv[2]);

	while (true) {
		DWORD bytesWrite;
		DWORD bytesRead;
		int key;

		cout << "Enter the command key: 1 for read, 2 for modify order, 3 for exit: " << endl;
		
		cin >> key;
		system("cls");

		if (key == 1) {
			int numOfOrder;

			cout << "Enter the number of order to find: " << endl;
			cin >> numOfOrder;

			WriteFile(writePipe, &READ, sizeof(READ), &bytesWrite, NULL);
			WriteFile(writePipe, &numOfOrder, sizeof(numOfOrder), &bytesWrite, NULL);

			char serverAnswer;
			ReadFile(readPipe, &serverAnswer, sizeof(serverAnswer), &bytesRead, NULL);

			if (serverAnswer == ORDER_FOUND) {
				Order order;

				ReadFile(readPipe, &order, sizeof(order), &bytesRead, NULL);

				cout <<  order.name << " " << order.amount << " " << order.price << endl;
			}
			else {
				cout << "Your order is not found." << endl;
			}

			WriteFile(writePipe, &END_OPERATION, sizeof(END_OPERATION), &bytesWrite, NULL);
		}
		else if (key == 2) {

			int numOfOrder;

			cout << "Enter the number of order to modify: " << endl;
			cin >> numOfOrder;

			WriteFile(writePipe, &READ, sizeof(READ), &bytesWrite, NULL);

			WriteFile(writePipe, &numOfOrder, sizeof(numOfOrder), &bytesWrite, NULL);

			char serverAnswer;
			ReadFile(readPipe, &serverAnswer, sizeof(serverAnswer), &bytesRead, NULL);

			if (serverAnswer == ORDER_FOUND) {
				Order order;

				ReadFile(readPipe, &order, sizeof(order), &bytesRead, NULL);

				WriteFile(writePipe, &END_OPERATION, sizeof(END_OPERATION), &bytesWrite, NULL);

				cout <<  order.name << " " << order.amount << " " << order.price << endl << endl;

				cout << "Enter new order name: " << endl;
				cin >> order.name;

				cout << "Enter new product count: " << endl;
				cin >> order.amount;

				cout << "Enter new product price: " << endl;
				cin >> order.price;

				WriteFile(writePipe, &MODIFY, sizeof(MODIFY), &bytesWrite, NULL);

				WriteFile(writePipe, &order, sizeof(order), &bytesWrite, NULL);
			}
			else {
				cout << "Your order is not found." << endl;
			}

			char answer;
			ReadFile(readPipe, &answer, sizeof(answer), &bytesRead, NULL);

			WriteFile(writePipe, &END_OPERATION, sizeof(END_OPERATION), &bytesWrite, NULL);
		}
		else {
			WriteFile(writePipe, &EXIT, sizeof(EXIT), &bytesWrite, NULL);
			break;
		}
	}

	CloseHandle(writePipe);
	CloseHandle(readPipe);
	return 0;
}