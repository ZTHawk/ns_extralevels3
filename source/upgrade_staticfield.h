#ifndef _UPGRADE_STATICFIELD_H_
#define _UPGRADE_STATICFIELD_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char *SF_config_names[] =
{
	"STATICFIELD",
	"SF_COST",
	"SF_MAX",
	"SF_LEVEL",
	"SF_INITIALRANGE",
	"SF_LEVELRANGE",
	"SF_INITSHOCK",
	"SF_SHOCKADDER"
};

class Upgrade_Staticfield : public base_upgrade_data
{
	public:
	float baseRange;
	float bonusRange;
	float base_shock;
	float shock_adder;
	
	void init( );
	bool add_to_menu( byte ID , int num , int &Keys , char *menu );
	void show_upgrade_menu( edict_t *player );
	void precache( );
};

class EL_Staticfield : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro
	
	private:
	float nextStaticfieldTime;
	float StaticfieldRange;
	float StaticfieldPercentage;
};

extern Upgrade_Staticfield data_staticfield;
extern EL_Staticfield player_staticfield[MAX_PLAYERS_PLUS1];


enum SF_sounds
{
	SF_sound_elecspark = 0,
	
	SF_MAX_SOUNDS
};

static const char *SF_sound_files[SF_MAX_SOUNDS] =
{
	"misc/elecspark3.wav"
};

#define SF_MAX_STRENGTH		99.0
#define SF_TIME			2.0

// default config settings
#define STATICFIELD		true
#define SF_COST			1
#define SF_MAX			5
#define SF_LEVEL		0
#define SF_INITIALRANGE		400.0
#define SF_LEVELRANGE		50.0
#define SF_INITSHOCK		15.0
#define SF_SHOCKADDER		5.0

#endif

