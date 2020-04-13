#include<string.h>
#include"HashTable.hpp"

const USH HASH_BITS = 15;                  //��ϣ��ַ15λ
const USH HASH_SIZE = (1 << HASH_BITS);    //��ϣ��ַ���� 32K
const USH HASH_MASK = HASH_SIZE - 1;       //��ֹ���  ��15λȫ1

HashTable::HashTable(USH size)
:_prev(new USH[2 * size])
, _head(_prev + size)
{
	memset(_prev, 0, 2 * size * sizeof(USH));
}
HashTable::~HashTable() {
	delete[] _prev;
	_prev = nullptr;
}
void HashTable::Insert(USH& matchhead, UCH ch, USH pos, USH& hashAddr) {
	hashFunc(hashAddr, ch);//��ȡ���β���Ĺ�ϣ��ַ

	matchhead = _head[hashAddr];//��ȡ��һ��ƥ����ַ���ͷ

	//���µĹ�ϣ��ַ��������
	_prev[pos&HASH_MASK] = _head[hashAddr];
	_head[hashAddr] = pos;
}
USH HashTable::GetNext(USH matchHead) {
	return _prev[matchHead&HASH_MASK];
}
void HashTable::Update() {
	for (size_t i = 0; i < WSIZE; ++i) {
		//����head
		if (_head[i] > WSIZE)
			_head[i] -= WSIZE;
		else
			_head[i] = 0;
		//����prev
		if (_prev[i] > WSIZE)
			_prev[i] -= WSIZE;
		else
			_prev[i] = 0;
	}
}
void HashTable::hashFunc(USH& hashAddr, UCH ch) {
	hashAddr = (((hashAddr) << H_SHIFT()) ^ (ch))&HASH_MASK;
}
USH HashTable::H_SHIFT() {
	return (HASH_BITS + MIN_MATCH - 1) / MIN_MATCH;
}