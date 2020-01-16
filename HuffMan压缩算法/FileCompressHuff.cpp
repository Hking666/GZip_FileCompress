#include "FileCompressHuff.h"



FileCompressHuffman::FileCompressHuffman() // 构造函数，初始化统计字符数组
{
	_fileInfo.resize(256);
	for (int i = 0; i < 256; ++i)
	{
		_fileInfo[i]._ch = i;
		_fileInfo[i]._count = 0;
	}


}


void FileCompressHuffman::CompressFile(const std::string& path)//压缩文件路径
{
	// 1.统计源文件字符出现的次数

			// 1.1 打开文件
	FILE* fIn = fopen(path.c_str(),"rb");// 

	if (nullptr == fIn)
	{
		assert(false);// 文件打开失败
		return;
	}	
			//1.2 文件打开成功

	char* pReadBuff = new  char[1024]; //开辟1024字节缓存区，供每次读取1k数据

	// 读取数据保存至pReadBuff,一个字符1字节，每次读取1024个，从fIn文件指针中读取；
	int readSize = 0;
	while (true)
	{
		readSize = fread(pReadBuff, 1, 1024, fIn);

		if (0 == readSize) // 如果读取到的字符为0，则已经读完了
			break;

		for (int i = 0; i < readSize; ++i)
		{
			//pReadBuff[i] --- 缓冲区第i个字符
			_fileInfo[pReadBuff[i]]._count++; // 缓冲区第i个字符，对应得下标

		}

		
	}

	fseek(fIn, 0, SEEK_SET); // 将fIn文件指针,移到起始读的位置，后面改写

	//2、以字符出现的次数为权值创建huffman树
	HuffmanTree<CharInfo> t(_fileInfo, CharInfo()); //_fileInfo 记录字符数量得 vector

	//3、获取每个字符的编码

	GenerateHuffmanCode(t.GetRoot());

	//4、用获取到的编码重新改写源文件

	FILE* fOut = fopen("2.txt","w"); // 以只写形式，写出改写后的文件
	if (nullptr == fOut)
	{
		assert(false);
		return;
	}
	//  4.1  先向文件中添加 文件头  （文件名 ， 行数 ，次数）等信息

	WriteHead(fOut,path); //path文件路径，


	//  4.2 添加编码

	char ch = 0; // 替换变量
	int bitCount = 0; // 比特位计数
	while (true)
	{
		readSize = fread(pReadBuff,1,1024,fIn);
		if ( 0 == readSize)
		{
			break;
		}

		
		//4.1 根据字符编码对读取到的内容进行重写 , 用Huffman编码进行字符替换
		for (size_t i = 0; i < readSize; ++i)
		{
			std::string strCode = _fileInfo[pReadBuff[i]]._strCode ; // 用Huffman编码进行字符替换


			for (size_t j = 0; j < strCode.size(); ++j)
			{
				ch <<= 1; // 只有 ++ -- 改变原变量

				if ('1' == strCode[j])
					ch |= 1;
				
				bitCount++;

				if (8 == bitCount)  // 最后一次，可能不够8个比特位
				{
					fputc(ch, fOut);   //将 字节 写入fOu->文件
					bitCount = 0;
					ch = 0;
				}

			}
		}


	}

	if (bitCount < 8)
	{
		ch << (8 - bitCount);  //因为是左移,所有不足8位，要将后面的往前移到第一个bit位
		fputc(ch,fOut);
	}

	delete[] pReadBuff;
	fclose(fIn);
	fclose(fOut);
}


void FileCompressHuffman::GenerateHuffmanCode(HuffManTreeNode<CharInfo>* pRoot)
{
	if (pRoot == nullptr)
		return;

	GenerateHuffmanCode(pRoot->_pLeft);// 递归到叶子节点
	GenerateHuffmanCode(pRoot->_pRight);

	if (nullptr == pRoot->_pLeft && nullptr == pRoot->_pRight) // 叶子结点
	{
		std::string strCode;
		HuffManTreeNode<CharInfo>* pCur = pRoot; // 保存叶子节点
		HuffManTreeNode<CharInfo>* pParent = pRoot->_pParent; // 保存叶子节点双亲

		while (pParent)
		{
			if (pParent->_pLeft == pCur)
			{
				strCode += '0'; //
			}
			else
			{
				strCode += '1';
			}

			pCur = pParent;
			pParent = pCur->_pParent;
		}

		reverse(strCode.begin(),strCode.end());

		_fileInfo[pRoot->_weight._ch]._strCode = strCode;
	}


}

// 2.txt
//f:\123\2.txt
std::string FileCompressHuffman::GetFilePostFix(const std::string&  fileName) //获取文件名 
{

	return fileName.substr(fileName.rfind('.')); // 截取文件名 .文件类型
}

void  FileCompressHuffman::WriteHead(FILE* fOut, const std::string& fileName) //// 对压缩文件添加头部信息(文件后缀，编码行数，字符次数)
{

	assert(fOut);

	std::string  strHead;
	strHead += GetFilePostFix( fileName);;  // （1）向文件头添加后缀
	strHead += '\n';
	// 1、写文件的后缀
	//fwrite(filePostFix.c_str(), 1, filePostFix.size(), fOut);  //将 filePostFix.c_str() 通过fOut写入文件，每次写1字节

	// 写行数
	size_t lineCount = 0; // huffman编码的行数，// 统计待写入的  行数
	std::string strChCount; //记录各个字符及出现的次数，换行信息，// 统计待写入的  次数



	char szValue[32] = { 0 };
	for (int i = 0; i < 256;++i) // 统计每个出现不为0次数的字符，及其次数
	{
		CharInfo& charInfo = _fileInfo[i];
		if (_fileInfo[i]._count) //_fileInfo[i]._count 为 size_t类型
		{
			lineCount++;  // 统计待写入的  行数

			strChCount += charInfo._ch; // 对应字符

			strChCount += ':'; // 字符与字符次数之间用冒号区分

			//itoa(charInfo._count,szValue,10); // 将charInfo._count转化为字符类型，存入字符串strChCount中
			strChCount += _itoa(charInfo._count, szValue, 10); // 字符出现次数 ， 需要将size_t 转化为 string ,存入字符串中
			strChCount += '\n'; // 换行


		}
	}

	
	//2、 将行数转化为 字符型 ， 写入行数
	// itoa(lineCount,szValue,10); // 将行数转化为 字符型

	strHead += _itoa(lineCount, szValue, 10); // （2）向文件头 添加  行数
	strHead += '\n';

	strHead += strChCount; //(3) 向文件头添加 字符次数信息
	//将头写入文件
	fwrite(strHead.c_str() , 1 , strHead.size(),fOut);

	// 写入字符次数信息
}