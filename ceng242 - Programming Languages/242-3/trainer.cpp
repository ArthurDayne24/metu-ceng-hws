#include "trainer.h"

Trainer::Trainer(const string & name, const Pokemon & firstPoke,
        const int rmin[3], const int rmax[3]) : name(name)
{
    for (int i = 0; i < 3; i++)
    {
        minimum[i] = rmin[i];
        maximum[i] = rmax[i];
    }

    pokemons.push_back(new Pokemon(firstPoke));
}

Trainer::Trainer(const Trainer & rhs) : name(name)
{
    for (int i = 0; i < 3; i++)
    {
        minimum[i] = rhs.minimum[i];
        maximum[i] = rhs.maximum[i];
    }

    for (int i = 0; i < rhs.pokemons.size(); i++)
        pokemons.push_back(new Pokemon(*rhs.pokemons[i]));
}

Trainer::~Trainer()
{
    for (int i = 0; i < pokemons.size(); i++)
        delete pokemons[i];
}

ostream& showPokemons(ostream & os, const Trainer & trainer)
{
    for (int i = 0; i < trainer.pokemons.size(); i++)
        os << trainer.pokemons[i]->getName() << std::endl;

    return os;
}

void Trainer::scanRegion(Region & region)
{
    for (int x = minimum[0]; x <= maximum[0]; x++)
        for (int y = minimum[1]; y <= maximum[1]; y++)
            for (int z = minimum[2]; z <= maximum[2]; z++)
                try {
                    catchPokemon(region(x, y, z));
                }catch (pokemonException) {}
}
