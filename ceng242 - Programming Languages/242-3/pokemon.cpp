#include "pokemon.h"

Pokemon::Pokemon(const string & name, const string & type, int evolutionBound) :
    name(name),
    type(type),
    evolutionBound(evolutionBound),
    numberOfExperiences(0),
    t_ref_cnt(0) {}

Pokemon::Pokemon(const Pokemon & rhs) :
    name(rhs.name),
    type(rhs.type),
    numberOfExperiences(rhs.numberOfExperiences),
    evolutionBound(rhs.evolutionBound),
    t_ref_cnt(0) {}

Pokemon::~Pokemon() {}

const string& Pokemon::getName() const
{
    return name;
}

//geq or greater only
bool Pokemon::operator>>(const Pokemon & rhs)
{
    if (evolutionBound != -1 && numberOfExperiences >= evolutionBound)
    {
        name = rhs.name;
        type = rhs.type;
        evolutionBound = rhs.evolutionBound;

        return 1;
    }
    else
        return 0;
}

bool decide(const Pokemon & pk1, const Pokemon & pk2)
{
    string t1 = pk1.type;
    string t2 = pk2.type;

    if (t1 == "fire")
        if (t2 == "water")
            return 1;
        else if (t2 == "grass")
            return 0;
        else if (t2 == "electric")
            return 0;
        else if (t2 == "flying")
            return 1;
        else
           return ! decide(pk2, pk1);
    else if (t1 == "water")
        if (t2 == "grass")
            return 1;
        else if (t2 == "electric")
            return 1;
        else if (t2 == "flying")
            return 0;
        else
           return ! decide(pk2, pk1);
    else if (t1 == "grass")
        if (t2 == "electric")
            return 0;
        else if (t2 == "flying")
            return 1;
        else
           return ! decide(pk2, pk1);
    else if (t1 == "electric")
        if (t2 == "flying")
            return 0;
        else
           return ! decide(pk2, pk1);
    else
       return ! decide(pk2, pk1);

}

Pokemon operator&(Pokemon & pk1, Pokemon & pk2)
{
    bool which = decide(pk1, pk2);

    pk1.numberOfExperiences++;
    pk2.numberOfExperiences++;

    if (!which)
        return pk1;
    else
        return pk2;
}

Pokemon & Pokemon::operator=(const Pokemon & rhs)
{
    name = rhs.name;
    type = rhs.type;
    numberOfExperiences = rhs.numberOfExperiences;
    evolutionBound = rhs.evolutionBound;
    t_ref_cnt = rhs.t_ref_cnt;
    return *this;
}
