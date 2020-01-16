#pragma once
#pragma warning(disable:4996)
#include <iostream>
//using namespace std;
#include <queue>
#include <vector>
template<class W>   //W 代表节点的权值类型
class HuffManTreeNode
{
public:
	HuffManTreeNode(const W& weight =W()) // 构造，创建HuffMan结点
		:_pLeft(nullptr)
		,_pRight(nullptr)
		, _pParent(nullptr)
		, _weight(weight)
	{

	}

	HuffManTreeNode<W>* _pLeft; // 左指针
	HuffManTreeNode<W>* _pRight;// 右指针
	HuffManTreeNode<W>* _pParent;// 双亲指针
	W _weight; // 权值


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
		return pLeft->_weight > pRight->_weight; // 自定义类型需要重载
	}
};

template<class W>
class HuffmanTree // 创建HuffMan树
{
	typedef HuffManTreeNode<W> Node;   // 重命名

public:
	HuffmanTree()
		:_pRoot(nullptr)
	{

	}

	HuffmanTree(const std::vector<W> vweight, const W& invalid_weight) // 传入权值数组
		:_pRoot(nullptr)
	{
		CreateHuffManTree(vweight, invalid_weight);
	}

	~HuffmanTree()
	{
		_DestroyTree(_pRoot); // 销毁Huffman树

	}
	void CreateHuffManTree(const std::vector<W>& vWeight ,const W& invalid_weight) // 传入 权值， 无效权值invalid_weight ， 过滤传入权值为0得
	{
		// (1). 构建森林

		// 1、采用优先级队列，存放结点，而优先级队列，只需要存放这些结点的地址就行

		std::priority_queue<Node*, std::vector<Node*>, Greater<W>> pq; // 小堆
		for (auto e : vWeight) //
		{
			if (e == invalid_weight) // 若传入为无效权值，跳过
				continue;
			//Node(e)将权值存入结点，new 返回地址，将结点地址压入优先队列
			pq.push(new Node(e)); //优先对列，存放节点的地址
		}

		while (pq.size()>1)  //优先队列中存放超过1个结点地址
		{
			Node* pLeft = pq.top(); // 堆顶最小权值结点地址
			pq.pop();

			Node* pRight = pq.top();// 优先队列更新后，堆顶最小权值结点地址
			pq.pop();


			// 将 用所取出两个结点地址，获取权值，
			// 创建新的结点，将两个结点的权值之和，存在在 新结点中
			Node* pParent = new Node(pLeft->_weight + pRight->_weight);  // 注意：_weight 得 类型为 W ，可以时自定义类型或者内置类型
			
			// 并将新节点地址作为双亲结点，
			pParent->_pLeft = pLeft;
			pParent->_pRight = pRight;


			pLeft->_pParent = pParent; // 左子树的双亲
			pRight->_pParent = pParent;// 右子树的双亲
			// 将新地址节点压入
			pq.push(pParent);
		}

		//跳出while 循环，此时就只剩一棵树 ， 即 Huffman树  -- 构建成功
		_pRoot = pq.top();


	}
	
	Node* GetRoot()
	{
		return _pRoot;
	}

	void _DestroyTree(Node*& pRoot)//pRoot 为 _pRoot堆顶 的一份拷贝，因此需要用二级指针 或者 引用 才能将 栈顶_pRoot置空
	{
		if (pRoot) // 加入堆顶指针不为空,存在
		{
			// 递归销毁
			_DestroyTree(pRoot->_pLeft); //先消除左子树
			_DestroyTree(pRoot->_pRight); //在消除右子树
			delete pRoot;
			pRoot = nullptr;
		}
	}
private:
	Node* _pRoot;  //堆顶 

};


