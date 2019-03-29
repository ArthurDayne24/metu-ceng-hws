#ifndef POKEFLYING_H
#define POKEFLYING_H

#include "Pokemon.h"

class PokeFlying : public Pokemon
{
    public:

        PokeFlying(int pokemonID, const std::string& name);

        void levelUp(int levels = 1);

    private:

        void setDefaults();
        
        void setEffBools(const Pokemon *rhs);
        void takesDamageFromEffects();

        void doBuff(Arena arena);
        
        bool haveEffect() const;
};

#endif
