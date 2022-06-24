#include <windows.h>
#include <iostream>

using std::cout;
using std::cin;
using std::string;
using std::endl;
int sleepTime = 15;

DWORD WINAPI polynom(LPVOID lparam) {
	int* arr = (int*)lparam;
	int deg = (int)arr[1];
	int x = (int)arr[2];
	int result = 0;

	for (int i = 3; i < deg + 4; i++) {
		int temp = arr[i] * pow(x, (i - 3));
		Sleep(sleepTime);
		result += temp;
	}

	cout << "p(" << x << ") = " << result << endl;
	arr[0] = result;

	return 0;
}

int* createData(string name, int x) {
	int deg;

	cout << "Enter the degree of " << name << " : " << endl;
	cin >> deg;
	if (cin.fail()) {
		cout << "Uncorrect input." << endl;
		exit(0);
	}

	int* param = new int[deg + 4];
	param[1] = deg;
	param[2] = x;

	cout << "Enter the coefficients of " << name << " : " << endl;
	for (int i = 3; i < deg + 4; i++) {
		cin >> param[i];

		if (cin.fail()) {
			cout << "Uncorrect input." << endl;
			exit(0);
		}
	}
	cout << endl;
	system("cls");

	return param;
}

HANDLE* runThreads(int* param1, int* param2) {
	HANDLE* handles = new HANDLE[2];
	DWORD ID;
	handles[0] = CreateThread(NULL, 0, polynom, (void*)param1, 0, &ID);
	handles[1] = CreateThread(NULL, 0, polynom, (void*)param2, 0, &ID);

	for(int i = 0; i < 2; i++)
	if (handles[i] == NULL) {
		cout << "The thread is not created." << endl;
		CloseHandle(handles[1]);
		CloseHandle(handles[0]);
		delete[] param1;
		delete[] param2;
		exit(0);
	}

	return handles;
}

double calculateResult(int* param1, int* param2) {
	double result = 0;
	HANDLE* handles = new HANDLE[2];
	handles = runThreads(param1, param2);

	WaitForMultipleObjects(2, handles, TRUE, INFINITE);
	CloseHandle(handles[0]);
	CloseHandle(handles[1]);

	if ((double)param2[0] == 0) {
		cout << "The denomerator is 0." << endl;
		//для тестов
		return INFINITY;
	}

	result = ((double)param1[0] / (double)param2[0]);

	return result;
}

int main() {
	int x;
	double result;

	cout << "Enter x (integer number): " << endl;
	cin >> x;

	int* param1 = createData("numerator", x);
	int* param2 = createData("denomerator", x);
	
	result = calculateResult(param1, param2);

	if (result == INFINITY) {
		return 0;
	}

	cout << "f(" << x << ") = " << result << endl;
	
	delete[] param1;
	delete[] param2;
	return 0;
}