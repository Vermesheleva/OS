#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#pragma warning(disable : 4996)

using namespace std;

struct employee {
	int num;
	char name[10];
	double hours;
};

void runCreatorProcess(string filename, string kolvo) {
	string creator = "Creator " + filename + " " + kolvo;
	wchar_t wcreator[200];
	mbstowcs(wcreator, creator.c_str(), creator.length());
	LPWSTR lcreator = wcreator;

	STARTUPINFO si;
	PROCESS_INFORMATION create;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, lcreator, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &create))
	{
		cout << "The Creator process is not created" << endl;
	}

	else cout << "The Creator process is created" << endl;

	WaitForSingleObject(create.hProcess, INFINITE);

	CloseHandle(create.hThread);
	CloseHandle(create.hProcess);
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
	string reporter = "Reporter " + binfile+ " " + reportFile + " " + payment + " " + kolvo;
	wchar_t wreporter[200];
	mbstowcs(wreporter, reporter.c_str(), reporter.length());
	LPWSTR lreporter = wreporter;
	STARTUPINFO si;
	PROCESS_INFORMATION report;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, lreporter, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, NULL, &si, &report))
	{
		cout << "The Reporter process is not created" << endl;
	}

	else cout << "The Reporter process is created" << endl;

	WaitForSingleObject(report.hProcess, INFINITE);

	CloseHandle(report.hThread);
	CloseHandle(report.hProcess);
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
