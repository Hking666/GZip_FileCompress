#define _CRT_SECURE_NO_WARNINGS 1
#include"FileCompressHuffman.hpp"
#include"FileCompressHuffman.cpp"
#include"LZ77.hpp"
#include<time.h>
using namespace std;

void test() {
	LZ77 lz;
	FileCompressHuffman tree;
	double a = clock();
	lz.LZCompressFile("1.png");

	lz.UnLZCompressFile("LZ77.bin");

	double b = clock();
	cout << (b - a) / 1000 << endl;
}
int main() {
	test();
	_CrtDumpMemoryLeaks();
	system("pause");
	return 0;
}