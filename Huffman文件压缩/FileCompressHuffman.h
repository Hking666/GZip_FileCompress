//����huffman��ѹ��
#pragma once

#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>  //reverse()
#include <fstream>
#include "Huffman.hpp"


//��Ȩֵ�ýṹ���ʾ,������Ҫ��huffman�еĲ�������������
struct CharInfo
{
	unsigned char _ch;  //�����ַ�
	int _count;  //�ַ����ִ���
	std::string _strCode;  //�ַ�����

	CharInfo(int count = 0)
		: _count(count)
	{}

	CharInfo operator+(const CharInfo& ch) const
	{
		return CharInfo(_count + ch._count);
	}

	bool operator>(const CharInfo& ch) const
	{
		return _count > ch._count;
	}

	bool operator==(const CharInfo& ch) const
	{
		return _count == ch._count;
	}
};

class FileCompressHuffman
{
public:
	FileCompressHuffman();
	void CompressFile(const std::string& path);
	void UnCompressFile(const std::string& path);

private:
	void GenerateHuffmanCode(HuffmanTreeNode<CharInfo>* pRoot);  //����huffman����
	void WriteHead(FILE*  fOut, const std::string& filePostFix);
	std::string GetFilePostFix(const std::string& fileName);
	std::string GetFileNameOnly(const std::string& fileName);
	void ReadLine(FILE* fIn, std::string& strInfo);

private:
	std::vector<CharInfo> _fileInfo;
};
