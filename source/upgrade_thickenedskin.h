#ifndef _UPGRADE_THICKENED_SKIN_H_
#define _UPGRADE_THICKENED_SKIN_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char *TS_config_names[] =
{
	"THICKENEDSKIN",
	"TS_COST",
	"TS_MAX",
	"TS_LEVEL",
	"TS_SKULK",
	"TS_GORGE",
	"TS_LERK",
	"TS_FADE",
	"TS_ONOS",
	"TS_GESTATE"
};

class Upgrade_Thickenedskin : public base_upgrade_data
{
	public:
	float health_add_per_class[13];
	
	void init( );
	bool add_to_menu( byte ID , int num , int &Keys , char *menu );
	void show_upgrade_menu( edict_t *player );
	void precache( );
};

class EL_Thickenedskin : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro
	
	public:
	void BaseTS_heal( );
	void Metabolize_heal( );
	void Hive_heal( );
	void setHealthInfo( );
	void RegenerateHealth( float Regen_Amount );
	
	private:
	float nextRegen;		// check when player was healed with Thickened Skin
	float nextHiveRegen;		// check when player was healed with Thickened Skin due to hive
	float nextMetabolizeRegen;	// check when player was healed with Thickened Skin due to metabolize
	
	float baseHP;
	float bonusHP;
	float HiveRegenHP;
	float RegenHP;
};

extern Upgrade_Thickenedskin data_thickenedskin;
extern EL_Thickenedskin player_thickenedskin[MAX_PLAYERS_PLUS1];


static const float alien_regen_hp[13] =
{
	0.0,
	6.0, 13.0, 11.0, 27.0, 63.0,	// skulk - onos
	0.0, 0.0, 0.0, 0.0,		// dummy for rines
	18.0,				// gestate
	0.0				// dead
};

static const float alien_regen_hp_hive[13] =
{
	0.0,
	10.0, 22.0, 18.0, 54.0, 105.0,	// skulk - onos
	0.0, 0.0, 0.0, 0.0,		// dummy for rines
	20.0,				// gestate
	0.0				// dead
};

enum TS_sounds
{
	TS_sound_metabolize1 = 0,
	TS_sound_metabolize2,
	TS_sound_metabolize3,
	TS_sound_regen,
	
	TS_MAX_SOUNDS
};

static const char *TS_sound_files[TS_MAX_SOUNDS] =
{
	"weapons/metabolize1.wav",
	"weapons/metabolize2.wav",
	"weapons/metabolize3.wav",
	"misc/regeneration.wav"
};

#define METABOLIZE_HEAL_HP		20.0
#define METABOLIZE_HEAL_TIME		1.5
#define TS_HEAL_TIME			2.0
#define TS_HIVE_HEAL_TIME		1.0

#define HIVE_HEAL_RANGE			525.0

#define TS_HEALRATE_NON_MAX_AP		0.3

// default config settings
#define THICKENEDSKIN		true
#define TS_COST			1
#define TS_MAX			5
#define TS_LEVEL		0
#define TS_SKULK		10.0
#define TS_GORGE		25.0
#define TS_LERK			15.0
#define TS_FADE			25.0
#define TS_ONOS			30.0
#define TS_GESTATE		20.0


#endif

