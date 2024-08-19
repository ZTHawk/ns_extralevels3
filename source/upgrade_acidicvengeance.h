#ifndef _UPGRADE_ACIDICVENGEANCE_H_
#define _UPGRADE_ACIDICVENGEANCE_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char* AV_config_names[] =
{
	"ACIDICVENGEANCE",
	"AV_COST",
	"AV_MAX",
	"AV_LEVEL",
	"AV_MA_HP",
	"AV_MA_AP",
	"AV_HA_HP",
	"AV_HA_AP",
	"AV_GORGEGEST_PERCENTAGE",
	"AV_INITRANGE",
	"AV_CLASSRANGE"
};

class Upgrade_Acidicvengeance : public base_upgrade_data
{
public:
	float MA_HealthDmg;
	float MA_ArmorDmg;
	float HA_HealthDmg;
	float HA_ArmorDmg;
	float GorgeGestateBonus;
	float InitRange;
	float ClassRange;

	void init();
	bool add_to_menu(byte ID, int num, int& Keys, char* menu);
	void show_upgrade_menu(edict_t* player);
	void precache();
};

class EL_Acidicvengeance : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro

public:
	float ma_HealthDmg;
	float ma_ArmorDmg;
	float ha_HealthDmg;
	float ha_ArmorDmg;

	void respawned();
	void initAV();

private:
	byte startedAV;
};

extern Upgrade_Acidicvengeance data_acidicvengeance;
extern EL_Acidicvengeance player_acidicvengeance[MAX_PLAYERS_PLUS1];


enum AV_sounds
{
	AV_xenocide_explode = 0,

	AV_MAX_SOUNDS
};

static const char* AV_sound_files[AV_MAX_SOUNDS] =
{
	"weapons/divinewindexplode.wav"
};

#define AV_NONE				0
#define AV_INIT				1
#define AV_DONE				2

// default config settings
#define ACIDICVENGEANCE			true
#define AV_COST				1
#define AV_MAX				5
#define AV_LEVEL			0
#define AV_MA_HP			10.0
#define AV_MA_AP			10.0
#define AV_HA_HP			10.0
#define AV_HA_AP			20.0
#define AV_GORGEGEST_PERCENTAGE		50.0
#define AV_INITRANGE			100.0
#define AV_CLASSRANGE			50.0

#endif

