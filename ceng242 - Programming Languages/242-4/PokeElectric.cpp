#include "PokeElectric.h"

void PokeElectric::doBuff(Arena arena)
{
    switch(arena)
    {
        case MAGMA:
            decrement();
            break;
        case ELECTRICITY:
            increment();
            break;
        case FOREST:
            decrement();
            break;
    }
}

void PokeElectric::levelUp(int levels)
{
    Pokemon::levelUp(levels);

    HP += levels * 50;
    ATK += levels * 7;
    MAG_DEF += levels * 3;
}

PokeElectric::PokeElectric(int pokemonID, const std::string & name) :
    Pokemon(pokemonID, name)
{
    effect = ELECTRIFIED;
    setDefaults();
}

void PokeElectric::setDefaults()
{
    HP = 500;
    ATK = 70;
    MAG_DEF = 30;
    PHY_DEF = 0;
}

void PokeElectric::setEffBools(const Pokemon * rhs)
{
    switch(rhs->getEffect())
    {
        case BURNING:
            burning = 1;
            break;
        case DROWNING:
            drowning = 1;
            break;
        case ROOTED:
            rooted = 1;
            break;
        default:
            break;
    }
}

void PokeElectric::takesDamageFromEffects()
{
    int netDamage;

    if (HP && burning)
    {
        netDamage = std::max(0, BURN_DAMAGE - MAG_DEF);
        dmgFromEffectsOut(1, BURNING);
        HP = std::max(HP - netDamage, 0);
    }
    
    if (HP && drowning)
    {
        netDamage = std::max(0, DROWN_DAMAGE - MAG_DEF);
        dmgFromEffectsOut(1, DROWNING);
        HP = std::max(HP - netDamage, 0);
    }

    if (HP && rooted)
    {
        netDamage = std::max(0, ROOT_DAMAGE * 2 - MAG_DEF);
        dmgFromEffectsOut(2, ROOTED);
        HP = std::max(HP - netDamage, 0);
    }
}
