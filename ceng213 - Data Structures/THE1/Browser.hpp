#ifndef _BROWSER_H_
#define _BROWSER_H_

#include <string>
#include "LinkedList.hpp"
#include "Stack.hpp"
#include "Tab.hpp"
#include <sstream>

class Browser{

    private:
        Tab* selected=NULL;
        LinkedList<Tab> pages;
        Stack<Tab> closedPages;
		Stack<int> indexesOfClp;

    public:
        Browser(){}
        void handleTask(string task);

		//can be a case, though aint no tab?
		void openLink(const string& link){if(selected) selected->open(link);}

		void openInNewTab(const string& link);	
		void openInNewPage(const string& link); 
		void closeTab(int index);				
		void closeTab(Node<Tab>* ptrToBeClosed);
		void reOpenLast();                      
		void closeSelected();					

        void printClosedPages() const{closedPages.print();}
        void printPages() const{pages.print();}

		int findIndexOf(Tab* tab) const;
		void moveTabFromTo(int from, int to);   
		void displayState() const;				
		void selectTab(int index){selected=pages.getNodeAt(index).getDataPtr();}	 	  	  	 	   	     	    	 	
		void displayTabDetails() const;
		void back(){if(selected) selected->openPrev();}
		void forward(){if(selected) selected->openForward();}
};
#endif
