#include "Tournament.h"

Tournament::Tournament(const std::string& tournamentName) :
    tournamentName(tournamentName) {}

Tournament::~Tournament() {}

void Tournament::registerTrainers(const std::vector<Trainer*>& trainers)
{
    this->trainers = trainers;
}

const std::string& Tournament::getTournamentName() const
{
    return tournamentName;
}

void Tournament::declareWinnerPAndUp(Pokemon *poke)
{
    std::cout << "\t\t"
              << "WinnerP:"
              << poke->getName() << std::endl;
    
    poke->levelUp();
}

void Tournament::declareWinnerT(const Trainer *trainer) const
{
    std::cout << '\t'
              << "WinnerT:"
              << trainer->getName() << std::endl << std::endl;
}

int Tournament::duelBetween(Pokemon *p1, Pokemon *p2, Arena arena)
{
    int flag;

    p1->doBuff(arena);
    p2->doBuff(arena);

    while (1)
    {
        if (p1->isAlive())
            p1->takesDamageFromEffects();
        else
        {
            flag = -1;
            break;
        }
        
        if (p1->isAlive())
            p1->attackTo(p2, arena);
        else
        {
            flag = -1;
            break;
        }
        
        if (p2->isAlive())
            p2->takesDamageFromEffects();
        else
        {
            flag = 1;
            break;
        }
        
        if (p2->isAlive())
            p2->attackTo(p1, arena);
        else
        {
            flag = 1;
            break;
        }
    }
    
    p1->undoBuff();
    p2->undoBuff();

    if (flag == 1)
        declareWinnerPAndUp(p1);
    else
        declareWinnerPAndUp(p2);

    return flag;
        
}

void Tournament::renewPokemons(std::vector<Pokemon *> & pokes, int idx)
{
    for (int i = 0; i <= idx; i++)
        pokes[i]->renew();
}

int Tournament::duelBetween(Trainer* t1, Trainer* t2, Arena arena)
{
    std::vector<Pokemon *> & t1pokes = t1->getPokemons();
    std::vector<Pokemon *> & t2pokes = t2->getPokemons();

    int i = 0, j = 0;

    bool changeBit = 0;

    std::cout << "\t"
              << t1->getName() << ' '
              << "vs "
              << t2->getName()
              << ": " << Helpers::getArenaName(arena) << std::endl;

    while (1)
    {
        if (!changeBit)
        {     
            if (duelBetween(t1pokes[i], t2pokes[j], arena) == 1)
            {
                if (j == t2pokes.size() - 1)
                {
                    declareWinnerT(t1);

                    renewPokemons(t1pokes, i);
                    renewPokemons(t2pokes, j);

                    return 1;
                }

                changeBit = 1;                

                j++;
            }
            
            else //if (duelBetween(t1pokes[i], t2pokes[j], arena) == -1)
            {
                if (i == t1pokes.size() - 1)
                {
                    declareWinnerT(t2);
                    
                    renewPokemons(t1pokes, i);
                    renewPokemons(t2pokes, j);
                    
                    return -1;
                }

                changeBit = 0;                

                i++;
            }
        }

        else //if (changeBit)
        {     
            if (duelBetween(t2pokes[j], t1pokes[i], arena) == 1)
            {
                if (i == t1pokes.size() - 1)
                {
                    declareWinnerT(t2);

                    renewPokemons(t1pokes, i);
                    renewPokemons(t2pokes, j);

                    return -1;
                }

                changeBit = 0;                

                i++;
            }
            
            else //if (duelBetween(t2pokes[j], t1pokes[i], arena) == -1)
            {
                if (j == t2pokes.size() - 1)
                {
                    declareWinnerT(t1);
                    
                    renewPokemons(t1pokes, i);
                    renewPokemons(t2pokes, j);
                    
                    return 1;
                }

                changeBit = 1;                

                j++;
            }
        }
    }
}

// Commence the tournament, and return the champion.
Trainer* Tournament::commence()
{
    while (trainers.size() != 1) 
    {
        int i = 0, j = trainers.size() - 1;
    
        std::vector<Trainer *> tmpTrainers;

        for (; j > i; i++, j--)
        {
            std::cout << "Started Round: " << i << " vs " << j << std::endl;
            int total = 0;
            total += duelBetween(trainers[i], trainers[j], trainers[i]->getFavoriteArena());
            total -= duelBetween(trainers[j], trainers[i], trainers[j]->getFavoriteArena());
            total += duelBetween(trainers[i], trainers[j], STADIUM);

            if (total >= 1)
            {
                std::cout << "Round Winner: " << trainers[i]->getName() << '!' << std::endl;
                tmpTrainers.push_back(trainers[i]);
            }

            else //if (total <= -1)
            {
                std::cout << "Round Winner: " << trainers[j]->getName() << '!' << std::endl;
                tmpTrainers.push_back(trainers[j]);
            }
        }

        trainers = tmpTrainers;
    }
    
    return trainers[0]; 
}
