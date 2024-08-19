#ifndef _UPGRADE_HUNGER_H_
#define _UPGRADE_HUNGER_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char* H_config_names[] =
{
	"HUNGER",
	"H_COST",
	"H_MAX",
	"H_LEVEL",
	"H_SPEED",
	"H_HEALTHPERCENTAGE",
	"H_INITIALTIME",
	"H_LEVELTIME"
};

class Upgrade_Hunger : public base_upgrade_data
{
public:
	float bonusSpeed;
	float bonusHealthPercentage;
	float baseTime;
	float bonusTime;

	void init();
	bool add_to_menu(byte ID, int num, int& Keys, char* menu);
	void show_upgrade_menu(edict_t* player);
	void precache();
};

class EL_Hunger : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro

public:
	void Think_Post();
	void reset_basic();

private:
	bool justKilled;	// check if player killed someone
	float lastFrags;	// last saved frags
	float HungerTime;
	float HungerEndTime;
	float SpeedBonus;

	int amount_boosts_got;
};

extern Upgrade_Hunger data_hunger;
extern EL_Hunger player_hunger[MAX_PLAYERS_PLUS1];


enum H_sounds
{
	H_sound_primalscream = 0,
	H_sound_chargekill,

	H_MAX_SOUNDS
};

static const char* H_sound_files[H_MAX_SOUNDS] =
{
	"weapons/primalscream.wav",
	"weapons/chargekill.wav"
};

#define H_MAX_HUNGER_BOOSTS	3

// default config settings
#define HUNGER			true
#define H_COST			1
#define H_MAX			5
#define H_LEVEL			0
#define H_SPEED			6.0
#define H_HEALTHPERCENTAGE	10.0
#define H_INITIALTIME		3.0
#define H_LEVELTIME		1.0

#endif

