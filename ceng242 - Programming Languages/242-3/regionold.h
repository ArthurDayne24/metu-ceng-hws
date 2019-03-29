#ifndef REGION_H
#define REGION_H

#include "pokemon.h"
#include <iostream>
#include <exception>

using namespace std;

class Region {
    private:
        int minimum[3];
        int maximum[3];
        //can defauls be set here?
        int pokemons;
        Pokemon ****region;

    public:
        // Do NOT make any modifications below
        Region(const int[3], const int[3]);
        Region(const Region &);
        ~Region();
        int getMinBorder(char) const;
        int getMaxBorder(char) const;
        void placePokemon(const Pokemon &, int, int, int);
        Pokemon & operator()(int, int, int);
        int getPokemonCount(const int[3], const int[3]) const;
        Region crop(const int[3], const int[3]) const;
        void patch(Region);
        Region & operator=(const Region &);
};

class pokemonException: public exception {
    virtual const char* what() const throw() {
            return "There does not exist any pokemon in the given coordinates!";
    }
};

#endif
