#pragma once
#include "Defines.hpp"
#include <mutex>
#include <functional>
template<typename T>
class StateList
{
private:
	std::mutex mutex;
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
		std::lock_guard<std::mutex> lock(mutex);
		startA->next = new Node<T>(startA->next, val);
	}
	void PushBackToB(T val)
	{
		std::lock_guard<std::mutex> lock(mutex);
		startB->next = new Node<T>(startB->next, val);
	}
	Node<T> *GetBackNodeInA()
	{
		std::lock_guard<std::mutex> lock(mutex);
		return startA->next;
	}
	T GetBackInA()
	{
		std::lock_guard<std::mutex> lock(mutex);
		return startA->next->val;
	}
	Node<T> *GetBackNodeInB()
	{
		std::lock_guard<std::mutex> lock(mutex);
		return startB->next;
	}
	T GetBackInB()
	{
		std::lock_guard<std::mutex> lock(mutex);
		return startB->next->val;
	}
	void PutAToB()
	{
		std::lock_guard<std::mutex> lock(mutex);
		Node<T> * node = startA->next;
		startA->next = node->next;
		node->next = startB->next;
		startB->next = node;
	}
	void PutBToA()
	{
		std::lock_guard<std::mutex> lock(mutex);
		Node<T> * node = startB->next;
		startB->next = node->next;
		node->next = startA->next;
		startA->next = node;
	}

	void PutAToB(T t)
	{
		std::lock_guard<std::mutex> lock(mutex);
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
		std::lock_guard<std::mutex> lock(mutex);
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
		std::lock_guard<std::mutex> lock(mutex);
		return startA->next == nullptr;
	}
	bool EmptyInB()
	{
		std::lock_guard<std::mutex> lock(mutex);
		return startB->next == nullptr;
	}
	size_t GetSizeA()
	{
		std::lock_guard<std::mutex> lock(mutex);
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
		std::lock_guard<std::mutex> lock(mutex);
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
		std::lock_guard<std::mutex> lock(mutex);
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
		std::lock_guard<std::mutex> lock(mutex);
		Node<T> * node = startA->next;
		while (node != nullptr)
		{
			fun(node->val);
			node = node->next;
		}
	}
	void For_eachInB(std::function<void(T)> fun)
	{
		std::lock_guard<std::mutex> lock(mutex);
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

