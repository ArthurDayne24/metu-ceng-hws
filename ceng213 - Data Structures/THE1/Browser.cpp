#include "Browser.hpp"

void Browser::openInNewPage(const string& link)
{
	Tab newTab;
	newTab.open(link, 'n');
	pages.pushBack(newTab);
	selected=pages.getBack().getDataPtr();
}

void Browser::openInNewTab(const string& link)
{
	Tab newTab;
	newTab.open(link, 'n');

	int indexOfSelected;

	if(selected)
		indexOfSelected=findIndexOf(selected);
	
	else
		indexOfSelected=-1;
			
	pages.insertAt(indexOfSelected+1, newTab);
	selectTab(indexOfSelected+1);
}

//call this only when it is not the *selected* tab
void Browser::closeTab(int index)
{
	if(0<=index && index<=pages.getSize()-1)
	{
		if(index==findIndexOf(selected))
			closeSelected();

		else
		{	 	  	  	 	   	     	    	 	
			Node<Tab>* ptrAtIndex=pages.getNodePtrAt(index);

			closedPages.push(ptrAtIndex->getData());
			indexesOfClp.push(index);

			pages.erase(ptrAtIndex);
		}
	}
}

void Browser::closeTab(Node<Tab>* ptrToBeClosed)
{
	if(ptrToBeClosed->getDataPtr()==selected)
		closeSelected();

	else
	{
		int index=findIndexOf(ptrToBeClosed->getDataPtr());
		closedPages.push(ptrToBeClosed->getData());
		indexesOfClp.push(index);
	
		pages.erase(ptrToBeClosed);
	}
}

void Browser::closeSelected()
{
	if(selected)
	{
		int index=findIndexOf(selected);
		Node<Tab>* ptrAtIndex=pages.getNodePtrAt(index);

		if(!ptrAtIndex->getNext() && ptrAtIndex->getPrev())
			selected=ptrAtIndex->getPrev()->getDataPtr();
			
		else if(ptrAtIndex->getNext())
			selected=ptrAtIndex->getNext()->getDataPtr();
		
		else if(!ptrAtIndex->getNext() && !ptrAtIndex->getPrev())
			selected=NULL;
			
		closeTab(ptrAtIndex);
	}	 	  	  	 	   	     	    	 	
}

void Browser::reOpenLast()
{	
	if(!indexesOfClp.isEmpty())
	{
		int index=indexesOfClp.pop();
		pages.insertAt(index, closedPages.pop());
		selected=pages.getNodeAt(index).getDataPtr();
	}
}                               

void Browser::moveTabFromTo(int from, int to)
{
	if(0<=from && from<=pages.getSize()-1 && 
	   0<=to   && to  <=pages.getSize()-1   )
	{
		Tab toBeMoved(pages.getNodeAt(from).getData());
		pages.eraseFrom(from);
		pages.insertAt(to, toBeMoved);
		selected=pages.getNodeAt(to).getDataPtr();
	}
}

int Browser::findIndexOf(Tab* tab) const
{
	int index=0;
	size_t size=pages.getSize();
	Node<Tab>* locater=&pages.getFront();

	while(locater->getDataPtr()!=tab)
	{
		index++;
		if(index==size)
			throw runtime_error("Browser::tab not found!");
		locater=locater->getNext();	
	}	 	  	  	 	   	     	    	 	
	return index;
}

void Browser::displayState() const
{
	cout<<"CURRENT TAB: "<<findIndexOf(selected)<<endl;
	selected->printPage();
	cout<<"OPEN TABS: "<<pages.getSize()<<endl;
	printPages();
	cout<<"CLOSED TABS: "<<closedPages.getSize()<<endl;
	printClosedPages();	
}

void Browser::displayTabDetails() const
{	
	if(selected)
	{
		selected->printNextPages();
		cout<<"> "<<selected->getPage()<<endl;
		selected->printPrevPages();
	}
}
  
void Browser::handleTask(string task)
{
	stringstream ss(task);
	string command;
	string token;
	
	ss>>command;
	
	if(command=="open_new_page")
	{
		ss>>token;
		openInNewPage(token);
	}	 	  	  	 	   	     	    	 	
	
	else if(command=="open_link")
	{
		ss>>token;
		openLink(token);
	}
	
	else if(command=="open_link_in_new_tab")
	{
		ss>>token;
		openInNewTab(token);
	}
	 
	else if(command=="close_tab")
	{
		int index;
		ss>>index;	
		closeTab(index);
	}
	
	else if(command=="reopen_closed_tab")
		reOpenLast();		
	
	else if(command=="move_tab")
	{	
		int index1, index2;
		ss>>index1>>index2;
		moveTabFromTo(index1, index2);
	}
	
	else if(command=="display")
		displayState();
	
	else if(command=="select_tab")
	{
		int index;
		ss>>index;
		selectTab(index);
	}	 	  	  	 	   	     	    	 	
	
	else if(command=="display_tab_details")
		displayTabDetails();

	else if(command=="close_selected_tab")
		closeSelected();
	
	else if(command=="back")
		back();
	
	else if(command=="forward")
		forward();
}
