#ifndef _UPGRADE_BLINDINGSURGE_H_
#define _UPGRADE_BLINDINGSURGE_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char *BS_config_names[] =
{
	"BLINDINGSURGE",
	"BS_COST",
	"BS_LEVEL",
	"BS_COOLDOWN",
	"BS_HEALTH",
	"BS_RANGE",
	"BS_BLIND_TIME"
};

class Upgrade_Blindingsurge : public base_upgrade_data
{
	public:
	float cooldown_time;
	float health_ratio;
	float range;
	float blind_time;
	
	int FuseLight;
	
	void init( );
	bool add_to_menu( byte ID , int num , int &Keys , char *menu );
	void show_upgrade_menu( edict_t *player );
	void precache( );
};

class EL_Blindingsurge : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro
	
	public:
	float nextBlindingsurge;
	
	void respawned( );
	
	private:
	void blind_player( edict_t *target );
	void flash_effect( );
};

extern Upgrade_Blindingsurge data_blindingsurge;
extern EL_Blindingsurge player_blindingsurge[MAX_PLAYERS_PLUS1];

#define FLASH_COLOR_R		255
#define FLASH_COLOR_G		255
#define FLASH_COLOR_B		255
#define FLASH_COLOR_A		255

#define FLASH_SECOND		(1<<12)
#define FLASH_SECOND_FLOAT	(float)FLASH_SECOND
#define FLASH_FADE_IN		0

// default config settings
#define BLINDINGSURGE		true
#define BS_COST			2
#define BS_LEVEL		12
#define BS_COOLDOWN		5.0
#define BS_HEALTH		50.0
#define BS_RANGE		100.0
#define BS_BLIND_TIME		1.0

#endif

