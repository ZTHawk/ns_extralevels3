#ifndef _UPGRADE_CYBERNETICS_H_
#define _UPGRADE_CYBERNETICS_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char *C_config_names[] =
{
	"CYBERNETICS",
	"C_COST",
	"C_MAX",
	"C_LEVEL",
	"C_MA",
	"C_HA_JP"
};

class Upgrade_Cybernetics : public base_upgrade_data
{
	public:
	float ma_speed;
	float ha_jp_speed;
	
	void init( );
	bool add_to_menu( byte ID , int num , int &Keys , char *menu );
	void show_upgrade_menu( edict_t *player );
};

class EL_Cybernetics : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro
	
	private:
	float ma_speed;
	float ha_jp_speed;
	float jp_air_speed;
	float in_air_time_check;
	
	void Think_Post( );
};

extern Upgrade_Cybernetics data_cybernetics;
extern EL_Cybernetics player_cybernetics[MAX_PLAYERS_PLUS1];

#define C_MAX_TIME_IN_AIR	0.5
#define C_JP_SPEED_PERCENTAGE	33.0

// default config settings
#define CYBERNETICS		true
#define C_COST			1
#define C_MAX			5
#define C_LEVEL			5
#define C_MA			15.0
#define C_HA_JP			8.0

#endif

