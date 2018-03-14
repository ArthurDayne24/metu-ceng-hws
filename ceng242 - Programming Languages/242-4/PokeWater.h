#ifndef POKEWATER_H
#define POKEWATER_H

#include "Pokemon.h"

class PokeWater : public Pokemon
{
    public:

        PokeWater(int pokemonID, const std::string& name);
    
        void levelUp(int levels = 1);
        
    private:

        void setDefaults();

        void setEffBools(const Pokemon *rhs);
        void takesDamageFromEffects();
     
        void doBuff(Arena arena);
};

#endif
