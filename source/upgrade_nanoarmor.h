#ifndef _UPGRADE_NANOARMOR_H_
#define _UPGRADE_NANOARMOR_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char* NA_config_names[] =
{
	"NANOARMOR",
	"NA_COST",
	"NA_MAX",
	"NA_LEVEL",
	"NA_MA",
	"NA_HA"
};

class Upgrade_Nanoarmor : public base_upgrade_data
{
public:
	float ma_nanoarmor;
	float ha_nanoarmor;

	void init();
	bool add_to_menu(byte ID, int num, int& Keys, char* menu);
	void show_upgrade_menu(edict_t* player);
	void precache();
};

class EL_Nanoarmor : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro

private:
	float ma_nanoarmor;
	float ha_nanoarmor;
	bool welding_self;	// check if Nano Armor is welding player
	float nextNanoweld;	// check time player got last Nanoweld
};

extern Upgrade_Nanoarmor data_nanoarmor;
extern EL_Nanoarmor player_nanoarmor[MAX_PLAYERS_PLUS1];

enum NA_sounds
{
	NA_sound_welderidle = 0,
	NA_sound_welderstop,

	NA_MAX_SOUNDS
};

static const char* NA_sound_files[NA_MAX_SOUNDS] =
{
	"weapons/welderidle.wav",	// selfweld in progress
	"weapons/welderstop.wav"	// selfweld done
};

#define NANO_TIME		2.0

// default config settings
#define NANOARMOR		true
#define NA_COST			1
#define NA_MAX			5
#define NA_LEVEL		0
#define NA_MA			4.0
#define NA_HA			6.0

#endif
