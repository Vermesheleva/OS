#include <string>
using namespace std;

struct Order {
	int n;
	char name[10];
	int amount;
	double price;

	string toString() {
		string res = this->n + " ";
		res.append(this->name);
		res += " " + this->amount;
		res += " ";
        res += this->price;

		return res;
	}
};