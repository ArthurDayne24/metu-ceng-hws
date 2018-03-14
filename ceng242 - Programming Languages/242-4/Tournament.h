#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "Trainer.h"

#include <vector>

class Tournament
{
    public:

        Tournament(const std::string& tournamentName);
        ~Tournament();

        void registerTrainers(const std::vector<Trainer*>& trainers);
        
        // Commence the tournament, and return the champion.
        Trainer* commence();

        // Return 1 if Trainer1 wins.
        // Return -1 if Trainer2 wins.
        int duelBetween(Trainer* trainer1, Trainer* trainer2, Arena currentArena);

        const std::string& getTournamentName() const;

    private:

        std::string tournamentName;
        std::vector<Trainer *> trainers;

        // Return 1 if Pokemon1 wins.
        // Return -1 if Pokemon2 wins.
        int duelBetween(Pokemon *pokemon1, Pokemon *pokemon2, Arena currentArena);

        //Declare victory of pokemon and level it up
        void declareWinnerPAndUp(Pokemon *pokemon);
        //Declare victory of trainer
        void declareWinnerT(const Trainer *trainer) const;

        //Renew given pokemons upto and including pokemon at idx
        void renewPokemons(std::vector<Pokemon *> & pokes, int idx);
};

#endif
