#include <iostream>
#include <windows.h>
#include <fstream>
#include "Employee.cpp"

using std::ios;
using std::ofstream;
using std::endl;
using std::ifstream;


int main(int argc, char* argv[]) {
	char* binFile = argv[1];
	char* reportFile = argv[2];
	double payment = atoi(argv[3]);
	int kolvo = atoi(argv[4]);

	ifstream fin(binFile, ios::binary);
	ofstream fout(reportFile);
	fout << "Report of file: ";
	fout << binFile << endl;
	fout << "Number, name, hours, salary" << endl;
	for (int i = 0; i < kolvo; i++) {
		employee temp;
		fin.read((char*)&temp, sizeof(employee));
		double salary = temp.hours * payment;
		fout << temp.num << " " << temp.name << " " << temp.hours << " ";
		fout << salary << endl;
	}
	fin.close();
	fout.close();

	return 0;
};
