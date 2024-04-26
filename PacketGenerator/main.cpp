#include <iostream>
using namespace std;

class A {
public:
	A() {

	}
	virtual ~A() {

	}
public:
	int a;

public:
	virtual void Set() {
		a = 10;
	}


};

class B : public A {
public:
	B() : A() {

	}
	virtual ~B() override {

	}

public:
	int b;

public:
	virtual void Set() override {
		a = 10;
		b = 20;
	}

};


int main(void) {

	char* test = new char[100];


	return 0;
}