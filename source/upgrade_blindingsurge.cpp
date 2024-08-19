
#include "upgrade_blindingsurge.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

#include "plugin.h"

Upgrade_Blindingsurge data_blindingsurge;
EL_Blindingsurge player_blindingsurge[MAX_PLAYERS_PLUS1];

void Upgrade_Blindingsurge::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = BLINDINGSURGE), TYPE_BOOL, (char *)BS_config_names[0]},
		{&(req_points = BS_COST), TYPE_INT, (char *)BS_config_names[1]},
		{&(req_level = BS_LEVEL), TYPE_INT, (char *)BS_config_names[2]},
		{&(cooldown_time = BS_COOLDOWN), TYPE_FLOAT, (char *)BS_config_names[3]},
		{&(health_ratio = BS_HEALTH), TYPE_FLOAT, (char *)BS_config_names[4]},
		{&(range = BS_RANGE), TYPE_FLOAT, (char *)BS_config_names[5]},
		{&(blind_time = BS_BLIND_TIME), TYPE_FLOAT, (char *)BS_config_names[6]}
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	strcpy(upgrade_name, "BlindingSurge");
	strcpy(upgrade_description, "Whenever your health is below 50%% a flash will be spawned.\n"
					"Every enemy in range of [%d] will be blinded for [%2.1f] second%s.\n"
					"After each flash there is a [%2.1f] second%s cooldown time.\n\n"
					"Requires: Resupply , Scan Area , Level %d, %d Point%s\n\n"
					"\n\n"
					"%s%s\n\n"
					"0. Exit\n\n\n\n\n\n\n\n");
	
	health_ratio /= 100.0;
	max_level = 1;
	team = MARINE;
	max_alien_points += available * max_level * req_points;
}

bool Upgrade_Blindingsurge::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s                (Disabled)\n", upgrade_name);
		//return false;
	}else if ( player_blindingsurge[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s                 ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s                ( %3i / %3i )\n", num, upgrade_name, player_blindingsurge[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
	return true;
}

void Upgrade_Blindingsurge::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	bool req_correct = player_blindingsurge[ID].check_Requirements();
	sprintf(menu, dummy_description,
			(int)range,
			blind_time,
			blind_time != 1.0 ? "s" : "",
			cooldown_time,
			cooldown_time != 1.0 ? "s" : "",
			req_level + player_blindingsurge[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			req_correct ? "2. Buy " : "",
			req_correct ? upgrade_name : ( player_blindingsurge[ID].cur_level > 0 ? "You have already acquired this upgrade." : "" )
			);
	
	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;
	
	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

void Upgrade_Blindingsurge::precache( )
{
	if ( isAvA == true )
		return;
	
	FuseLight = PRECACHE_MODEL("sprites/glow01.spr");
}

//////////

void EL_Blindingsurge::reset( )
{
	cur_level = 0;
	nextBlindingsurge = 0.0;
}

bool EL_Blindingsurge::check_Requirements( )
{
	return ( player_data[ID].level >= data_blindingsurge.req_level + cur_level
		&& player_data[ID].points_available >= data_blindingsurge.req_points
		&& cur_level < data_blindingsurge.max_level
		&& player_data[ID].got_resupply
		&& player_data[ID].got_scan );
}

void EL_Blindingsurge::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_addPoints(pEntity, data_blindingsurge.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_blindingsurge.max_level,
							data_blindingsurge.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Blindingsurge::set_upgrade_values( )
{
	nextBlindingsurge = 0.0;
}

void EL_Blindingsurge::respawned( )
{
	nextBlindingsurge = 0.0;
}

void EL_Blindingsurge::Think( )
{
	if ( cur_level == 0 )
		return;
	
	if ( UTIL_isAlive(pEntity) == false )
		return;
	
	// check if below 50%
	if ( pEntity->v.health >= (player_data[ID].maxHP * data_blindingsurge.health_ratio) )
		return;
	
	if ( UTIL_getMask(pEntity, NS_MASK_DIGESTING) == true )
		return;
	
	if ( gpGlobals->time < nextBlindingsurge )
		return;
	
	nextBlindingsurge = gpGlobals->time + data_blindingsurge.cooldown_time;
	
	flash_effect();
	edict_t *targetEntity = NULL;
	for ( byte targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( player_data[targetID].ingame == false )
			continue;
		
		if ( player_data[targetID].team == NO_TEAM )
			continue;
		
		targetEntity = INDEXENT(targetID);
		if ( UTIL_isAlive(targetEntity) == false )
			continue;
		
		if ( pEntity->v.team == targetEntity->v.team )
			continue;
		
		if ( (targetEntity->v.origin - pEntity->v.origin).Length() > data_blindingsurge.range )
			continue;
		
		blind_player(targetEntity);
	}
}

void EL_Blindingsurge::blind_player( edict_t *target )
{
	MESSAGE_BEGIN(MSG_ONE, ScreenFade_ID, NULL, target);
	WRITE_SHORT(FLASH_SECOND);		// fade time
	WRITE_SHORT((int)(FLASH_SECOND_FLOAT * data_blindingsurge.blind_time));		// hold time
	WRITE_SHORT(FLASH_FADE_IN);		// fade effect
	WRITE_BYTE(FLASH_COLOR_R);		// red
	WRITE_BYTE(FLASH_COLOR_G);		// green
	WRITE_BYTE(FLASH_COLOR_B);		// blue
	WRITE_BYTE(FLASH_COLOR_A);		// alpha
	MESSAGE_END();
}

void EL_Blindingsurge::flash_effect( )
{
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_SPRITE);
	WRITE_COORD(pEntity->v.origin.x);
	WRITE_COORD(pEntity->v.origin.y);
	WRITE_COORD(pEntity->v.origin.z);
	WRITE_SHORT(data_blindingsurge.FuseLight);		// sprite index
	WRITE_BYTE(20);			// scale
	WRITE_BYTE(200);		// brightness
	MESSAGE_END();
}

