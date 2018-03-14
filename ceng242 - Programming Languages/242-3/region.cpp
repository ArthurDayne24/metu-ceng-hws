#include "region.h"

//forlari, ifleri elegant eyle
//3lu olanları

//parantezleri, linelari, satir atlamalari

//name public ones, break members, endline members
//break - group functions
//space vs functions

Region::Region(const int rmin[3], const int rmax[3]) :
    divideDim(0), pokemons(0)
{
    arrCpy(rmin, minimum);
    arrCpy(rmax, maximum);

    pokemon = NULL;

    lregion = rregion = NULL;
}

Region::Region(const Region & rhs)
{
    arrCpy(rhs.minimum, minimum);
    arrCpy(rhs.maximum, maximum);

    pokemon = rhs.pokemon ? new Pokemon(*rhs.pokemon) : NULL;

    divideDim = rhs.divideDim;

    pokemons = rhs.pokemons;

    lregion = rhs.lregion ? new Region(*rhs.lregion) : NULL;
    rregion = rhs.rregion ? new Region(*rhs.rregion) : NULL;
}

Region::~Region()
{
    delete pokemon;

    delete lregion;
    delete rregion;
}

int Region::getMinBorder(char op) const
{
    if (op == 'x')
        return minimum[0];
    else if (op == 'y')
        return minimum[1];
    else
        return minimum[2];
}

int Region::getMaxBorder(char op) const
{
    if (op == 'x')
        return maximum[0];
    else if (op == 'y')
        return maximum[1];
    else
        return maximum[2];
}

void Region::arrCpy(const int *from, int *to) const
{
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
}

void Region::arrCpy(const int *from, int *to)
{
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];
}

bool Region::cannotDivide() const
{
    return maximum[divideDim] == minimum[divideDim];
}

int Region::lmax() const
{
    return (minimum[divideDim] + maximum[divideDim]) / 2;
}

bool Region::leftOrRight(const int coor[3]) const
{
    return coor[divideDim] > lmax();
}

bool Region::isUnitCell() const
{
    return maximum[0] == minimum[0] &&
           maximum[1] == minimum[1] &&
           maximum[2] == minimum[2];
}

int Region::nextDivideDim() const
{
    return (divideDim + 1) % 3;
}

void Region::setSubregionCoords(
        int lsubmin[3], int lsubmax[3],
        int rsubmin[3], int rsubmax[3]
        ) const
{
    arrCpy(minimum, lsubmin);
    arrCpy(minimum, rsubmin);

    arrCpy(maximum, lsubmax);
    arrCpy(maximum, rsubmax);

    lsubmax[divideDim] = lmax();
    rsubmin[divideDim] = lmax() + 1;
}

void Region::placePokemon(const Pokemon & rhs, int x, int y, int z)
{
    if (isUnitCell())
    {
        if (pokemon)
        {
            pokemons--;
            delete pokemon;
        }

        pokemon = new Pokemon(rhs);
        pokemons++;
    }
    else if (cannotDivide())
    {
        divideDim = nextDivideDim();
        placePokemon(rhs, x, y, z);
    }
    else
    {
        pokemons++;

        int lsubmin[3], lsubmax[3], rsubmin[3], rsubmax[3];

        setSubregionCoords(lsubmin, lsubmax, rsubmin, rsubmax);

        int coor[3] = {x, y, z};

        if (!leftOrRight(coor))
        {
            if (!lregion)
                createLeftRegion(lsubmin, lsubmax);

            lregion->placePokemon(rhs, x, y, z);
        }
        else
        {
            if (!rregion)
                createRightRegion(rsubmin, rsubmax);

            rregion->placePokemon(rhs, x, y, z);
        }
    }
}

bool Region::isThisRegion(const int submin[3], const int submax[3]) const
{
    if (submin[0] == minimum[0] &&
        submin[1] == minimum[1] &&
        submin[2] == minimum[2] &&
        submax[0] == maximum[0] &&
        submax[1] == maximum[1] &&
        submax[2] == maximum[2])
        return 1;
    else
        return 0;
}

int Region::getPokemonCount(const int submin[3], const int submax[3]) const
{
    if (isThisRegion(submin, submax))
        return pokemons;
    else
    {
        if(!leftOrRight(submax))
            return lregion ? lregion->getPokemonCount(submin, submax) : 0;
        else if (leftOrRight(submin))
            return rregion ? rregion->getPokemonCount(submin, submax) : 0;
        else
        {
            int lsubmax[3], rsubmin[3];

            arrCpy(submin, rsubmin);
            arrCpy(submax, lsubmax);

            lsubmax[divideDim] = (submin[divideDim] + submax[divideDim]) / 2;
            rsubmin[divideDim] = lsubmax[divideDim] + 1;

            int lcount, rcount;

            lcount = lregion ? lregion->getPokemonCount(submin, lsubmax)  : 0;
            rcount = rregion ? rregion->getPokemonCount(rsubmin, submax)  : 0;

            return lcount + rcount;
        }
    }
}

Region Region::crop(const int submin[3], const int submax[3]) const
{
    if (isThisRegion(submin, submax))
        return *this;
    else
    {
        if(!leftOrRight(submax))
            return lregion->crop(submin, submax);
        else
            return rregion->crop(submin, submax);
    }
}

Region & Region::operator=(const Region & rhs)
{
    arrCpy(rhs.minimum, minimum);
    arrCpy(rhs.maximum, maximum);

    delete pokemon;
    pokemon = rhs.pokemon ? new Pokemon(*rhs.pokemon) : NULL;

    divideDim = rhs.divideDim;

    pokemons = rhs.pokemons;

    delete lregion;
    delete rregion;

    lregion = rhs.lregion ? new Region(*rhs.lregion) : NULL;
    rregion = rhs.rregion ? new Region(*rhs.rregion) : NULL;

    return *this;
}

bool Region::isThisRegion(const int min1[3], const int max1[3],
                          const int min2[3], const int max2[3]) const
{
    if (min1[0] == min2[0] &&
        min1[1] == min2[1] &&
        min1[2] == min2[2] &&
        max1[0] == max2[0] &&
        max1[1] == max2[1] &&
        max1[2] == max2[2])
        return 1;
    else
        return 0;
}

void Region::patch(Region rhs)
{
    for (int x = rhs.minimum[0]; x <= rhs.maximum[0]; x++)
        for (int y = rhs.minimum[1]; y <= rhs.maximum[1]; y++)
            for (int z = rhs.minimum[2]; z <= rhs.maximum[2]; z++)
                try {
                    delete &operator()(x, y, z);
                }catch (pokemonException) {}

    patchHelper(rhs);
}

void Region::patchHelper(Region rhs)
{
    if (rhs.isUnitCell())
    {
        if (rhs.pokemon)
            placePokemon(*rhs.pokemon, rhs.minimum[0],
                         rhs.minimum[1], rhs.minimum[2]);
    }

    else
    {
        if (rhs.lregion)
            patchHelper(*rhs.lregion);
        if (rhs.rregion)
            patchHelper(*rhs.rregion);
    }
}

void Region::createLeftRegion(const int submin[3], const int submax[3])
{
    lregion = new Region(submin, submax);
    lregion->divideDim = nextDivideDim();
}

void Region::createRightRegion(const int submin[3], const int submax[3])
{
    rregion = new Region(submin, submax);
    rregion->divideDim = nextDivideDim();
}

Pokemon & Region::operator()(int x, int y, int z)
{
    int coor[3] = {x, y, z};

    if (isUnitCell())
    {
        if(!pokemon)
            throw pokemonException();

        Pokemon &tmpPokemon = *pokemon;

        pokemon = NULL;
        pokemons--;

        return tmpPokemon;
    }

    else if (!leftOrRight(coor))
    {
        if (!lregion)
            throw pokemonException();

        if (lregion->isUnitCell())
        {
            Pokemon &tmpPokemon = *lregion->pokemon;//what if same name?

            lregion->pokemon = NULL;
            delete lregion;
            lregion = NULL;

            pokemons--;

            return tmpPokemon;
        }
        else
        {
            Pokemon &tmpPokemon = lregion->operator()(x, y, z);

            if (!lregion->pokemons)
            {
                delete lregion;
                lregion = NULL;
            }

            pokemons--;

            return tmpPokemon;
        }
    }
    else
    {
        if (!rregion)
            throw pokemonException();

        if (rregion->isUnitCell())
        {
            Pokemon &tmpPokemon = *rregion->pokemon;

            rregion->pokemon = NULL;
            delete rregion;
            rregion = NULL;

            pokemons--;

            return tmpPokemon;
        }
        else
        {
            Pokemon &tmpPokemon = rregion->operator()(x, y, z);

            if (!rregion->pokemons)
            {
                delete rregion;
                rregion = NULL;
            }

            pokemons--;

            return tmpPokemon;
        }
    }
}
