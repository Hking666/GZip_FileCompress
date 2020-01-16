#pragma once
#pragma warning(disable:4996)
// ����Huffman��ѹ��
#include <string>
#include <assert.h>
#include <algorithm>
#include <functional> 
#include "Huffman.hpp"

// ͳ��Դ�ļ��ַ����ֵĴ���
struct CharInfo
{

	char _ch; // ������ַ�
	size_t _count; // �ַ����ֵĴ���
	std::string _strCode; // �ַ���Huffman����

	CharInfo(size_t count = 0)
		:_count(count)
	{
	}

	CharInfo operator+(const CharInfo& ch)const // ��������������� �� ����ֵҲ����Ϊ����
	{
		// ���ܷ���return ch._count + _count;
		//    
		return CharInfo(ch._count + _count); // ����һ�������ṹ�����
	}

	bool operator>(const CharInfo& ch)const 
	{
		return _count >ch._count; // ����һ�������ṹ�����
	}

	bool operator==(const CharInfo& ch)const //
	{
		return _count == ch._count; // ֻ��Ҫ�жϴ��������˵�����Ϊ0��
	}
};

class FileCompressHuffman // ����Huffman��ѹ��
{

public:
	FileCompressHuffman(); // ��ʼ��ͳ���ַ�����

	void CompressFile(const std::string& path); //ѹ���ļ���·��
	
	void UNCompressFile(const std::string& path); //��ѹ�ļ����·��

	
	
private:
	void GenerateHuffmanCode(HuffManTreeNode<CharInfo>* pRoot);// ��ȡÿ���ַ��ı���
	void WriteHead(FILE* fOut, const std::string& fileName); // ��ѹ���ļ����ͷ����Ϣ(�ļ���׺�������������ַ�����) ,filePostFix �ļ���׺
	std::string GetFilePostFix(const std::string&  fileName); //��ȡ�ļ��� 
private:
	std::vector<CharInfo> _fileInfo; // �ṹ������  ͳ�Ʊ��� �±��Ӧ �ַ����࣬�������ַ�����

};