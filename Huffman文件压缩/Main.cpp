#include "FileCompressHuffman.h"

int main()
{

	FileCompressHuffman hf;
	hf.CompressFile("1.txt");
	hf.UnCompressFile("1.gzip");
	return 0;
}