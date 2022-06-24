#include <string>
using namespace std;

struct Order {
	int n;
	char name[10];
	int amount;
	double price;

	string toString() {
		string res = to_string(this->n) + " ";
		res += this->name;
		res += " " + to_string(this->amount);
		res += " ";
        res += to_string(this->price);

		return res;
	}
};