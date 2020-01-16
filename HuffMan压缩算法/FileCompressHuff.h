#pragma once
#pragma warning(disable:4996)
// 基于Huffman的压缩
#include <string>
#include <assert.h>
#include <algorithm>
#include <functional> 
#include "Huffman.hpp"

// 统计源文件字符出现的次数
struct CharInfo
{

	char _ch; // 具体的字符
	size_t _count; // 字符出现的次数
	std::string _strCode; // 字符的Huffman编码

	CharInfo(size_t count = 0)
		:_count(count)
	{
	}

	CharInfo operator+(const CharInfo& ch)const // 重载两个对象相加 ， 返回值也必须为对象
	{
		// 不能返回return ch._count + _count;
		//    
		return CharInfo(ch._count + _count); // 返回一个无名结构体对象
	}

	bool operator>(const CharInfo& ch)const 
	{
		return _count >ch._count; // 返回一个无名结构体对象
	}

	bool operator==(const CharInfo& ch)const //
	{
		return _count == ch._count; // 只需要判断次数，过滤掉次数为0的
	}
};

class FileCompressHuffman // 基于Huffman的压缩
{

public:
	FileCompressHuffman(); // 初始化统计字符数组

	void CompressFile(const std::string& path); //压缩文件的路径
	
	void UNCompressFile(const std::string& path); //解压文件存放路径

	
	
private:
	void GenerateHuffmanCode(HuffManTreeNode<CharInfo>* pRoot);// 获取每个字符的编码
	void WriteHead(FILE* fOut, const std::string& fileName); // 对压缩文件添加头部信息(文件后缀，编码行数，字符次数) ,filePostFix 文件后缀
	std::string GetFilePostFix(const std::string&  fileName); //获取文件名 
private:
	std::vector<CharInfo> _fileInfo; // 结构体数组  统计保存 下标对应 字符种类，及各种字符数量

};