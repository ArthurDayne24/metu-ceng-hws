#include "Tab.hpp"
void Tab::open(const string& link, char flag)
{
	if(flag!='n')
		prevPages.push(page);
	page=link;
	nextPages.clear();
}
void Tab::openPrev()
{
	if(!prevPages.isEmpty())
	{
		nextPages.push(page);
		page=prevPages.pop();
	}
}
void Tab::openForward()
{	
	if(!nextPages.isEmpty())
	{
		prevPages.push(page);
		page=nextPages.pop();
	}
}
