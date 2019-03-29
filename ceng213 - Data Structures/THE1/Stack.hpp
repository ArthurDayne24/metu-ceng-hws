#ifndef _STACK_H_
#define _STACK_H_

#include <iostream>
#include <cstddef>
#include <stdexcept>

using namespace std;

template <class T> 
class Stack {

    private:
        /* array keeping the items of the stack */
        T* items;
        /* number of items in the stack */
        size_t size;
        /* capacity of the stack */
        size_t capacity;

        /*** You can add other private members ***/

    public:
        /*** Do not modify the interface ***/

        /* Creates a stack of given "capacity" 
           defult is 8 items
           */
        Stack(int capacity=8);

        /* Copy constructor, Assignment operator, Destructor*/
        Stack(const Stack<T>& stack);
        Stack<T>& operator=(const Stack<T>& stack);
        ~Stack();

        /* pushes the "item" to the top of the stack 
           increases the capacity as needed
           doubles the capacity when full
           */
        void push(const T& item);
        /* pops the item at the top of the stack and returns 
           decreases the capacity when needed
           halves the capacity when less then 1/3 full
           */
        T pop();
        /* returns the item at the top of the stack witout modifiying the stack 
           (take a look at the top item) 
           */
        const T& top() const;
        /* clears the contents of the stack */
        void clear();
        /* returns true if the stack is empty, false otherwise */
        bool isEmpty() const;
        /* returns the number of items in the stack */
        size_t getSize() const;
        /* returns the capacity the stack */
        size_t getCapacity() const;
        /* prints the contents of the stack 
           from top to bottom, one item per line
           assumes the objects in the stack have operator<< overloaded 
           */
        void print() const;
        /* prints the contents of the stack 
           from bottom to top, one item per line
           assumes the objects in the stack have operator<< overloaded 
           */
        void printReversed() const;

        /*** Do not modify the interface ***/
};


/* TO-DO: method implementations here */

template <class T> 
Stack<T>::Stack(int capacity)
{	 	  	  	 	   	     	    	 	
	this->size=0;
	this->capacity=capacity;	
	items=new T[capacity];
}

template <class T> 
Stack<T>::Stack(const Stack<T>& stack)
{
	size=stack.getSize();
	capacity=stack.getCapacity();
	items=new T [capacity];

	for(size_t i=0; i<size; i++)
		items[i]=stack.items[i];	
}

template <class T> 
Stack<T>& Stack<T>::operator=(const Stack<T>& stack)
{
	if(this!=&stack)
	{
		delete[] items;
		
		capacity=stack.getCapacity();
		size=stack.getSize();
		
		items=new T [capacity];
		
		for(size_t i=0; i<size; i++)
			items[i]=stack.items[i];
	}
	return *this;
}

template <class T> 
Stack<T>::~Stack(){delete[] items;}	 	  	  	 	   	     	    	 	

template <class T> 
void Stack<T>::push(const T& item)
{
	//resize if needed
	if(size+1>capacity)
	{
		T* tmp=new T[2*capacity];
		for(size_t i=0; i<size; i++)
			tmp[i]=items[i];
		
		
		delete[] items;
		items=tmp;
		capacity=2*capacity;
	}
	size++;	
	items[size-1]=item;
}

template <class T> 
T Stack<T>::pop() 
{
	if(!isEmpty())
	{ 
		T toBePopped=top();
	
		if(size*3<capacity && capacity>=16)
		{
			
			T* tmp=new T[capacity/2];
			for(size_t i=0; i<size; i++)
				tmp[i]=items[i];

			delete[] items;
			items=tmp;

			capacity/=2;
		}	 	  	  	 	   	     	    	 	

		//now pop
		size--;
		return toBePopped;
	}
	else
		throw runtime_error("Stack::pop() error!");
}

template <class T> 
const T& Stack<T>::top() const 
{
	if(!isEmpty())
		return items[size-1];
	else
		throw runtime_error("Stack::top() error!");
}

template <class T> 
void Stack<T>::clear()
{
	delete[] items;
	items=new T[8]; 
}

template <class T> 
bool Stack<T>::isEmpty() const{return size==0;}

template <class T> 
size_t Stack<T>::getSize() const{return size;}

template <class T> 
size_t Stack<T>::getCapacity() const{return capacity;}

template <class T> 
void Stack<T>::print() const
{	 	  	  	 	   	     	    	 	
	if(!isEmpty())
    {
	    size_t i;
		for(i=size-1; i>0; i--)
			cout<<items[i]<<endl;
		cout<<items[i]<<endl;
	}
}

template <class T> 
void Stack<T>::printReversed() const 
{
	if(!isEmpty())
    {   
        size_t i;
		for(i=0; i<size-1; i++)
			cout<<items[i]<<endl;
		cout<<items[i]<<endl;
	}
}
#endif