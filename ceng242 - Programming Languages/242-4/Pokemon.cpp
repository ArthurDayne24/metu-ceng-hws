#include "Pokemon.h"

Pokemon::Pokemon(int pokemonID, const std::string& name) :
    pokemonID(pokemonID),
    name(name),

	burning(0),
	drowning(0),
	electrified(0),
	rooted(0),

    isBuffed(0),
    level (0) {}

Pokemon::~Pokemon() {}

bool Pokemon::isAlive() const
{
    return HP ? 1 : 0;
}

int Pokemon::getPokemonID() const
{
    return pokemonID;
}

const std::string& Pokemon::getName() const
{
    return name;
}

int Pokemon::getHP() const
{
    return HP;
}
int Pokemon::getATK() const
{
    return ATK;
}

int Pokemon::getMAG_DEF() const
{
    return MAG_DEF;
}

int Pokemon::getPHY_DEF() const
{
    return PHY_DEF;
}

bool Pokemon::isBurning() const
{
    return burning;
}

bool Pokemon::isDrowning() const
{
    return drowning;
}

bool Pokemon::isElectrified() const
{
    return electrified;
}

bool Pokemon::isRooted() const
{
    return rooted;
}

void Pokemon::setBurning(bool burning)
{
    this->burning = burning;
}

void Pokemon::setDrowning(bool drowning)
{
    this->drowning = drowning;
}

void Pokemon::setElectrified(bool electrified)
{
    this->electrified = electrified;
}

void Pokemon::setRooted(bool rooted)
{
    this->rooted = rooted;
}

void Pokemon::increment()
{
    HP += HP_MODIFIER;
    ATK += ATK_MODIFIER;
    
    isBuffed = 1;
}
void Pokemon::decrement()
{
    HP -= HP_MODIFIER;
    ATK -= ATK_MODIFIER;
    
    isBuffed = -1;
}

void Pokemon::undoBuff()
{
    if(!isBuffed) return;

    if (isBuffed == 1)
        decrement();
    else //if (isBuffed == -1)
        increment();
    
    isBuffed = 0;
}

Effect Pokemon::getEffect() const
{
    return effect;
}

void Pokemon::levelUp(int levels)
{
    level += levels;
}

void Pokemon::attackTo(Pokemon* target, Arena currentArena)
{
    char c;

    if(!isBuffed)
        c = '/';
    else if (isBuffed == 1)
        c = '+';
    else //if (isBuffed == -1)
        c = '-';
    
    if (haveEffect())
        target->setEffBools(this); //change target's bools
    
    std::cout << "\t\t\t"
              << name << '(' << HP  << ") "
              << "hit "
              << target->name << '(' << target->HP << ") ";
    int dmg = target->takesDamage(ATK);
    std::cout << dmg
              << '(' << c << ')' << std::endl;
}

//ATK yapaydik?
int Pokemon::takesDamage(int opATK)
{
    int netDamage = std::max(1, opATK - PHY_DEF);

    HP = std::max(HP - netDamage, 0);
    
    return netDamage;
}

bool Pokemon::haveEffect() const
{
    return 1;
}

void Pokemon::renew()
{
    int oldLevel = level;
    
    burning = drowning = electrified = rooted = 0;
    level = 0;

    setDefaults();

    levelUp(oldLevel);
}

void Pokemon::dmgFromEffectsOut(int i, Effect effect) const
{
    std::string mark = (i == 1) ? "!!" : "!!!!";

    std::cout << "\t\t\t"
              << name << '(' << HP << ") "
              << Helpers::getEffectName(effect)
              << mark << std::endl;
}
