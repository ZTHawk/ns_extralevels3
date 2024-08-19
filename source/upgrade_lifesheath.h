#ifndef _UPGRADE_LIFESHEATH_H_
#define _UPGRADE_LIFESHEATH_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char* LS_config_names[] =
{
	"LIFESHEATH",
	"LS_COST",
	"LS_LEVEL",
	"LS_COOLDOWN",
	"LS_HEALTH"
};

class Upgrade_Lifesheath : public base_upgrade_data
{
public:
	float cooldown_time;
	float health_ratio;

	void init();
	bool add_to_menu(byte ID, int num, int& Keys, char* menu);
	void show_upgrade_menu(edict_t* player);
};

class EL_Lifesheath : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro

public:
	float nextLifesteath;

	void respawned();
};

extern Upgrade_Lifesheath data_lifesheath;
extern EL_Lifesheath player_lifesheath[MAX_PLAYERS_PLUS1];

// default config settings
#define LIFESHEATH		true
#define LS_COST			2
#define LS_LEVEL		12
#define LS_COOLDOWN		5.0
#define LS_HEALTH		50.0

#endif

