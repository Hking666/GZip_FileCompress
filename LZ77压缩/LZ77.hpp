#pragma once
#include<string>
#include"HashTable.hpp"

class LZ77 {
public:
	LZ77();
	~LZ77();
	void LZCompressFile(const std::string& fileName);
	void UnLZCompressFile(const std::string& fileName);
private:
	void MergeFile(FILE* fW, ULL fileSize);
	void fillWindow(USH& start, FILE* fR, size_t& readSize);
	USH  LongestMatch(USH matchHead, USH &curMatchDist, USH start);
	void WriteFlag(FILE* file, UCH& chNum, UCH& bitCount, bool isLen);
private:
	UCH* _pWin;
	HashTable _ht;
	std::string _fileName;
};