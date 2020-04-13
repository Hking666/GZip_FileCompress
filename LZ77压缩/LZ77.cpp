#define _CRT_SECURE_NO_WARNINGS 1
#include<iostream>
#include"LZ77.hpp"

const USH MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1; //Ҫ��֤���һ��ƥ��,���ƥ�䳤��258 
const USH MAX_DIST = WSIZE - MIN_LOOKAHEAD;    //�ƥ�����

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
		std::cout << "��ѹ���ļ���ʧ��!" << std::endl;
		return;
	}
	//�����ļ���С
	fseek(fR, 0, SEEK_END);
	ULL fileSize = ftell(fR);

	if (fileSize <= MIN_MATCH) {
		std::cout << "�ļ�̫С��������ѹ������" << std::endl;
		return;
	}
	//���ļ�ָ���û���ʼλ��
	fseek(fR, 0, SEEK_SET);
	//��ѹ���ļ��ж�ȡһ�������������ݵ�������
	size_t lookAhead = fread(_pWin, sizeof(UCH), 2 * WSIZE, fR);

	//����ǰ�����ַ��Ĺ�ϣ��ַ
	USH hashAddr = 0;
	for (UCH i = 0; i < MIN_MATCH - 1; ++i) {
		_ht.hashFunc(hashAddr, _pWin[i]);
	}

	FILE* fW = fopen("LZ77.bin", "wb");//дѹ������
	FILE* fWT = fopen("3.bin", "wb");//д���ݵı��
	if (!fW || !fWT) {
		std::cout << "�ļ���ʧ��" << std::endl;
		return;
	}

	USH matchHead = 0;//ƥ������ͷ
	USH curMatchLen = 0; //�ƥ�����ĳ���
	USH curMatchDist = 0; //�ƥ�����ľ���
	USH start = 0;  //�����ַ����ڻ������ĵ�ַ

	UCH chNum = 0;   //��Ҫд��ı��
	UCH bitCount = 0; //��¼ ���д�˶���λ
	while (lookAhead) {
		//1.����ǰ�����ַ����뵽��ϣ���У�����ȡƥ������ͷ
		_ht.Insert(matchHead, _pWin[start + 2], start, hashAddr);

		curMatchLen = 0;
		curMatchDist = 0;
		//2.��֤�ڲ��һ��������Ƿ��ҵ�ƥ�䣬�����ƥ�䣬���ƥ��
		if (matchHead > 0) {
			//˳��ƥ�������ƥ��,���մ���<���ȣ�����>��
			curMatchLen = LongestMatch(matchHead, curMatchDist, start);
		}
		//3.��֤�Ƿ��ҵ�ƥ��
		if (curMatchLen < MIN_MATCH) {//�ҵ�
			//дԭ�ַ�
			fputc(_pWin[start], fW);
			//д���
			WriteFlag(fWT, chNum, bitCount, false);
			++start;
			--lookAhead;
		}
		else {           //δ�ҵ�
			//д����
			UCH chlen = curMatchLen - 3;
			fputc(chlen, fW);
			//д����
			fwrite(&curMatchDist, sizeof(curMatchDist), 1, fW);
			//д���
			WriteFlag(fWT, chNum, bitCount, true);

			lookAhead -= curMatchLen;
			//���Ѿ�ƥ����ַ�����������һ�齫����뵽��ϣ����
			++start;   //��һ���ַ��Ѿ�����
			--curMatchLen;
			while (curMatchLen) {
				_ht.Insert(matchHead, _pWin[start + 2], start, hashAddr);
				++start;
				--curMatchLen;
			}
		}
		//������л�������ʣ���ַ�����
		if (lookAhead <= MIN_LOOKAHEAD)
			fillWindow(start, fR, lookAhead);
	}
	//�����λ��������λ��д��
	if (bitCount > 0 && bitCount < 8) {
		chNum <<= (8 - bitCount);
		fputc(chNum, fWT);
	}
	fclose(fWT);
	fclose(fR);
	//�ϲ�ѹ�������ļ��ͱ���ļ�
	MergeFile(fW, fileSize);
	fclose(fW);
	//��������������Ϣ����ʱ�ļ�ɾ����
	if (remove("3.bin") != 0) {
		std::cout << "3.binɾ��ʧ��" << std::endl;
	}
}
void LZ77::MergeFile(FILE* fW, ULL fileSize) {
	//��ѹ�������ļ��ͱ����Ϣ�ļ��ϲ�
	//��ȡ�����Ϣ�ļ������ݣ�Ȼ�󽫽��д�뵽ѹ���ļ���
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
	//startѹ���Ѿ����е��Ҵ������л�����ʣ�����ݲ���MIN_LOOKAHEAD
	if (start >= WSIZE) {
		//1.���Ҵ��е����ݰ��Ƶ���
		memcpy(_pWin, _pWin + WSIZE, WSIZE);
		memset(_pWin + WSIZE, 0, WSIZE);
		start -= WSIZE;
		//2.���¹�ϣ��
		_ht.Update();
		//3.���Ҵ��в���WSIZE���Ĵ�ѹ������
		if (!feof(fR))
			readSize += fread(_pWin + WSIZE, sizeof(UCH), WSIZE, fR);
	}
}
USH LZ77::LongestMatch(USH matchHead, USH& MatchDist, USH start) {     //���ƥ��
	USH curMatchLen = 0;
	USH maxMatchLen = 0;
	USH maxMatchHead = 0;
	UCH matchCount = 255;

	//�����л������в���ƥ��ʱ������̫Զ�����ܳ���MAX_DIST
	USH limit = start > MAX_DIST ? start - MAX_DIST : 0;
	do {
		//���ƥ�䷶Χ
		UCH* pStart = _pWin + start;
		UCH* pEnd = pStart + MAX_MATCH;
		//���һ�����ƥ�䴮����ʼ
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
	//��ȡ���ƥ�����
	MatchDist = start - maxMatchHead;
	//��ȡ���ƥ�䳤��
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
	FILE* fR = fopen(fileName.c_str(), "rb");    //��ȡѹ������
	FILE* fRT = fopen(fileName.c_str(), "rb");   //��ȡ���
	if (!fR || !fRT) {
		std::cout << "ѹ���ļ���ʧ�ܣ�" << std::endl;
		return;
	}

	ULL fileSize = 0;   //��ȡѹ�����ݴ�С
	fseek(fRT, 0 - sizeof(fileSize), SEEK_END);
	fread(&fileSize, sizeof(fileSize), 1, fRT);

	ULL flagSize = 0;  //��ȡ����ļ���С
	fseek(fRT, 0 - sizeof(fileSize)-sizeof(flagSize), SEEK_END);
	fread(&flagSize, sizeof(flagSize), 1, fRT);

	//���ļ�ָ��ָ�����ļ���ʼ
	fseek(fRT, 0 - sizeof(fileSize)-sizeof(flagSize)-flagSize, SEEK_END);

	std::string newFile = "new" + fileName;
	FILE* fW = fopen(newFile.c_str(), "wb");  //����ѹ�������д�뵽���ļ�
	FILE* fWr = fopen(newFile.c_str(), "rb"); //��ȡ���ļ���д��Ĳ���
	if (!fW || !fWr) {
		std::cout << "���ļ���/��ȡʧ��" << std::endl;
		return;
	}

	UCH chNum = 0;
	UCH bitCount = 0;
	ULL enCodeCount = 0;
	while (enCodeCount < fileSize) {
		//��ȡ�����Ϣ
		if (bitCount == 0) {
			chNum = fgetc(fRT);
			bitCount = 8;
		}
		if (chNum & 0x80) {//�ǳ�������
			//��ȡ����
			USH strLength = fgetc(fR) + 3;
			//��ȡ����
			USH strDist = 0;
			fread(&strDist, sizeof(strDist), 1, fR);

			//��ջ�����
			fflush(fW);

			enCodeCount += strLength;

			fseek(fWr, 0 - strDist, SEEK_END);
			UCH ch = 0;
			while (strLength) {  //fR����ȡǰ��ƥ�䴮�е�����
				ch = fgetc(fWr);
				fputc(ch, fW);
				//�ڻ�ԭ���Ⱦ����ʱ��һ��Ҫ��ջ�������������ܻỹԭ����
				fflush(fW);
				--strLength;
			}
		}
		else {//ԭʼ�ַ�
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
		std::cout << fileName << "ɾ��ʧ��" << std::endl;
	}
}