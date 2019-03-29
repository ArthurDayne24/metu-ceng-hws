#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include <iostream>
#include <cstddef>
#include <stdexcept>
#include "Node.hpp"

using namespace std;

template <class T> 
class LinkedList {
    private:
        /* pointer to the first node */
        Node<T>* front;
        /* pointer to the last node */
        Node<T>* back;

    public:
        /*** Do not modify the interface ***/

        LinkedList();
        LinkedList(const LinkedList<T>& ll);
        LinkedList<T>& operator=(const LinkedList<T>& ll);
        ~LinkedList();

        /* returns the first node of the linked list */
        Node<T>& getFront() const;
        /* returns the last node of the linked list */
        Node<T>& getBack() const;
        /* returns the node in given "pos"ition of the linked list */
        Node<T>& getNodeAt(int pos) const;
        /* returns the pointer of the node in given 
           "pos"ition of the linked list */
        Node<T>* getNodePtrAt(int pos) const;
        
        /* inserts a new node containing "data" 
           after the node "prev" 
           */
        void insert(Node<T>* prev, const T& data);
        /* inserts a new node containing "data" 
           at "pos"ition in the linked list 
           */
        void insertAt(int pos, const T& data);
        /* erases the given "node" from the linked list */
        void erase(Node<T>* node);
        /* erases the node in given "pos"ition from the linked list */
        void eraseFrom(int pos);
        /* clears the contents of the linked list */
        void clear();

        /* inserts a new node containing "data" 
           to the front of the linked list 
           */
        void pushFront(const T& data);
        /* inserts a new node containing "data" 
           to the back of the linked list
           */
        void pushBack(const T& data);

        /* removes the first node */
        void popFront();
        /* removes the last node */
        void popBack();

        /* returns true if the list is empty, false otherwise */
        bool isEmpty() const;
        /* returns the number of items in the list */
        size_t getSize() const;
        /* prints the contents of the linked list 
           one node data per line
           assumes the objects in the node have operator<< overloaded 
           */
        void print() const;

        /*** Do not modify the interface ***/
};

/* TO-DO: method implementations here */

template <class T> 
LinkedList<T>:: LinkedList()
{//inline initiation tarzı bir sey vardi?
	front=back=NULL;
}	 	  	  	 	   	     	    	 	

template <class T> 
LinkedList<T>:: LinkedList(const LinkedList<T>& ll)
{//ne durumda private'a erisim var?
	//*this=LinkedList(); ?
	front=back=NULL;
	
	if(!ll.isEmpty())
	{	
		//initilaize front node!
		pushFront(ll.getFront().getData());
		//now insert other nodes if any
		Node<T>* locater=ll.getFront().getNext();
		while(locater!=NULL)
		{
			pushBack(locater->getData());
			locater=locater->getNext();
		}
	}
}

template <class T> 
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& ll)
{
	//!!!
	clear();
	
	if(!ll.isEmpty() && this!=&ll)
	{
		//initilaize front node!
		pushFront(ll.getFront().getData());
		//now insert other nodes if any
		Node<T>* locater=ll.getFront().getNext();
		while(locater!=NULL)
		{
			pushBack(locater->getData());
			locater=locater->getNext();
		}	 	  	  	 	   	     	    	 	
	}
}

template <class T> 
LinkedList<T>:: ~LinkedList(){clear();}

template <class T> 
 bool LinkedList<T>::isEmpty() const{return front==NULL;}

template <class T> 
Node<T>& LinkedList<T>::getFront() const
{	
	if(isEmpty())
		throw runtime_error("LinkedList::getFront() error!");
	else
		return *front;
}

template <class T> 
Node<T>& LinkedList<T>::getBack() const
{
	if(isEmpty())
		throw runtime_error("LinkedList::getBack() error!");
	else
		return *back;
}

template <class T> 
Node<T>& LinkedList<T>::getNodeAt(int pos) const
{
	if(pos<0 || pos>=getSize())
		throw runtime_error("LinkedList::getNodeAt() error!");
		
	Node<T>* locater=front;
	while(pos--)
		locater=locater->getNext();
		
	return *locater;
}	 	  	  	 	   	     	    	 	

template <class T> 
Node<T>* LinkedList<T>::getNodePtrAt(int pos) const
{
	if(pos<0 || pos>=getSize())
		throw runtime_error("LinkedList::getNodePtrAt() error!");
		
	Node<T>* locater=front;
	while(pos--)
		locater=locater->getNext();
		
	return locater;
}

template <class T> 
void LinkedList<T>::insert(Node<T>* prev, const T& data)
{
    Node<T>* thing=new Node<T>(data);
	//it is the first time!
	if(isEmpty())
	{	
		back=thing;
		front=thing;
		front->setNext(NULL);
		front->setPrev(NULL);
    }
    
	else//there exists at least one element
	{
		if(prev==NULL)
		{
			thing->setNext(front);
			thing->setPrev(NULL);
			front->setPrev(thing);
			front=thing;
		}	 	  	  	 	   	     	    	 	
		else//(prev!=NULL)
		{
			if(prev->getNext()==NULL && prev->getPrev()==NULL)
			{
				prev->setNext(thing);
				thing->setPrev(front);
				thing->setNext(NULL);
				back=thing;
			}
			else if(prev->getNext()==NULL && prev->getPrev()!=NULL)
			{
				prev->setNext(thing);
				thing->setPrev(prev);
				thing->setNext(NULL);
				back=thing;
			}
			else//(prev->getNext()!=NULL)
			{	
				thing->setNext(prev->getNext());
				thing->setPrev(prev);
				prev->setNext(thing);
				thing->getNext()->setPrev(thing);
			}	
		}
	}
}

template <class T> 
void LinkedList<T>::insertAt(int pos, const T& data)
{
	if(pos<0 || pos>getSize())
		throw runtime_error("LinkedList::insertAt() error!");
	else if(pos==getSize())
		pushBack(data);
	else		
		insert(getNodeAt(pos).getPrev(), data);
}	 	  	  	 	   	     	    	 	

template <class T> 
void LinkedList<T>::erase(Node<T>* node)
{

	if(node->getNext()==NULL && node->getPrev()==NULL)
	{
		back=front=NULL;
		node->setNext(NULL);
		node->setPrev(NULL);
		delete node;	
	}

	//it is front
	else if(node->getNext()!=NULL && node->getPrev()==NULL)
	{
		//front is shifted
		front=front->getNext();
		front->setPrev(NULL);
		//for safety
		node->setNext(NULL);
		delete node;
	}

	//it is back
	else if(node->getNext()==NULL && node->getPrev()!=NULL)
	{
		//back is shifted
		back=back->getPrev();
		back->setNext(NULL);
		//for safety
		node->setPrev(NULL);
		delete node;
	}

	//it is the only node
	else//if(node->getNext()!=NULL && node->getPrev()!=NULL)
	{	 	  	  	 	   	     	    	 	
		node->getPrev()->setNext(node->getNext());
		node->getNext()->setPrev(node->getPrev());
		//for safety
		node->setNext(NULL);
		node->setPrev(NULL);
		delete node;
	}

}

template <class T> 
void LinkedList<T>::eraseFrom(int pos)
{	
	if(pos<0 || pos>=getSize())
		throw runtime_error("LinkedList::eraseFrom() error!");
	else
		erase(getNodePtrAt(pos));
}

template <class T> 
void LinkedList<T>::clear()
{	
	while(!isEmpty())
		popFront();
}

template <class T> 
void LinkedList<T>::pushFront(const T& data)
{
	insert(NULL, data);
}

template <class T> 
void LinkedList<T>::pushBack(const T& data){insert(back, data);}
	
template <class T> 
void LinkedList<T>::popFront(){erase(front);}	 	  	  	 	   	     	    	 	

template <class T> 
void LinkedList<T>::popBack(){erase(back);}

template <class T> 
size_t LinkedList<T>::getSize() const
{
	size_t size=0;
	if(!isEmpty())
	{
		//What if size declared here tried to be returned _end_?
		Node<T>* locater=front;

		while(locater!=back)
		{
			size++;
			locater=locater->getNext();
		}

		size++;
	}

	return size;
}

template <class T> 
void LinkedList<T>::print() const
{
	if(!isEmpty())
	{
		Node<T>* locater=front;

		while(locater!=back)
		{
			cout<<locater->getData()<<endl;
			locater=locater->getNext();
		}	 	  	  	 	   	     	    	 	

		cout<<back->getData()<<endl;
	}
}
#endif