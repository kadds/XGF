#pragma once
#include "Defines.hpp"
#include <functional>
template<typename T>
class StateList
{
public:
	template<typename  T>
	class Node
	{
	public:
		Node<T> * next;
		T val;
		Node(Node<T> * nextNode, T v) :next(nextNode), val(v) {}
	};
	StateList()
	{
		startA = new Node<T>(nullptr, nullptr);
		startB = new Node<T>(nullptr, nullptr);
	}
	~StateList()
	{
		delete startA;
		delete startB;
	}
	void PushBackToA(T val) 
	{
		startA->next = new Node<T>(startA->next, val);
	}
	void PushBackToB(T val)
	{
		startB->next = new Node<T>(startB->next, val);
	}
	Node<T> *GetBackNodeInA()
	{
		return startA->next;
	}
	T GetBackInA()
	{
		return startA->next->val;
	}
	Node<T> *GetBackNodeInB()
	{
		return startB->next;
	}
	T GetBackInB()
	{
		return startB->next->val;
	}
	void PutAToB()
	{
		Node<T> * node = startA->next;
		startA->next = node->next;
		node->next = startB->next;
		startB->next = node;
	}
	void PutBToA()
	{
		Node<T> * node = startB->next;
		startB->next = node->next;
		node->next = startA->next;
		startA->next = node;
	}

	void PutAToB(T t)
	{
		Node<T> * node = startA->next;
		Node<T> * nodePrev = startA;
		while (node != nullptr)
		{
			if (node == t)
			{
				nodePrev->next = node->next;
				node->next = startB->next;
				startB->next = node;
				break;
			}
			node = node->next;
			nodePrev = nodePrev->next;
		}
	}
	void PutBToA( T t)
	{
		Node<T> * node = startB->next;
		Node<T> * nodePrev = startB;
		while (node != nullptr)
		{
			if (node->val == t)
			{
				nodePrev->next = node->next;
				node->next = startA->next;
				startA->next = node;
				break;
			}
			node = node->next;
			nodePrev = nodePrev->next;
		}
	}
	bool EmptyInA()
	{
		return startA->next == nullptr;
	}
	bool EmptyInB()
	{
		return startB->next == nullptr;
	}
	size_t GetSizeA()
	{
		Node<T> * node = startA->next;
		size_t i = 0;
		while (node != nullptr)
		{
			i++;
			node = node->next;
		}
		return i;
	}
	size_t GetSizeB()
	{
		Node<T> * node = startB->next;
		size_t i = 0;
		while (node != nullptr)
		{
			i++;
			node = node->next;
		}
		return i;
	}
	void DelAll()
	{
		Node<T> * cnode;
		Node<T> * node = startA->next;
		while (node != nullptr)
		{
			cnode = node->next;
			delete node;
			node = cnode;
			
		}
		node = startB->next;
		while (node != nullptr)
		{
			cnode = node->next;
			delete node;
			node = cnode;
		}
	}
	void For_eachInA(std::function<void(T)> fun)
	{
		Node<T> * node = startA->next;
		while (node != nullptr)
		{
			fun(node->val);
			node = node->next;
		}
	}
	void For_eachInB(std::function<void(T)> fun)
	{
		Node<T> * node = startB->next;
		while (node != nullptr)
		{
			fun(node->val);
			node = node->next;
		}
	}
private:
	Node<T> * startA;
	Node<T> * startB;
};

