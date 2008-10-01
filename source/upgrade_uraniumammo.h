#ifndef _UPGRADE_URANIUMAMMO_H_
#define _UPGRADE_URANIUMAMMO_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char *UA_config_names[] =
{
	"URANIUMAMMO",
	"UA_COST",
	"UA_MAX",
	"UA_BULLETDMG",
	"UA_GRENDMG"
};

class Upgrade_Uraniumammo : public base_upgrade_data
{
	public:
	float BulletDmgPercentage;
	float GrenadeDmgPercentage;
	
	void init( );
	void add_to_menu( byte ID , int num , int &Keys , char *menu );
	void show_upgrade_menu( edict_t *player );
};

class EL_Uraniumammo : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro
	
	public:
	void respawned( );
	
	void setWeaponData_Dmg( byte mode = 0 );
	
	private:
	int entPistolID;
	int entPrimaryGunID;
	byte PrimaryGunID;
	int entPrimaryGunOldID;
	int entHandGrenadeID;
	float SpawnTime;
	
	void resetGunPointer( );
	void findWeaponData( byte mode = 0 );
};

extern Upgrade_Uraniumammo data_uraniumammo;
extern EL_Uraniumammo player_uraniumammo[MAX_PLAYERS];

#define UA_BOUGHT_NEW_WEAPON		1

// default config settings
#define URANIUMAMMO			true
#define UA_COST				1
#define UA_MAX				5
#define UA_BULLETDMG			10.0
#define UA_GRENDMG			7.0

#endif

