#ifndef POKEMON_H
#define POKEMON_H

#include <string>

using namespace std;

class Pokemon
{
    private:
        string name;
        string type;
        int numberOfExperiences;
        int evolutionBound;

        friend bool decide(const Pokemon & pk1, const Pokemon & pk2);

    public:
        mutable int t_ref_cnt; //stores how many trainers have this pokemon
        Pokemon(const string &, const string &, int);
        Pokemon(const Pokemon &);
        ~Pokemon();
        const string & getName() const;
        bool operator>>(const Pokemon &);
        friend Pokemon operator&(Pokemon &, Pokemon &);
        Pokemon& operator=(const Pokemon &);
};

#endif
