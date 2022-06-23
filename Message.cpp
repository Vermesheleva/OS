#include <iostream>
#pragma warning(disable : 4996)

using namespace std;

const int nameLen = 10;
const int textLen = 20;

struct Message {
	char name[nameLen];
	char text[textLen];

	Message() {
		strcpy(name, "");
		strcpy(text, "");
	}

	Message(const char* name, const char* text) {
		strcpy(this->name, name);
		strcpy(this->text, text);
	}
};