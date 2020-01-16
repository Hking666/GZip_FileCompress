#pragma once
#pragma warning(disable:4996)
#include <iostream>
//using namespace std;
#include <queue>
#include <vector>
template<class W>   //W ����ڵ��Ȩֵ����
class HuffManTreeNode
{
public:
	HuffManTreeNode(const W& weight =W()) // ���죬����HuffMan���
		:_pLeft(nullptr)
		,_pRight(nullptr)
		, _pParent(nullptr)
		, _weight(weight)
	{

	}

	HuffManTreeNode<W>* _pLeft; // ��ָ��
	HuffManTreeNode<W>* _pRight;// ��ָ��
	HuffManTreeNode<W>* _pParent;// ˫��ָ��
	W _weight; // Ȩֵ


};

template <class W>
class Greater
{
	typedef HuffManTreeNode<W> Node;
public:
	Greater()
	{}

	bool operator()(const Node* pLeft, const Node* pRight)
	{
		return pLeft->_weight > pRight->_weight; // �Զ���������Ҫ����
	}
};

template<class W>
class HuffmanTree // ����HuffMan��
{
	typedef HuffManTreeNode<W> Node;   // ������

public:
	HuffmanTree()
		:_pRoot(nullptr)
	{

	}

	HuffmanTree(const std::vector<W> vweight, const W& invalid_weight) // ����Ȩֵ����
		:_pRoot(nullptr)
	{
		CreateHuffManTree(vweight, invalid_weight);
	}

	~HuffmanTree()
	{
		_DestroyTree(_pRoot); // ����Huffman��

	}
	void CreateHuffManTree(const std::vector<W>& vWeight ,const W& invalid_weight) // ���� Ȩֵ�� ��ЧȨֵinvalid_weight �� ���˴���ȨֵΪ0��
	{
		// (1). ����ɭ��

		// 1���������ȼ����У���Ž�㣬�����ȼ����У�ֻ��Ҫ�����Щ���ĵ�ַ����

		std::priority_queue<Node*, std::vector<Node*>, Greater<W>> pq; // С��
		for (auto e : vWeight) //
		{
			if (e == invalid_weight) // ������Ϊ��ЧȨֵ������
				continue;
			//Node(e)��Ȩֵ�����㣬new ���ص�ַ��������ַѹ�����ȶ���
			pq.push(new Node(e)); //���ȶ��У���Žڵ�ĵ�ַ
		}

		while (pq.size()>1)  //���ȶ����д�ų���1������ַ
		{
			Node* pLeft = pq.top(); // �Ѷ���СȨֵ����ַ
			pq.pop();

			Node* pRight = pq.top();// ���ȶ��и��º󣬶Ѷ���СȨֵ����ַ
			pq.pop();


			// �� ����ȡ����������ַ����ȡȨֵ��
			// �����µĽ�㣬����������Ȩֵ֮�ͣ������� �½����
			Node* pParent = new Node(pLeft->_weight + pRight->_weight);  // ע�⣺_weight �� ����Ϊ W ������ʱ�Զ������ͻ�����������
			
			// �����½ڵ��ַ��Ϊ˫�׽�㣬
			pParent->_pLeft = pLeft;
			pParent->_pRight = pRight;


			pLeft->_pParent = pParent; // ��������˫��
			pRight->_pParent = pParent;// ��������˫��
			// ���µ�ַ�ڵ�ѹ��
			pq.push(pParent);
		}

		//����while ѭ������ʱ��ֻʣһ���� �� �� Huffman��  -- �����ɹ�
		_pRoot = pq.top();


	}
	
	Node* GetRoot()
	{
		return _pRoot;
	}

	void _DestroyTree(Node*& pRoot)//pRoot Ϊ _pRoot�Ѷ� ��һ�ݿ����������Ҫ�ö���ָ�� ���� ���� ���ܽ� ջ��_pRoot�ÿ�
	{
		if (pRoot) // ����Ѷ�ָ�벻Ϊ��,����
		{
			// �ݹ�����
			_DestroyTree(pRoot->_pLeft); //������������
			_DestroyTree(pRoot->_pRight); //������������
			delete pRoot;
			pRoot = nullptr;
		}
	}
private:
	Node* _pRoot;  //�Ѷ� 

};


