#ifndef _UPGRADE_BLOODLUST_H_
#define _UPGRADE_BLOODLUST_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char* BL_config_names[] =
{
	"BLOODLUST",
	"BL_COST",
	"BL_MAX",
	"BL_LEVEL",
	"BL_REGEN",
	"BL_ONOSPERCENTAGE",
	"BL_VAMPIRISM"
};

class Upgrade_Bloodlust : public base_upgrade_data
{
public:
	float baseAdrenalineRegen;
	float OnosPercentage;
	float baseVampirism;

	void init();
	bool add_to_menu(byte ID, int num, int& Keys, char* menu);
	void show_upgrade_menu(edict_t* player);
};

class EL_Bloodlust : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro

private:
	float BloodlustRegen;
	float BloodlustRegenOnos;
	float nextBloodlust;	// check time player gets next Bloodlust

	float lastFrags;	// last saved frags

public:
	float Vampirism;
	float Vampirism_Fade_Onos;

	void drink_my_Blood();
};

extern Upgrade_Bloodlust data_bloodlust;
extern EL_Bloodlust player_bloodlust[MAX_PLAYERS_PLUS1];

#define BASE_ADRENELINE_REGEN		15.5	// adreneline player gets with lvl 3 adren per 0.1 sec ( not 100% exact value )
#define ONOS_MIN_ADRENALINE		64.0

#define BL_TIME				0.1

#define BL_KILL_BOOSTER			100.0
#define BL_VAMPIRISM_PERCENTAGE		0.5	// 1.0 = 100%
#define BL_VAMPIRISM_HA_PERCENTAGE	0.5	// 1.0 = 100%

// default config settings
#define BLOODLUST			true
#define BL_COST				1
#define BL_MAX				5
#define BL_LEVEL			0
#define BL_REGEN			2.0
#define BL_ONOSPERCENTAGE		33.0
#define BL_VAMPIRISM			1.0

#endif
