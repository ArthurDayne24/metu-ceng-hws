#ifndef _NODE_H_
#define _NODE_H_

#include <iostream>
#include <iomanip>

using namespace std;

/* forward declerations for overloading operator<< in template*/
template<class T>
class Node;

template<class T>
ostream &operator<<(ostream &out, const Node<T>& n);

/* node class for linked list */
template<class T>
class Node {

    private:
        Node<T> *prev;
        Node<T> *next;
        T data;

    public:
        /*** Do not modify the interface ***/
        Node();
        Node(const T& d);

        Node<T>* getNext() const;
        Node<T>* getPrev() const;
        T getData() const;
        T* getDataPtr();
        
        void setNext(Node<T> *newNext); 
        void setPrev(Node<T> *newPrev);
        void setData(const T& data);

        friend ostream &operator<< <> (ostream &out, const Node<T>& n);

        /*** Do not modify the interface ***/
};

/* TO-DO: method implementations here */

template<class T>
Node<T>::Node()
{	 	  	  	 	   	     	    	 	
	setNext(NULL);
	setPrev(NULL);
	data=T();
}

template<class T>
Node<T>::Node(const T& d)
{
	setNext(NULL);
	setPrev(NULL);
	setData(d);
}

template<class T>
Node<T>* Node<T>::getNext() const
{
	if(this)
		return (this->next);
	else
		throw runtime_error("Node::getNext() error!");
}

template<class T>
Node<T>* Node<T>::getPrev() const
{
	if(this)
		return (this->prev);
	else
		throw runtime_error("Node::getPrev() error!");
}

template<class T>
T* Node<T>::getDataPtr()
{
	if(this)
		return &this->data;
	else
		throw runtime_error("Node::getDataPtr() error!");
}	 	  	  	 	   	     	    	 	

template<class T>
void Node<T>::setNext(Node<T> *newNext)
{	
	if(this)
		this->next=newNext;
	else	
		throw runtime_error("Node::setNext() error!");
}

template<class T>
void Node<T>::setPrev(Node<T> *newPrev)
{	
	if(this)
		this->prev=newPrev;
	else	
		throw runtime_error("Node::setPrev() error!");
}

template<class T>
void Node<T>::setData(const T& data)
{	
	if(this)
		this->data=data;
	else
		throw runtime_error("Node::setData() error!");
}

template<class T>
T Node<T>::getData() const
{	
	if(this)
		return this->data;
	else
		throw runtime_error("Node::getData() error!");
}	 	  	  	 	   	     	    	 	

template<class T>
ostream &operator<<(ostream &out, const Node<T>& n){
    out << setfill('.') << setw(10) << (void*)n.prev 
        <<" <-| "<< (void*)&n <<" |-> " 
        << setfill('.') << setw(10) << (void*)n.next << " : "
        << n.data ; 
    return out;
};
#endif