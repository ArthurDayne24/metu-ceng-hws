#ifndef SONG_H__
#define SONG_H__

#include <string>
#include <ostream>

using std::string;
using std::ostream;

//all is implemented, do not modify anything
class Song
{
public:
  Song( string, string, unsigned short, size_t = 0 );
  ~Song( );
  string getName( ) const;
  string getBand( ) const;
  unsigned short getYear( ) const;
  size_t getDuration( ) const;
  bool isActive( ) const;
  void deactivate( );
private:
  string name;
  string band;
  unsigned short year;
  size_t duration;
  bool active;
  friend ostream & operator<<( ostream &, const Song & );
};

#endif	 	  	  	 	   	     	    	 	
