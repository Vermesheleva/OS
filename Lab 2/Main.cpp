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

	for (int i = deg; i >= 0; i--) {
		int temp = arr[deg - i + 3] * pow(x, i);
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

	int* param = new int[deg + 4];
	param[1] = deg;
	param[2] = x;

	cout << "Enter the coefficients of numerator: " << endl;
	for (int i = 3; i < deg + 4; i++) {
		cin >> param[i];
	}
	cout << endl;

	return param;
}

HANDLE runThread(int* param) {
	HANDLE handle;
	DWORD ID;
	handle = CreateThread(NULL, 0, polynom, (void*)param, 0, &ID);

	if (handle == NULL) {
		cout << "The thread is not created." << endl;
		delete[] param;
		return 0;
	}

	return handle;
}

int main() {
	int x;
	double result;

	cout << "Enter x: " << endl;
	cin >> x;

	int* param1 = createData("numerator", x);
	int* param2 = createData("denomerator", x);

	HANDLE hNumerator;
	HANDLE hDenomerator;

	hNumerator = runThread(param1);
	hDenomerator = runThread(param2);

	HANDLE* handles = new HANDLE[2];
	handles[0] = hNumerator;
	handles[1] = hDenomerator;

	WaitForMultipleObjects(2, handles, TRUE, INFINITE);
	CloseHandle(hNumerator);
	CloseHandle(hDenomerator);

	result = ((double)param1[0] / (double)param2[0]);
	cout << "f(" << x << ") = " << result << endl;
	
	delete[] param1;
	delete[] param2;
	return 0;
}
