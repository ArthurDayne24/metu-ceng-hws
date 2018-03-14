#ifndef POKEGRASS_H
#define POKEGRASS_H

#include "Pokemon.h"

class PokeGrass : public Pokemon
{
    public:

        PokeGrass(int pokemonID, const std::string & name);
        
        void levelUp(int levels = 1);

    private:

        void setDefaults();

        void setEffBools(const Pokemon * rhs);
        void takesDamageFromEffects();
        
        void doBuff(Arena arena);
};

#endif
