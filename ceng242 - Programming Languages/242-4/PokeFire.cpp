#include "PokeFire.h"

void PokeFire::doBuff(Arena arena)
{
    switch(arena)
    {
        case MAGMA:
            increment();
            break;
        case OCEAN:
            decrement();
            break;
        case SKY:
            decrement();
            break;
    }
}

void PokeFire::levelUp(int levels)
{
    Pokemon::levelUp(levels);
    
    HP += levels * 60;
    ATK += levels * 6;
    MAG_DEF += levels * 2;
    PHY_DEF += levels * 1;
}

PokeFire::PokeFire(int pokemonID, const std::string& name) :
    Pokemon(pokemonID, name)
{
    effect = BURNING;
    setDefaults();
}

void PokeFire::setDefaults()
{
    HP = 600;
    ATK = 60;
    MAG_DEF = 20;
    PHY_DEF = 10;
}

void PokeFire::setEffBools(const Pokemon *rhs)
{
    switch(rhs->getEffect())
    {
        case DROWNING:
            drowning = 1;
            break;
        case ELECTRIFIED:
            electrified = 1;
            break;
        case ROOTED:
            rooted = 1;
            break;
        default:
            break;
    }
}

void PokeFire::takesDamageFromEffects()
{
    int netDamage;

    if (HP && drowning)
    {
        netDamage = std::max(0, DROWN_DAMAGE * 2 - MAG_DEF);
        dmgFromEffectsOut(2, DROWNING);
        HP = std::max(HP - netDamage, 0);
    }

    if (HP && electrified)
    {
        netDamage = std::max(0, ELECTRIFY_DAMAGE - MAG_DEF);
        dmgFromEffectsOut(1, ELECTRIFIED);
        HP = std::max(HP - netDamage, 0);
    }

    if (HP && rooted)
    {
        netDamage = std::max(0, ROOT_DAMAGE - MAG_DEF);
        dmgFromEffectsOut(1, ROOTED);
        HP = std::max(HP - netDamage, 0);
    }
}
