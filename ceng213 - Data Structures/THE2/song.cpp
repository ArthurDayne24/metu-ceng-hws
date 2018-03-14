#include "song.hpp"

//do not modify this file contents

Song::Song( string nm, string bd, unsigned short yr, size_t dur )
  : name( nm ), band( bd ), year( yr ), duration( dur ), active( true )
{
}

Song::~Song( )
{
}

string
Song::getName( ) const
{
  return name;
}

string
Song::getBand( ) const
{
  return band;
}

unsigned short
Song::getYear( ) const
{
  return year;
}

size_t
Song::getDuration( ) const
{
  return duration;
}

bool
Song::isActive( ) const
{	 	  	  	 	   	     	    	 	
  return active;
}

void
Song::deactivate( )
{
  active = false;
}

ostream &
operator<<( ostream & out, const Song & s )
{
  out << s.name << '|' << s.band << '|'
      << s.year << '|' << s.duration;

  return out;
}