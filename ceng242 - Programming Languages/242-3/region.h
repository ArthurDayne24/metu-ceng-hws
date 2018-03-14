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

        Pokemon *pokemon;

        int divideDim;

        int pokemons;
        Region *lregion, *rregion;

        int lmax() const;

        bool isUnitCell() const;

        int nextDivideDim() const;

        bool leftOrRight(const int coor[3]) const;

        bool cannotDivide() const;

        void arrCpy(const int *from, int *to);
        void arrCpy(const int *from, int *to) const;

        void createLeftRegion(const int submin[3], const int submax[3]);
        void createRightRegion(const int submin[3], const int submax[3]);

        bool isThisRegion(const int submin[3], const int submax[3]) const;
        bool isThisRegion(
                const int min1[3], const int max1[3],
                const int min2[3], const int max2[2]
                ) const;

        void setSubregionCoords(
                int lsubmin[3], int lsubmax[3],
                int rsubmin[3], int rsubmax[3]
                ) const;

        void patchHelper(Region rhs);
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
