#include "Trainer.h"

Trainer::Trainer(int trainerID, const std::string& name,
    Arena favoriteArena, std::vector<Pokemon *>& pokemons) :
    
    trainerID(trainerID),
    name(name),
    favoriteArena(favoriteArena),
    pokemons(pokemons) {}

Trainer::~Trainer()
{
    for (int i = 0; i < pokemons.size(); i++)
        delete pokemons[i];
}

int Trainer::getTrainerID() const
{
    return trainerID;
}

const std::string& Trainer::getName() const
{
    return name;
}

Arena Trainer::getFavoriteArena() const
{
    return favoriteArena;
}

std::vector<Pokemon *> & Trainer::getPokemons()
{
    return pokemons; 
}
