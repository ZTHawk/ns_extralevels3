#ifndef _UPGRADE_REINFORCEDARMOR_H_
#define _UPGRADE_REINFORCEDARMOR_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char* RA_config_names[] =
{
	"REINFORCEDARMOR",
	"RA_COST",
	"RA_MAX",
	"RA_LEVEL",
	"RA_MA",
	"RA_HA"
};

class Upgrade_Reinforcedarmor : public base_upgrade_data
{
public:
	float ma_armor;
	float ha_armor;

	void init();
	bool add_to_menu(byte ID, int num, int& Keys, char* menu);
	void show_upgrade_menu(edict_t* player);
	void precache();
};

class EL_Reinforcedarmor : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro

public:
	void setArmorInfo();
	void Think_Post();

private:
	float ma_armor;
	float ha_armor;

	bool weldingRA;
	bool weldedRA;
	float nextWeld;
};

extern Upgrade_Reinforcedarmor data_reinforcedarmor;
extern EL_Reinforcedarmor player_reinforcedarmor[MAX_PLAYERS_PLUS1];


enum RA_sounds
{
	RA_sound_welderidle = 0,
	RA_sound_welderstop,
	RA_sound_welderhit,

	RA_MAX_SOUNDS
};

static const char* RA_sound_files[RA_MAX_SOUNDS] =
{
	"weapons/welderidle.wav",	// selfweld in progress
	"weapons/welderstop.wav",	// selfweld done
	"weapons/welderhit.wav"		// selfweld in progress 2
};

#define WELD_RANGE			200.0
#define ARMOR_ADD_PER_WELD		35.0
#define WELD_DISTANCE			500.0
#define WELD_TIME			0.7

#define MAX_ARMOR_VALUE			12800.0		// +100 for health

#define AngleVectors			(*g_engfuncs.pfnAngleVectors)

// default config settings
#define REINFORCEDARMOR			true
#define RA_COST				1
#define RA_MAX				5
#define RA_LEVEL			0
#define RA_MA				15.0
#define RA_HA				25.0

#endif

