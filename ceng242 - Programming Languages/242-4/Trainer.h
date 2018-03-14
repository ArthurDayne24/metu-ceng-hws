#ifndef TRAINER_H
#define TRAINER_H

#include <string>
#include <vector>

#include "Pokemon.h"
class Trainer
{
    public:

        Trainer(int trainerID, const std::string & name,
            Arena favoriteArena, std::vector<Pokemon *> & pokemons); 
        ~Trainer();

        int getTrainerID() const;
        const std::string & getName() const;
        Arena getFavoriteArena() const;
        std::vector<Pokemon *> & getPokemons();

    private:

        int trainerID;
        std::string name;
        Arena favoriteArena;
        std::vector<Pokemon *> pokemons;
};

#endif
