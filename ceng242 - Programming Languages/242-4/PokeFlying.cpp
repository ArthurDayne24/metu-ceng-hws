#include "PokeFlying.h"

//Fill Here
void PokeFlying::doBuff(Arena arena)
{
    switch(arena)
    {
        case OCEAN:
            decrement();
            break;
        case ELECTRICITY:
            decrement();
            break;
        case SKY:
            increment();
            break;
    }
}

void PokeFlying::levelUp(int levels)
{
    Pokemon::levelUp(levels);
    
    HP += levels * 65;
    ATK += levels * 5;
    PHY_DEF += levels * 3;
}

PokeFlying::PokeFlying(int pokemonID, const std::string& name) :
    Pokemon(pokemonID, name)
{
    setDefaults();
}

void PokeFlying::setDefaults()
{
    HP = 650;
    ATK = 55;
    MAG_DEF = 0;
    PHY_DEF = 15;
}

bool PokeFlying::haveEffect() const
{
    return 0;
}

void PokeFlying::setEffBools(const Pokemon *rhs)
{   
}

void PokeFlying::takesDamageFromEffects()
{
}
