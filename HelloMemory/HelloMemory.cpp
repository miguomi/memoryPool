// HelloMemory.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "alloc.h"

int main()
{
	//1
	char * data1 = new char[128];
	std::cout << "data1[" << data1 << "]" << std::endl;
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
	}

    return 0;
}

