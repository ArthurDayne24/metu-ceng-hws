#ifndef POKEMON_H
#define POKEMON_H

#include <string>
#include <iostream>

#include "Enums.h"

class Pokemon
{
    public:

        Pokemon(int pokemonID, const std::string & name);
        virtual ~Pokemon();

        void attackTo(Pokemon* target, Arena currentArena);
        virtual void levelUp(int levels = 1) = 0;

        int getPokemonID() const;
        const std::string& getName() const;

        int getHP() const;
        int getATK() const;
        int getMAG_DEF() const;
        int getPHY_DEF() const;

        Effect getEffect() const;

        bool isBurning() const;
        bool isDrowning() const;
        bool isElectrified() const;
        bool isRooted() const;

        virtual void setBurning(bool burning);
        virtual void setDrowning(bool drowning);
        virtual void setElectrified(bool electrified);
        virtual void setRooted(bool rooted);

        //Take damage according to effect booleans
        virtual void takesDamageFromEffects() = 0;
        
        //Have any effect on other pokemons
        virtual bool haveEffect() const;

        bool isAlive() const;
        
        //Clear demages token
        //Reset pokemon to its current level
        void renew();   
        
        //Buff or debuff pokemon according to its type
        virtual void doBuff(Arena arena) = 0;
        //Undo applied buffs
        void undoBuff();

    protected:

        int pokemonID;
        std::string name;

        int HP;
        int ATK;
        int MAG_DEF;
        int PHY_DEF;
       
        virtual void setDefaults() = 0;

        bool burning;
        bool drowning;
        bool electrified;
        bool rooted;

        Effect effect;
        
        int isBuffed;

        //Provide extra HP and ATK or reduce them
        //They will be used in doBuff and undoBuff
        void increment();
        void decrement();
        
        int level;

        //Give damage to pokemon according to ATK of its opponent
        //Return actual damage according to its DEF
        int takesDamage(int opATK);

        //Set effect bools according to its opponent
        virtual void setEffBools(const Pokemon * rhs) = 0;
        //Output for demage token according to effect booleans
        void dmgFromEffectsOut(int i, Effect effect) const;
};

#endif
