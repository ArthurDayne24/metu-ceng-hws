#ifndef POKEELECTRIC_H
#define POKEELECTRIC_H

#include "Pokemon.h"

class PokeElectric : public Pokemon
{
    public:

        PokeElectric(int pokemonID, const std::string& name);

        void levelUp(int levels = 1);

    private:

        void setDefaults();
        
        void setEffBools(const Pokemon * rhs);
        void takesDamageFromEffects();
            
        void doBuff(Arena arena);
};

#endif
