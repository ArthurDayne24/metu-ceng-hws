#include "PokeWater.h"

void PokeWater::doBuff(Arena arena)
{   
    switch(arena)
    {
        case OCEAN:
            increment();
            break;
        case ELECTRICITY:
            decrement();
            break;
        case FOREST:
            decrement();
            break;
    }
}

void PokeWater::levelUp(int levels)
{
    Pokemon::levelUp(levels);
    
    HP += levels * 70;
    ATK += levels * 5;
    MAG_DEF += levels * 1;
    PHY_DEF += levels * 2;
}

PokeWater::PokeWater(int pokemonID, const std::string& name) :
    Pokemon(pokemonID, name)
{
    effect = DROWNING;
    setDefaults();
}

void PokeWater::setDefaults()
{
    HP = 700;
    ATK = 50;
    MAG_DEF = 10;
    PHY_DEF = 20;
}

void PokeWater::setEffBools(const Pokemon *rhs)
{
    switch(rhs->getEffect())
    {
        case BURNING:
            burning = 1;
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

void PokeWater::takesDamageFromEffects()
{
    int netDamage;

    if (HP && burning)
    {
        netDamage = std::max(0, BURN_DAMAGE - MAG_DEF);
        dmgFromEffectsOut(1, BURNING);
        HP = std::max(HP - netDamage, 0);
    }
    
    if (HP && electrified)
    {
        netDamage = std::max(0, ELECTRIFY_DAMAGE * 2 - MAG_DEF);
        dmgFromEffectsOut(2, ELECTRIFIED);
        HP = std::max(HP - netDamage, 0);
    }

    if (HP && rooted)
    {
        netDamage = std::max(0, ROOT_DAMAGE - MAG_DEF);
        dmgFromEffectsOut(1, ROOTED);
        HP = std::max(HP - netDamage, 0);
    }
}
