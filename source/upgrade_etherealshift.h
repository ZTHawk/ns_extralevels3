#ifndef _UPGRADE_ETHEREALSHIFT_H_
#define _UPGRADE_ETHEREALSHIFT_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char* ES_config_names[] =
{
	"ETHEREALSHIFT",
	"ES_COST",
	"ES_MAX",
	"ES_LEVEL",
	"ES_INITIAL",
	"ES_LVLTIME",
	"ES_CLASSMULTI",
	"ES_ONOSSHIFT",
	"ES_DELAY"
};

class Upgrade_Etherealshift : public base_upgrade_data
{
public:
	float ShiftInitial;
	float ShiftLevel;
	float ShiftClassMultiplier_percent;
	bool OnosShift;
	float ShiftDelay;

	void init();
	bool add_to_menu(byte ID, int num, int& Keys, char* menu);
	void show_upgrade_menu(edict_t* player);
	void precache();
};

class EL_Etherealshift : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro

public:
	bool Shifting;		// check if player is using Ethereal Shift

	void reset_basic();

	void start_EtherealShift();

	bool setHUDText(byte vID, bool is_marine, hudtextparms_t& hud_params, char* CoreT_GL_reload_Shift_text);

private:
	float endShiftTime;	// check when player can make next shift
	float maxShiftTime;
	float maxShiftTime_FadeOnos;
};

extern Upgrade_Etherealshift data_etherealshift;
extern EL_Etherealshift player_etherealshift[MAX_PLAYERS_PLUS1];

enum ES_sounds
{
	ES_sound_cloakstart = 0,
	ES_sound_cloakend,

	ES_MAX_SOUNDS
};

static const char* ES_sound_files[ES_MAX_SOUNDS] =
{
	"misc/startcloak.wav",
	"misc/endcloak.wav"
};

#define SHIFT_ENERGY_COST		300.0

// default config settings
#define ETHEREALSHIFT			true
#define ES_COST				1
#define ES_MAX				5
#define ES_LEVEL			5
#define ES_INITIAL			1.0
#define ES_LVLTIME			0.5
#define ES_CLASSMULTI			100.0
#define ES_ONOSSHIFT			true
#define ES_DELAY			2.0

#endif
