#pragma once
#include "Defines.hpp"
template<typename T>
class LinkedList
{
private:
    template<typename T>
    struct Node {
        Node<T>(Node<T> *p, Node<T> *n, T d) : prev(p), next(n), data(d)
        {};
        Node<T>(Node<T> *p, Node<T> *n) : prev(p), next(n)
        {};
        T data;
        Node<T>* prev;
        Node<T>* next;
    };
public:
    LinkedList()
    {
        start = new Node<T>(nullptr, nullptr);
        end = new Node<T>(start, nullptr);
        start->next = end;
    }

    ~LinkedList()
    {
        delete start;
        delete end;
    }

    Node<T> * Begin() { 
        return start->next;
    }
    Node<T> * End() { return end; }

    void Combine(Node<T> * begins)
    {
        Node<T> * fbg = begins->prev ;
        T var (begins->data);
        var -= begins->data;
        if (begins->next == end)
        {
            return;
        }
        while (begins->data.CanCombine(begins->next->data) && begins->next != end)
        {
            var += begins->data;
            begins = begins->next;
            delete begins->prev;
        }
        fbg->next = new Node<T>(fbg, begins, var);
        begins->prev = fbg->next;
    }
    void InsertTo(Node<T> *node, T var)
    {
        node->next = new Node<T>(node, node->next, var);
        node->next->prev = node;
    }
    void Delete(Node<T> *n)
    {
        n->prev->next = n->next;
        n->next->prev = n->prev;
        delete n;
    }
    Node<T> * Insert(T var)
    {
        if (Begin() == End())       
        {
            start->next = new Node<T>(start, end, var);
            end->prev = start->next;
            return start->next;
        }
        auto it = Begin();
        while (it != End())
        {
            if (var < it->data)
            {
                it = new Node<T>(it->prev, it, var);
                it->prev->next = it;
                it->next->prev = it;
                break;
            }
            it = it->next;
        }
        if (it == end)
            return it->prev;
        return it;
    }
    void Clear()
    {
        auto it = Begin();
        Node <T> force;
        while (it != End())
        {
            force = it->next;
            delete it;
            it = force;
        }
    }

private:
    Node<T> *start;
    Node <T> *end;
};
