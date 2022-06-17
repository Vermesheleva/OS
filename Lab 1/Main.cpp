#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include "Employee.cpp"

#pragma warning(disable : 4996)

using std::cin;
using std::cout;
using std::ios;
using std::ofstream;
using std::endl;
using std::string;
using std::ifstream;

void runCreatorProcess(string filename, string kolvo) {
	string creator = "Creator " + filename + " " + kolvo;
	char* commandLine = new char[creator.length() + 1];
	strcpy(commandLine, creator.c_str());


	STARTUPINFO si;
	PROCESS_INFORMATION create;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &create))
	{
		cout << "The Creator process is not created" << endl;
		CloseHandle(create.hThread);
		CloseHandle(create.hProcess);
		delete[] commandLine;
	}

	else cout << "The Creator process is created" << endl;

	WaitForSingleObject(create.hProcess, INFINITE);

	CloseHandle(create.hThread);
	CloseHandle(create.hProcess);
	delete[] commandLine;
}

void readBinFile(string filename, int num) {
	ifstream fin;
	fin.open(filename, ios::binary);

	cout << "The contents of binary file: " << endl;

	for (int i = 0; i < num; i++) {
		employee temp;
		fin.read((char*)&temp, sizeof(struct employee));
		cout << temp.num << " " << temp.name << " " << temp.hours << endl;
	}

	fin.close();
}

void runReporterProcess(string reportFile, string binfile, string payment, string kolvo) {
	string reporter = "Reporter " + binfile + " " + reportFile + " " + payment + " " + kolvo;
	char* commandLine = new char[reporter.length() + 1];
	strcpy(commandLine, reporter.c_str());
	
	STARTUPINFO si;
	PROCESS_INFORMATION report;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE,
		NULL, NULL, NULL, &si, &report))
	{
		cout << "The Reporter process is not created" << endl;
		CloseHandle(report.hThread);
		CloseHandle(report.hProcess);
		delete[] commandLine;
	}

	else cout << "The Reporter process is created" << endl;

	WaitForSingleObject(report.hProcess, INFINITE);

	CloseHandle(report.hThread);
	CloseHandle(report.hProcess);
	delete[] commandLine;
}


void readReportFile(string filename) {
	ifstream fin(filename);
	string line;

	cout << "The contents of report file: " << endl;

	while (!fin.eof()) {
		getline(fin, line);
		cout << line << endl;
	}

	fin.close();
}

int main() {
	string binFileName;
	string kolvo;

	cout << "Enter binary file name: " << endl;
	cin >> binFileName;
	cout << "Enter number of notes: " << endl;
	cin >> kolvo;
	cout << endl;

	runCreatorProcess(binFileName, kolvo);
	cout << endl;

	int num = stoi(kolvo);
	readBinFile(binFileName, num);
	cout << endl;

	string reportFile;
	cout << "Enter report file name " << endl;
	cin >> reportFile;
	string payment;
	cout << "Enter payment " << endl;
	cin >> payment;
	cout << endl;
	runReporterProcess(reportFile, binFileName, payment, kolvo);
	cout << endl;

	readReportFile(reportFile);

	return 0;
}
