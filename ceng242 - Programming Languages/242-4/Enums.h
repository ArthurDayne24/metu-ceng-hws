#ifndef ENUMS_H
#define ENUMS_H

#define BURN_DAMAGE 50
#define DROWN_DAMAGE 60
#define ELECTRIFY_DAMAGE 70
#define ROOT_DAMAGE 40
#define HP_MODIFIER 100
#define ATK_MODIFIER 10

enum Effect
{
	BURNING,
	DROWNING,
	ELECTRIFIED,
	ROOTED
};

enum Arena
{
	STADIUM,
	MAGMA,
	OCEAN,
	ELECTRICITY,
	FOREST,
	SKY
};

namespace Helpers
{
	inline const std::string getArenaName(Arena arena)
	{
		switch(arena)
		{
			case STADIUM:
				return "Stadium";
			case MAGMA:
				return "Magma";
			case OCEAN:
				return "Ocean";
			case ELECTRICITY:
				return "ElectriCity";
			case FOREST:
				return "Forest";
			case SKY:
				return "Sky";
		}
	}

	inline const std::string getEffectName(Effect effect)
	{
		switch(effect)
		{
			case BURNING:
				return "is burning";
			case DROWNING:
				return "is drowning";
			case ELECTRIFIED:
				return "is electrified";
			case ROOTED:
				return "is rooted";
		}
	}

	inline int getEffectDamage(Effect effect)
	{
		switch(effect)
		{
			case BURNING:
				return BURN_DAMAGE;
			case DROWNING:
				return DROWN_DAMAGE;
			case ELECTRIFIED:
				return ELECTRIFY_DAMAGE;
			case ROOTED:
				return ROOT_DAMAGE;
		}
	}
}

#endif
