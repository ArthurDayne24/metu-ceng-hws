#ifndef TRAINER_H
#define TRAINER_H

#include "pokemon.h"
#include "region.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Trainer {
    private:
        string name;
        int minimum[3];
        int maximum[3];
        vector<const Pokemon*> pokemons;
    public:
        // Do NOT make any modifications below
        Trainer(const string&, const Pokemon&, const int[3], const int[3]);
        Trainer(const Trainer&);
        ~Trainer();
        void catchPokemon(const Pokemon& newPokemon) {pokemons.push_back(&newPokemon);};
        void scanRegion(Region&);
        friend ostream& showPokemons(ostream&, const Trainer&);
};

#endif
