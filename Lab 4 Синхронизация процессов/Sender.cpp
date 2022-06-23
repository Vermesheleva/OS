#include <windows.h>
#include <iostream>
#include <fstream>
#include "Message.cpp"

#pragma warning(disable : 4996)

using std::fstream;
using std::cin;
using std::cout;
using std::endl;
using std::ios;
using std::string;

HANDLE canRead;
HANDLE canWrite;
HANDLE mutex;
int queueSize;
int tail;

void writeMessage(string filename, Message* message) {
	WaitForSingleObject(canWrite, INFINITE);
	WaitForSingleObject(mutex, INFINITE);

	fstream fout(filename, ios::binary | ios::in | ios::out);

	char p[10];
	fout.read(p, sizeof(p));
	tail = atoi(p);
	int pos = sizeof(p) + sizeof(Message) * tail;
	fout.seekp(pos, ios::beg);
	fout.write((char*)message, sizeof(Message));

	tail++;
	if (tail == queueSize) {
		tail = 0;
	}
	itoa(tail, p, 10);
	fout.seekp(0, ios::beg);
	fout.write(p, sizeof(p));


	fout.close();

	ReleaseMutex(mutex);
	ReleaseSemaphore(canRead, 1, NULL);
}

int main(int argc, char* argv[]) {
	string filename;
	char name[nameLen];
	int key;

	canRead = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Queue is full");
	canWrite = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Queue is empty");
	mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "Mutex");

	filename = argv[1];
	queueSize = atoi(argv[2]);

	cout << "Enter name of sender: " << endl;;
	cin >> name;


	while (true) {
		cout << "Enter the command key: 1 for write, 2 for exit " << endl;
		cin >> key;

		if (key == 1) {
			char text[textLen];
			cout << "Enter the text of the messege: " << endl;
			cin >> text;
			Message* mess = new Message(name, text);
			writeMessage(filename, mess);
			system("cls");
		}
		else if (key == 2) {
			return 0;
		}
		else {
			cout << "Wrong key." << endl;
			system("pause");
			return 0;
		}
	}
}
