#ifndef TRAINER_CENTER_H
#define TRAINER_CENTER_H

#include "Trainer.h"
#include "Pokemon.h"

#include "PokeFire.h"
#include "PokeElectric.h"
#include "PokeWater.h"
#include "PokeGrass.h"
#include "PokeFlying.h"

#include "Enums.h"

#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>

class TrainerCenter
{
    public:

        TrainerCenter(const std::string & inputFileName);
        ~TrainerCenter();
        
        std::vector<Trainer*> & getAllTrainers();
        Trainer* getTrainerWithID(int trainerID);

    private:

        std::vector<Trainer *> trainers;
};

#endif
