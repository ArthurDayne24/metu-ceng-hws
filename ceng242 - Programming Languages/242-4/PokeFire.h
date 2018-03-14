#ifndef POKEFIRE_H
#define POKEFIRE_H

#include "Pokemon.h"

class PokeFire : public Pokemon
{
    public:

        PokeFire(int pokemonID, const std::string& name);

        void levelUp(int levels = 1);

    private:

        void setDefaults();

        void setEffBools(const Pokemon * rhs);
        void takesDamageFromEffects();
        
        void doBuff(Arena arena);
};

#endif
