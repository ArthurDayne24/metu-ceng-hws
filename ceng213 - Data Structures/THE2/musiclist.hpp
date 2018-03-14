#ifndef MUSICLIST__
#define MUSICLIST__

#include "tree.hpp"
#include "song.hpp"
#include "name.hpp"
#include "year.hpp"

class MusicList
{
public:
	MusicList( ); //empty constructor
	MusicList( const list<Song> & ); //sort list whenever necessary and construct tree
	~MusicList( ); //destroy list
	void insert( const string &, const string &,  //insert a new song with these parameters
			   int, size_t =0 );                //push_back into list, insert into indices
	void remove( const string &, const string &,  //remove those items
			   int ); 
	int getNumberOfSongs( ) const;  //how many songs are there in the indices?
	void printAllNameSorted( ) const; //print all sorted via name-based index
	void printAllYearSorted( ) const; //print all sorted via year-based index
	void printNameRange( const string &, const string & ) const; //print all songs sorted with names in range e.g. "AA" to "GK"
	void printYearRange( int, int ) const; //print all songs sorted with years in range e.g. 2005 to 2011
	void printNameTree( ) const; //print the underlying tree via its own print method, implemented, do not change
	void printYearTree( ) const; //the same.
private:
	list<Song> playlist; //actual song list
	Tree<Song,NameComparator> nameBasedIndex; //use this tree index for name queries
	Tree<Song,YearComparator> yearBasedIndex; //use this tree index for year queries

	NameComparator nameLessThan; //comparator function object for name attribute
	YearComparator yearLessThan; //comparator function object for year attribute

	void performActualDeletion( ) //delete nodes from the linked list if their count exceeds a certain threshold
	{
	    int ctr=0;

	    list<Song>::iterator be=playlist.begin();
	    list<Song>::iterator en=playlist.end();

	    while(be!=en)
	    {	 	  	  	 	   	     	    	 	
		    if(!be->isActive())
			    ctr++;
		    be++;
	    }

	    if(2*ctr>=playlist.size())
	        performActualDeletionHelper();
	}

	//You should define and use other utility methods in this section
	
    void performActualDeletionHelper()
    {
        list<Song>::iterator be=playlist.begin();
		list<Song>::iterator en=playlist.end();

		while(be!=en)
		{
    	    if(!be->isActive())
			    be=playlist.erase(be);
		    else
    			be++;
		}
    }
	
	void printWithRangeName(Tree<Song, NameComparator >::TreeNode* node, const string & left, const string & right) const
	{
		if(!node) return;
        
        const string curr=node->element->getName();
        
        if(left<=curr)
            printWithRangeName(node->left, left, right);
        if(left<=curr && curr<=right)
            cout<<*node->element<<endl;
        if(curr<=right)
            printWithRangeName(node->right, left, right);
	}	 	  	  	 	   	     	    	 	
	
	void printWithRangeYear(Tree<Song, YearComparator >::TreeNode* node, int left, int right) const
	{
		if(!node) return;
		
		int curr=node->element->getYear();
        
        if(left<=curr)
            printWithRangeYear(node->left, left, right);
        if(left<=curr && curr<=right)
            cout<<*node->element<<endl;
        if(curr<=right)
            printWithRangeYear(node->right, left, right);
	}
};

#endif