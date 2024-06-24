#ifndef _UPGRADE_COMBATEVOLUTION_H_
#define _UPGRADE_COMBATEVOLUTION_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char *CE_config_names[] =
{
	"COMBATEVOLUTION",
	"CE_COST",
	"CE_MAX",
	"CE_LEVEL",
	"CE_SKULK_SPEED_BOOST",
	"CE_GORGE_HEAL_RATE",
	"CE_LERK_DRAIN_RATE",
	"CE_FADE_CRIPPLE_RATE",
	"CE_ONOS_ARMOR_RATE",
	
};

enum CE_aura_types
{
	AURA_SPEED = 0,
	AURA_HEAL,
	AURA_DRAIN,
	AURA_CRIPPLE,
	AURA_ARMOR,
	
	AURA_END
};

class Upgrade_CombatEvolution : public base_upgrade_data
{
	public:
	float base_speed_boost;
	float base_heal_rate;
	float base_drain_rate;
	float base_cripple_rate;
	float base_armor_rate;
	
	void init( );
	bool add_to_menu( byte ID , int num , int &Keys , char *menu );
	void show_upgrade_menu( edict_t *player );
	
	void ServerFrame_Think( );
};

class EL_CombatEvolution : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro
	
	private:
	float next_heal_aura;
	float next_drain_aura;
	float next_cripple_aura;	// next time player can be crippled again
	float cripple_rate;		// how much he has been crippled
	
	float aura_data[AURA_END];
	float aura_time[AURA_END];
	
	void heal_aura_tick( );
	void drain_aura_tick( );
	
	public:
	int got_aura_by[AURA_END];
	int got_aura_level[AURA_END];
	float got_aura_value[AURA_END];
	
	
	void Think_Post( );
	void reset_basic( );
	
	void set_Aura( int auraID , byte owner );
	void remove_Aura( int auraID );
	void check_Cripple( );
};

extern Upgrade_CombatEvolution data_combatevolution;
extern EL_CombatEvolution player_combatevolution[MAX_PLAYERS_PLUS1];


#define CE_RANGE			400.0	// 170.0
#define CE_DRAIN_RANGE			100.0
#define CE_AURA_TIME			2.0
#define CE_HEAL_AURA_DELAY		1.0
#define CE_CRIPPLE_AURA_DELAY		10.0
#define CE_CRIPPLE_TIME			2.0

// default config settings
#define COMBATEVOLUTION			true
#define CE_COST				1
#define CE_MAX				5
#define CE_LEVEL			20
#define CE_SKULK_SPEED_BOOST		10.0
#define CE_GORGE_HEAL_RATE		1.0
#define CE_LERK_DRAIN_RATE		1.0
#define CE_FADE_CRIPPLE_RATE		10.0
#define CE_ONOS_ARMOR_BOOST		10.0


#endif

