#include "musiclist.hpp"

void //final, do not change
MusicList::printNameTree( ) const
{
	nameBasedIndex.print( );
}

void //final, do not change
MusicList::printYearTree( ) const
{
	yearBasedIndex.print( );
}

MusicList::MusicList( ){}

MusicList::MusicList( const list<Song> & s )
{
	playlist=s;

	playlist.sort(nameLessThan);
	nameBasedIndex.construct(playlist);

	playlist.sort(yearLessThan);
	yearBasedIndex.construct(playlist);
}

MusicList::~MusicList( )
{   
    //Perform delete operations
    performActualDeletionHelper();
}

void
MusicList::insert( const string & nm, const string & bd,
                   int year, size_t duration )
{
	//complete the body
	Song song(nm, bd, year, duration);
	if(!nameBasedIndex.contains(&song) /* or yearBasedIndex.contains(song)*/)
	{
		playlist.push_back(song);

		list<Song>::const_iterator itr=playlist.end();
		itr--;

		nameBasedIndex.insert(&(*itr));
		yearBasedIndex.insert(&(*itr));
	}
}

void
MusicList::remove( const string & nm, const string & bd,
                   int year )
{
	Song song(nm, bd, year);
	const Song* odevcoksacma=nameBasedIndex.getAddress(song);

	if(!odevcoksacma) return;

	Song* ptr2song = const_cast <Song*>(odevcoksacma);

	ptr2song->deactivate();

	nameBasedIndex.remove(ptr2song);
	yearBasedIndex.remove(ptr2song);

    //if needed perform delete operations
    performActualDeletion();
}

int
MusicList::getNumberOfSongs( ) const
{	 	  	  	 	   	     	    	 	
  return nameBasedIndex.getSize();
}

void
MusicList::printAllNameSorted( ) const
{
  nameBasedIndex.printInOrder(); 
}

void
MusicList::printAllYearSorted( ) const
{
  yearBasedIndex.printInOrder(); 
}

void
MusicList::printNameRange( const string & lower,
                           const string & upper ) const
{
  printWithRangeName(nameBasedIndex.root, lower, upper);
}

void
MusicList::printYearRange( int lower, int upper ) const
{
  printWithRangeYear(yearBasedIndex.root, lower, upper);
}