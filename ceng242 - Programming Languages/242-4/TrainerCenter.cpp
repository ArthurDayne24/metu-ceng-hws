#include "TrainerCenter.h"

TrainerCenter::TrainerCenter(const std::string& inputFileName)
{
    std::ifstream infile(inputFileName.c_str());

    std::string line;

    //----------------------------------------------    
    
    std::getline(infile, line); //read 1st line

    int numberOfTrainers = std::atoi(line.c_str()+14);

    //----------------------------------------------    

    std::getline(infile, line); //read 2nd line

    int numberOfPokemons = std::atoi(line.c_str()+14);
    
    //----------------------------------------------    

    for (int trainer = 0; trainer < numberOfTrainers; trainer++)
    {
        std::getline(infile, line); //pass blank line
        std::getline(infile, line); //read first line of trainer

        //positions of 1st and 2nd --
        int pos1 = line.find("--", 0);  
        int pos2 = pos1 + 2 + line.substr(pos1 + 2, line.length() - pos1 - 2).find("--", 0);
       
        std::string name = line.substr(pos1 + 2, pos2 - pos1 - 2);
        std::string arena = line.substr(pos2 + 2, line.length() - pos2 - 2);

        Arena favArena;

        if (arena == "Stadium")
            favArena = STADIUM;
	    else if (arena == "Magma")
		    favArena = MAGMA;
	    else if (arena == "Ocean")
    		favArena = OCEAN;
	    else if (arena == "ElectriCity")
		    favArena = ELECTRICITY;
		else if (arena == "Forest")
		    favArena = FOREST;
		else //if (arena == "Sky")
            favArena = SKY;

        std::vector<Pokemon *> pokemons;

        for (int pokemon = 0; pokemon < numberOfPokemons; pokemon++)
        {
            std::getline(infile, line); //read a line of pokemon

            int pokemonId = trainer * numberOfPokemons + pokemon;

            //positions of 1st and 2nd ::
            int pos1 = line.find("::", 0);
            int pos2 = pos1 + 2 + line.substr(pos1 + 2, line.length() - pos1 - 2).find("::", 0);

            std::string name = line.substr(pos1 + 2, pos2 - pos1 - 2); 
            std::string type = line.substr(pos2 + 2, line.length() - pos2 - 2);

            if (type == "FLYING")
                pokemons.push_back(new PokeFlying(pokemonId, name));
            else if (type == "ELECTRIC")
                pokemons.push_back(new PokeElectric(pokemonId, name));
            else if (type == "WATER")
                pokemons.push_back(new PokeWater(pokemonId, name));
            else if (type == "GRASS")
                pokemons.push_back(new PokeGrass(pokemonId, name));
            else //if (type == "FIRE")
                pokemons.push_back(new PokeFire(pokemonId, name));
        }
       
        trainers.push_back(new Trainer(trainer, name, favArena, pokemons));
    }
}

TrainerCenter::~TrainerCenter()
{
    for (int i = 0; i < trainers.size(); i++)
        delete trainers[i];
}

std::vector<Trainer*>& TrainerCenter::getAllTrainers()
{
    return trainers;
}

Trainer* TrainerCenter::getTrainerWithID(int trainerID)
{
    return trainers[trainerID];
}
