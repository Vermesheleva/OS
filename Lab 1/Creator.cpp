#include <iostream>
#include <windows.h>
#include <fstream>
#include "Employee.cpp"

using ::std::cin;
using ::std::cout;
using ::std::ios;
using ::std::ofstream;
using ::std::endl;

int main(int argc, char* argv[]) {

	char* filename = argv[1];
	ofstream fout(filename, ios::binary);
	int kolvo = atoi(argv[2]);

	for (int i = 0; i < kolvo; i++) {
		employee temp;
		cout << "Input employee number" << endl;
		cin >> temp.num;
		cout << "Input employee name" << endl;
		cin >> temp.name;
		cout << "Input employee working hours" << endl;
		cin >> temp.hours;
		fout.write((char*)&temp, sizeof(struct employee));

		system("cls");
	}
	fout.close();

	return 0;

}
