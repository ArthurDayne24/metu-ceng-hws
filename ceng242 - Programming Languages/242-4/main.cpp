#include "TrainerCenter.h"
#include "Tournament.h"
#include "Trainer.h"

#include "PokeFire.h"
#include "PokeWater.h"
#include "PokeGrass.h"

#include <iostream>
#include <vector>

int main(int argv, char** argc)
{
	if(argv < 2)
	{
		return -1;
	}

	TrainerCenter newCenter(argc[1]);

	std::vector<Trainer*> allTrainers = newCenter.getAllTrainers();

	Tournament newTournament("MegaTournament");
	newTournament.registerTrainers(allTrainers);
	
	Trainer *champion = newTournament.commence();

	std::cout << newTournament.getTournamentName() << " has finished! The champion is " << champion->getName() << "." << std::endl;

	return 0;
}
