#include<string.h>
#include"HashTable.hpp"

const USH HASH_BITS = 15;                  //哈希地址15位
const USH HASH_SIZE = (1 << HASH_BITS);    //哈希地址个数 32K
const USH HASH_MASK = HASH_SIZE - 1;       //防止溢出  低15位全1

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
	hashFunc(hashAddr, ch);//获取本次插入的哈希地址

	matchhead = _head[hashAddr];//获取上一次匹配的字符串头

	//将新的哈希地址插入链表
	_prev[pos&HASH_MASK] = _head[hashAddr];
	_head[hashAddr] = pos;
}
USH HashTable::GetNext(USH matchHead) {
	return _prev[matchHead&HASH_MASK];
}
void HashTable::Update() {
	for (size_t i = 0; i < WSIZE; ++i) {
		//更新head
		if (_head[i] > WSIZE)
			_head[i] -= WSIZE;
		else
			_head[i] = 0;
		//更新prev
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