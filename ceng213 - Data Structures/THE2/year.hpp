#ifndef YEAR_COMPARATOR__
#define YEAR_COMPARATOR__
#include "song.hpp"

class YearComparator
{
public:
	bool operator( )( const Song & s1, const Song & s2 ) const
	{
		if(s1.getYear()<s2.getYear())
			return true;
		else if(s1.getYear()>s2.getYear())
			return false;
		else if(s1.getBand()<s2.getBand())
			return true;
		else if(s1.getBand()>s2.getBand())
			return false;
		else if(s1.getName()<s2.getName())
			return true;
		else if(s1.getName()>s2.getName())
			return false;
		else return false;
	}
};

#endif	 	  	  	 	   	     	    	 	
