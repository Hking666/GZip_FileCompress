#define _CRT_SECURE_NO_WARNINGS 1
#include<iostream>
#include"LZ77.hpp"

const USH MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1; //要保证最后一次匹配,最大匹配长度258 
const USH MAX_DIST = WSIZE - MIN_LOOKAHEAD;    //最长匹配距离

LZ77::LZ77()
:_pWin(new UCH[WSIZE * 2])
,_ht(WSIZE)
{}
LZ77::~LZ77() {
	delete[] _pWin;
	_pWin = nullptr;
}
void LZ77::LZCompressFile(const std::string& fileName) {
	_fileName = fileName;
	FILE* fR = fopen(fileName.c_str(), "rb");
	if (!fR) {
		std::cout << "待压缩文件打开失败!" << std::endl;
		return;
	}
	//计算文件大小
	fseek(fR, 0, SEEK_END);
	ULL fileSize = ftell(fR);

	if (fileSize <= MIN_MATCH) {
		std::cout << "文件太小！不进行压缩！！" << std::endl;
		return;
	}
	//将文件指针置回起始位置
	fseek(fR, 0, SEEK_SET);
	//从压缩文件中读取一个缓冲区的数据到窗口中
	size_t lookAhead = fread(_pWin, sizeof(UCH), 2 * WSIZE, fR);

	//计算前两个字符的哈希地址
	USH hashAddr = 0;
	for (UCH i = 0; i < MIN_MATCH - 1; ++i) {
		_ht.hashFunc(hashAddr, _pWin[i]);
	}

	FILE* fW = fopen("LZ77.bin", "wb");//写压缩数据
	FILE* fWT = fopen("3.bin", "wb");//写数据的标记
	if (!fW || !fWT) {
		std::cout << "文件打开失败" << std::endl;
		return;
	}

	USH matchHead = 0;//匹配链的头
	USH curMatchLen = 0; //最长匹配链的长度
	USH curMatchDist = 0; //最长匹配链的距离
	USH start = 0;  //查找字符串在缓冲区的地址

	UCH chNum = 0;   //将要写入的标记
	UCH bitCount = 0; //记录 标记写了多少位
	while (lookAhead) {
		//1.将当前三个字符插入到哈希表中，并获取匹配链的头
		_ht.Insert(matchHead, _pWin[start + 2], start, hashAddr);

		curMatchLen = 0;
		curMatchDist = 0;
		//2.验证在查找缓冲区中是否找到匹配，如果有匹配，找最长匹配
		if (matchHead > 0) {
			//顺着匹配链找最长匹配,最终带出<长度，距离>对
			curMatchLen = LongestMatch(matchHead, curMatchDist, start);
		}
		//3.验证是否找到匹配
		if (curMatchLen < MIN_MATCH) {//找到
			//写原字符
			fputc(_pWin[start], fW);
			//写标记
			WriteFlag(fWT, chNum, bitCount, false);
			++start;
			--lookAhead;
		}
		else {           //未找到
			//写长度
			UCH chlen = curMatchLen - 3;
			fputc(chlen, fW);
			//写距离
			fwrite(&curMatchDist, sizeof(curMatchDist), 1, fW);
			//写标记
			WriteFlag(fWT, chNum, bitCount, true);

			lookAhead -= curMatchLen;
			//将已经匹配的字符串按照三个一组将其插入到哈希表中
			++start;   //第一个字符已经插入
			--curMatchLen;
			while (curMatchLen) {
				_ht.Insert(matchHead, _pWin[start + 2], start, hashAddr);
				++start;
				--curMatchLen;
			}
		}
		//检测先行缓冲区中剩余字符个数
		if (lookAhead <= MIN_LOOKAHEAD)
			fillWindow(start, fR, lookAhead);
	}
	//将标记位数不够八位的写入
	if (bitCount > 0 && bitCount < 8) {
		chNum <<= (8 - bitCount);
		fputc(chNum, fWT);
	}
	fclose(fWT);
	fclose(fR);
	//合并压缩数据文件和标记文件
	MergeFile(fW, fileSize);
	fclose(fW);
	//将用来保存标记信息的临时文件删除掉
	if (remove("3.bin") != 0) {
		std::cout << "3.bin删除失败" << std::endl;
	}
}
void LZ77::MergeFile(FILE* fW, ULL fileSize) {
	//将压缩数据文件和标记信息文件合并
	//读取标记信息文件中内容，然后将结果写入到压缩文件中
	FILE* fR = fopen("3.bin", "rb");
	UCH *buff = new UCH[1024];
	ULL rSize = 0;
	while (1) {
		size_t readSize = fread(buff, sizeof(UCH), 1024, fR);
		if (readSize == 0)
			break;
		rSize += readSize;
		fwrite(buff, sizeof(UCH), readSize, fW);
	}
	fwrite(&rSize, sizeof(rSize), 1, fW);
	fwrite(&fileSize, sizeof(fileSize), 1, fW);
	delete[] buff;
	fclose(fR);
}
void LZ77::fillWindow(USH& start, FILE* fR, size_t& readSize) {
	//start压缩已经进行到右窗，先行缓冲区剩余数据不够MIN_LOOKAHEAD
	if (start >= WSIZE) {
		//1.将右窗中的数据搬移到左窗
		memcpy(_pWin, _pWin + WSIZE, WSIZE);
		memset(_pWin + WSIZE, 0, WSIZE);
		start -= WSIZE;
		//2.更新哈希表
		_ht.Update();
		//3.向右窗中补充WSIZE个的待压缩数据
		if (!feof(fR))
			readSize += fread(_pWin + WSIZE, sizeof(UCH), WSIZE, fR);
	}
}
USH LZ77::LongestMatch(USH matchHead, USH& MatchDist, USH start) {     //找最长匹配
	USH curMatchLen = 0;
	USH maxMatchLen = 0;
	USH maxMatchHead = 0;
	UCH matchCount = 255;

	//在先行缓冲区中查找匹配时，不能太远即不能超过MAX_DIST
	USH limit = start > MAX_DIST ? start - MAX_DIST : 0;
	do {
		//最大匹配范围
		UCH* pStart = _pWin + start;
		UCH* pEnd = pStart + MAX_MATCH;
		//查找缓冲区匹配串的起始
		UCH* ptr = _pWin + matchHead;
		curMatchLen = 0;

		while (pStart < pEnd&&*pStart == *ptr) {
			++curMatchLen;
			++pStart;
			++ptr;
		}
		if (maxMatchLen < curMatchLen) {
			maxMatchLen = curMatchLen;
			maxMatchHead = matchHead;
		}
	} while ((matchHead = _ht.GetNext(matchHead)) > limit&&matchCount--);
	//获取最大匹配距离
	MatchDist = start - maxMatchHead;
	//获取最大匹配长度
	return maxMatchLen;
}
void LZ77::WriteFlag(FILE* file, UCH& chNum, UCH& bitCount, bool isLen) {
	chNum <<= 1;
	if (isLen)
		chNum |= 1;
	++bitCount;
	if (bitCount == 8) {
		fputc(chNum, file);
		bitCount = 0;
		chNum = 0;
	}
}
void LZ77::UnLZCompressFile(const std::string& fileName) {
	FILE* fR = fopen(fileName.c_str(), "rb");    //读取压缩数据
	FILE* fRT = fopen(fileName.c_str(), "rb");   //读取标记
	if (!fR || !fRT) {
		std::cout << "压缩文件打开失败！" << std::endl;
		return;
	}

	ULL fileSize = 0;   //读取压缩数据大小
	fseek(fRT, 0 - sizeof(fileSize), SEEK_END);
	fread(&fileSize, sizeof(fileSize), 1, fRT);

	ULL flagSize = 0;  //读取标记文件大小
	fseek(fRT, 0 - sizeof(fileSize)-sizeof(flagSize), SEEK_END);
	fread(&flagSize, sizeof(flagSize), 1, fRT);

	//将文件指针指向标记文件起始
	fseek(fRT, 0 - sizeof(fileSize)-sizeof(flagSize)-flagSize, SEEK_END);

	std::string newFile = "new" + fileName;
	FILE* fW = fopen(newFile.c_str(), "wb");  //将解压后的数据写入到新文件
	FILE* fWr = fopen(newFile.c_str(), "rb"); //读取新文件已写入的部分
	if (!fW || !fWr) {
		std::cout << "新文件打开/读取失败" << std::endl;
		return;
	}

	UCH chNum = 0;
	UCH bitCount = 0;
	ULL enCodeCount = 0;
	while (enCodeCount < fileSize) {
		//读取标记信息
		if (bitCount == 0) {
			chNum = fgetc(fRT);
			bitCount = 8;
		}
		if (chNum & 0x80) {//是长度数据
			//读取长度
			USH strLength = fgetc(fR) + 3;
			//读取距离
			USH strDist = 0;
			fread(&strDist, sizeof(strDist), 1, fR);

			//清空缓冲区
			fflush(fW);

			enCodeCount += strLength;

			fseek(fWr, 0 - strDist, SEEK_END);
			UCH ch = 0;
			while (strLength) {  //fR：读取前文匹配串中的内容
				ch = fgetc(fWr);
				fputc(ch, fW);
				//在还原长度距离对时，一定要清空缓冲区，否则可能会还原出错
				fflush(fW);
				--strLength;
			}
		}
		else {//原始字符
			UCH ch = fgetc(fR);
			fputc(ch, fW);
			fflush(fW);
			++enCodeCount;
		}
		chNum <<= 1;
		--bitCount;
	}
	fclose(fR);
	fclose(fRT);
	fclose(fW);
	fclose(fWr);
	if (remove(fileName.c_str()) != 0) {
		std::cout << fileName << "删除失败" << std::endl;
	}
}