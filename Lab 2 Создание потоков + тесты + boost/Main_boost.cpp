#include <boost\thread\thread.hpp>
#include <boost\chrono\chrono.hpp>
#include <iostream>

using std::cout;
using std::cin;
using std::string;
using std::endl;

int sleepTime = 15;
int* param1;
int* param2;

void sleep() {
	boost::this_thread::sleep_for(boost::chrono::milliseconds(sleepTime));
}

void numerator() {
	int deg = param1[1];
	int x = param1[2];
	int result = 0;

	for (int i = 3; i < deg + 4; i++) {
		int temp = param1[i] * pow(x, (i - 3));
		sleep();
		result += temp;
	}

	cout << "p(" << x << ") = " << result << endl;
	param1[0] = result;

}

void denumerator() {
	int deg = param2[1];
	int x = param2[2];
	int result = 0;

	for (int i = 3; i < deg + 4; i++) {
		int temp = param2[i] * pow(x, (i - 3));
		sleep();
		result += temp;
	}

	cout << "p(" << x << ") = " << result << endl;
	param2[0] = result;

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


double calculateResult(int* param1, int* param2) {
	double result = 0;
	
	boost::thread num(numerator);
	boost::thread denum(denumerator);

	num.join();
	denum.join();

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

	param1 = createData("numerator", x);
	param2 = createData("denomerator", x);

	result = calculateResult(param1, param2);

	if (result == INFINITY) {
		return 0;
	}

	cout << "f(" << x << ") = " << result << endl;

	delete[] param1;
	delete[] param2;
	return 0;
}