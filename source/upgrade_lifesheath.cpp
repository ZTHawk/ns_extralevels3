
#include "upgrade_lifesheath.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

#include "plugin.h"

Upgrade_Lifesheath data_lifesheath;
EL_Lifesheath player_lifesheath[MAX_PLAYERS];

void Upgrade_Lifesheath::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = LIFESHEATH), TYPE_BOOL, (char *)LS_config_names[0]},
		{&(req_points = LS_COST), TYPE_INT, (char *)LS_config_names[1]},
		{&(req_level = LS_LEVEL), TYPE_INT, (char *)LS_config_names[2]},
		{&(cooldown_time = LS_COOLDOWN), TYPE_FLOAT, (char *)LS_config_names[3]},
		{&(health_ratio = LS_HEALTH), TYPE_FLOAT, (char *)LS_config_names[4]}
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	strcpy(upgrade_name, "LifeSheath");
	strcpy(upgrade_description, "Whenever your health is below 50%% an umbra cloud will be spawned.\n"
					"After each umbra cloud there is a [%2.1f] second%s cooldown time.\n\n"
					"Requires: Hive Ability 1 , Level %d, %d point%s\n\n"
					"\n\n"
					"%s%s\n\n"
					"0. Exit\n\n\n\n\n\n\n");
	
	health_ratio /= 100.0;
	max_level = 1;
	team = ALIEN;
	max_alien_points += available * max_level * req_points;
}

void Upgrade_Lifesheath::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s                (Disabled)\n", upgrade_name);
	}else if ( player_lifesheath[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s                ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s                ( %3i / %3i )\n", num, upgrade_name, player_lifesheath[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
}

void Upgrade_Lifesheath::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	bool req_correct = player_lifesheath[ID].check_Requirements();
	sprintf(menu, dummy_description,
			cooldown_time,
			cooldown_time != 1.0 ? "s" : "",
			req_level + player_lifesheath[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			req_correct ? "2. Buy " : "",
			req_correct ? upgrade_name : ( player_lifesheath[ID].cur_level > 0 ? "You have already acquired this upgrade." : "" )
			);
	
	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;
	
	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

//////////

void EL_Lifesheath::reset( )
{
	cur_level = 0;
	nextLifesteath = 0.0;
}

bool EL_Lifesheath::check_Requirements( )
{
	return ( player_data[ID].level >= data_lifesheath.req_level + cur_level
		&& player_data[ID].points_available >= data_lifesheath.req_points
		&& cur_level < data_lifesheath.max_level
		&& UTIL_getHiveAbility(pEntity, HIVE_2_WEAPONS) );
}

void EL_Lifesheath::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	player_data[ID].gestation_Emulation = GESTATE_START;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_setPoints(pEntity, UTIL_getPoints(pEntity) + data_lifesheath.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_lifesheath.max_level,
							data_lifesheath.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Lifesheath::set_upgrade_values( )
{
	nextLifesteath = 0.0;
}

void EL_Lifesheath::respawned( )
{
	nextLifesteath = 0.0;
}

void EL_Lifesheath::Think( )
{
	if ( cur_level == 0 )
		return;
	
	if ( UTIL_isAlive(pEntity) == false )
		return;
	
	// check if below 50%
	if ( pEntity->v.health >= (player_data[ID].maxHP * data_lifesheath.health_ratio) )
		return;
	
	if ( gpGlobals->time < nextLifesteath )
		return;
	
	nextLifesteath = gpGlobals->time + data_lifesheath.cooldown_time;
	
	//edict_t *pUmbraCloud = CREATE_NAMED_ENTITY(ALLOC_STRING("umbracloud"));
	int hl_strings_umbracloud_id = hl_strings.find("umbracloud");
	edict_t *pUmbraCloud = CREATE_NAMED_ENTITY(hl_strings_umbracloud_id);
	pUmbraCloud->v.origin = pEntity->v.origin;
	pUmbraCloud->v.team = pEntity->v.team;
	MDLL_Spawn(pUmbraCloud);
	PLAYBACK_EVENT_FULL(0, pUmbraCloud, Umbra_Event_ID, 0.0, Vector(0, 0, 0), Vector(0, 0, 0), 0.0, 0.0, 0, 0, 0, 0);
}








