#pragma once
#include<queue>
#include<vector>
#include<stack>

struct HuffManTreeNode {
	HuffManTreeNode(unsigned long long weight, unsigned char ch = 0)
	:pLeft_(nullptr)
	, pRight_(nullptr)
	, Weight_(weight)
	, Ch_(ch)
	{}
	HuffManTreeNode *pLeft_;
	HuffManTreeNode *pRight_;
	unsigned long long Weight_;                    //Ȩֵ
	unsigned char Ch_;              //��ѹ���ַ�
};

class Less {            //ʹ��С��
public:
	bool operator()(const HuffManTreeNode* pLeft, const HuffManTreeNode* pRight) {
		return pLeft->Weight_ > pRight->Weight_;
	}
};

class HuffmanTree {
	typedef HuffManTreeNode Node;
	typedef HuffManTreeNode* PNode;
public:
	HuffmanTree(const std::vector<int> arr)
		:pRoot_(nullptr)
	{
		CreateHuffmanTree(arr);
	}
	~HuffmanTree() {
		std::stack<PNode> sa;
		PNode ptr = pRoot_;
		while (!sa.empty() || ptr) {
			while (ptr) {
				sa.push(ptr);
				ptr = ptr->pLeft_;
			}
			PNode del = sa.top();
			sa.pop();
			ptr = del->pRight_;
			delete del;
		}
	}
	PNode GetRoot() {
		return pRoot_;
	}
private:
	void CreateHuffmanTree(const std::vector<int> count) {
		//���ȶ��У�С����
		std::priority_queue<PNode, std::vector<PNode>, Less> que;
		for (int i = 0; i < 256; ++i) {
			if (count[i] > 0) { //ɸѡ���ֹ����ַ���ѹ�����
				que.push(new Node(count[i], i));
			}
		}
		while (que.size() > 1) {
			PNode left = que.top();
			que.pop();
			PNode right = que.top();
			que.pop();
			PNode newNode = new Node(left->Weight_ + right->Weight_);
			newNode->pLeft_ = left;
			newNode->pRight_ = right;
			que.push(newNode);
		}
		pRoot_ = que.top();
	}
private:
	Node* pRoot_;
};