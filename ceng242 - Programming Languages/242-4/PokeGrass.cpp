#include "PokeGrass.h"

//Fill Here
void PokeGrass::doBuff(Arena arena)
{
    switch(arena)
    {
        case MAGMA:
            decrement();
            break;
        case FOREST:
            increment();
            break;
        case SKY:
            decrement();
            break;
    }
}

void PokeGrass::levelUp(int levels)
{
    Pokemon::levelUp(levels);
    
    HP += levels * 80;
    ATK += levels * 4;
    PHY_DEF += levels * 3;
}

PokeGrass::PokeGrass(int pokemonID, const std::string & name) :
    Pokemon(pokemonID, name)
{
    effect = ROOTED;
    setDefaults();
}

void PokeGrass::setDefaults()
{
    HP = 800;
    ATK = 40;
    MAG_DEF = 0;
    PHY_DEF = 30;
}

void PokeGrass::setEffBools(const Pokemon * rhs)
{
    switch(rhs->getEffect())
    {
        case BURNING:
            burning = 1;
            break;
        case DROWNING:
            drowning = 1;
            break;
        case ELECTRIFIED:
            electrified = 1;
            break;
        default:
            break;
    }
}

void PokeGrass::takesDamageFromEffects()
{
    int netDamage;

    if (HP && burning)
    {
        netDamage = std::max(0, BURN_DAMAGE * 2 - MAG_DEF);
        dmgFromEffectsOut(2, BURNING);
        HP = std::max(HP - netDamage, 0);
    }
    
    if (HP && drowning)
    {
        netDamage = std::max(0, DROWN_DAMAGE - MAG_DEF);
        dmgFromEffectsOut(1, DROWNING);
        HP = std::max(HP - netDamage, 0);
    }

    if (HP && electrified)
    {
        netDamage = std::max(0, ELECTRIFY_DAMAGE - MAG_DEF);
        dmgFromEffectsOut(1, ELECTRIFIED);
        HP = std::max(HP - netDamage, 0);
    }
}
