// HelloMemory.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "alloc.h"
#include "objectPool.hpp"




//class MyClass:public ObjectPoolBase<MyClass>
class MyClass :public ObjectPoolBase<MyClass,10>
{
public:
	MyClass() {
		std::cout << "myclass create" << std::endl;
	}
	~MyClass() {
		std::cout << "myclass delete" << std::endl;
	}

private:

};


class MyClassA :public ObjectPoolBase<MyClassA,10>
{
public:
	MyClassA(int a,int b) {
		n = a + b;
		std::cout << "myclassAAA create[" << n << "]" << std::endl;
	}
	~MyClassA() {
		std::cout << "myclassAAA delete" << std::endl;
	}

private:
	int n;

};





int main()
{
	//1
	/*char * data1 = new char[128];
	//std::cout << "data1[" << data1 << "]" << std::endl;
	delete[] data1;

	//2
	char *data2 = new char;
	delete data2;

	char *data3 = new char[64];
	delete[] data3;

	char* data[120];
	for (int i = 0; i < 120; i++) {
		data[i] = new char[1+rand() % 1024];
	}

	for (int i = 0; i < 120; i++) {
		delete[] data[i];
	}*/

	MyClass *myClass2 = new MyClass();
	delete myClass2;
	
	MyClass *myClass = MyClass::createObject();
	MyClass::deleteObject(myClass);
	MyClassA *myClass1 = MyClassA::createObject(2,3);
	MyClassA::deleteObject(myClass1);

    return 0;
}

