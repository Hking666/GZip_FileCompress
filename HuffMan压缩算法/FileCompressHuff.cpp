#include "FileCompressHuff.h"



FileCompressHuffman::FileCompressHuffman() // ���캯������ʼ��ͳ���ַ�����
{
	_fileInfo.resize(256);
	for (int i = 0; i < 256; ++i)
	{
		_fileInfo[i]._ch = i;
		_fileInfo[i]._count = 0;
	}


}


void FileCompressHuffman::CompressFile(const std::string& path)//ѹ���ļ�·��
{
	// 1.ͳ��Դ�ļ��ַ����ֵĴ���

			// 1.1 ���ļ�
	FILE* fIn = fopen(path.c_str(),"rb");// 

	if (nullptr == fIn)
	{
		assert(false);// �ļ���ʧ��
		return;
	}	
			//1.2 �ļ��򿪳ɹ�

	char* pReadBuff = new  char[1024]; //����1024�ֽڻ���������ÿ�ζ�ȡ1k����

	// ��ȡ���ݱ�����pReadBuff,һ���ַ�1�ֽڣ�ÿ�ζ�ȡ1024������fIn�ļ�ָ���ж�ȡ��
	int readSize = 0;
	while (true)
	{
		readSize = fread(pReadBuff, 1, 1024, fIn);

		if (0 == readSize) // �����ȡ�����ַ�Ϊ0�����Ѿ�������
			break;

		for (int i = 0; i < readSize; ++i)
		{
			//pReadBuff[i] --- ��������i���ַ�
			_fileInfo[pReadBuff[i]]._count++; // ��������i���ַ�����Ӧ���±�

		}

		
	}

	fseek(fIn, 0, SEEK_SET); // ��fIn�ļ�ָ��,�Ƶ���ʼ����λ�ã������д

	//2�����ַ����ֵĴ���ΪȨֵ����huffman��
	HuffmanTree<CharInfo> t(_fileInfo, CharInfo()); //_fileInfo ��¼�ַ������� vector

	//3����ȡÿ���ַ��ı���

	GenerateHuffmanCode(t.GetRoot());

	//4���û�ȡ���ı������¸�дԴ�ļ�

	FILE* fOut = fopen("2.txt","w"); // ��ֻд��ʽ��д����д����ļ�
	if (nullptr == fOut)
	{
		assert(false);
		return;
	}
	//  4.1  �����ļ������ �ļ�ͷ  ���ļ��� �� ���� ������������Ϣ

	WriteHead(fOut,path); //path�ļ�·����


	//  4.2 ��ӱ���

	char ch = 0; // �滻����
	int bitCount = 0; // ����λ����
	while (true)
	{
		readSize = fread(pReadBuff,1,1024,fIn);
		if ( 0 == readSize)
		{
			break;
		}

		
		//4.1 �����ַ�����Զ�ȡ�������ݽ�����д , ��Huffman��������ַ��滻
		for (size_t i = 0; i < readSize; ++i)
		{
			std::string strCode = _fileInfo[pReadBuff[i]]._strCode ; // ��Huffman��������ַ��滻


			for (size_t j = 0; j < strCode.size(); ++j)
			{
				ch <<= 1; // ֻ�� ++ -- �ı�ԭ����

				if ('1' == strCode[j])
					ch |= 1;
				
				bitCount++;

				if (8 == bitCount)  // ���һ�Σ����ܲ���8������λ
				{
					fputc(ch, fOut);   //�� �ֽ� д��fOu->�ļ�
					bitCount = 0;
					ch = 0;
				}

			}
		}


	}

	if (bitCount < 8)
	{
		ch << (8 - bitCount);  //��Ϊ������,���в���8λ��Ҫ���������ǰ�Ƶ���һ��bitλ
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

	GenerateHuffmanCode(pRoot->_pLeft);// �ݹ鵽Ҷ�ӽڵ�
	GenerateHuffmanCode(pRoot->_pRight);

	if (nullptr == pRoot->_pLeft && nullptr == pRoot->_pRight) // Ҷ�ӽ��
	{
		std::string strCode;
		HuffManTreeNode<CharInfo>* pCur = pRoot; // ����Ҷ�ӽڵ�
		HuffManTreeNode<CharInfo>* pParent = pRoot->_pParent; // ����Ҷ�ӽڵ�˫��

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
std::string FileCompressHuffman::GetFilePostFix(const std::string&  fileName) //��ȡ�ļ��� 
{

	return fileName.substr(fileName.rfind('.')); // ��ȡ�ļ��� .�ļ�����
}

void  FileCompressHuffman::WriteHead(FILE* fOut, const std::string& fileName) //// ��ѹ���ļ����ͷ����Ϣ(�ļ���׺�������������ַ�����)
{

	assert(fOut);

	std::string  strHead;
	strHead += GetFilePostFix( fileName);;  // ��1�����ļ�ͷ��Ӻ�׺
	strHead += '\n';
	// 1��д�ļ��ĺ�׺
	//fwrite(filePostFix.c_str(), 1, filePostFix.size(), fOut);  //�� filePostFix.c_str() ͨ��fOutд���ļ���ÿ��д1�ֽ�

	// д����
	size_t lineCount = 0; // huffman�����������// ͳ�ƴ�д���  ����
	std::string strChCount; //��¼�����ַ������ֵĴ�����������Ϣ��// ͳ�ƴ�д���  ����



	char szValue[32] = { 0 };
	for (int i = 0; i < 256;++i) // ͳ��ÿ�����ֲ�Ϊ0�������ַ����������
	{
		CharInfo& charInfo = _fileInfo[i];
		if (_fileInfo[i]._count) //_fileInfo[i]._count Ϊ size_t����
		{
			lineCount++;  // ͳ�ƴ�д���  ����

			strChCount += charInfo._ch; // ��Ӧ�ַ�

			strChCount += ':'; // �ַ����ַ�����֮����ð������

			//itoa(charInfo._count,szValue,10); // ��charInfo._countת��Ϊ�ַ����ͣ������ַ���strChCount��
			strChCount += _itoa(charInfo._count, szValue, 10); // �ַ����ִ��� �� ��Ҫ��size_t ת��Ϊ string ,�����ַ�����
			strChCount += '\n'; // ����


		}
	}

	
	//2�� ������ת��Ϊ �ַ��� �� д������
	// itoa(lineCount,szValue,10); // ������ת��Ϊ �ַ���

	strHead += _itoa(lineCount, szValue, 10); // ��2�����ļ�ͷ ���  ����
	strHead += '\n';

	strHead += strChCount; //(3) ���ļ�ͷ��� �ַ�������Ϣ
	//��ͷд���ļ�
	fwrite(strHead.c_str() , 1 , strHead.size(),fOut);

	// д���ַ�������Ϣ
}