#ifndef _TAB_H_
#define _TAB_H_

#include <iostream>
#include <string>
#include "Stack.hpp"

using namespace std;

class Tab{

    private:
        string page;
        Stack<string> prevPages;
        Stack<string> nextPages;

    public:

		void open(const string& link, char flag='e');
		void openPrev();
		void openForward();
	
		void printPage() const{cout<<getPage()<<endl;}
		void printPrevPages() const{getPrevPages().print();}
		void printNextPages() const{getNextPages().printReversed();}

		//ne durumda reference donebilirim?
		Stack<string> getPrevPages() const{return prevPages;}
		Stack<string> getNextPages() const{return nextPages;}
		string getPage() const{return page;}

		friend ostream& operator<<(ostream &out, const Tab& tab){out<<tab.getPage(); return out;}
};
#endif	 	  	  	 	   	     	    	 	
