
#include "plugin.h"
#include "upgrade_cybernetics.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_Cybernetics data_cybernetics;
EL_Cybernetics player_cybernetics[MAX_PLAYERS_PLUS1];

void Upgrade_Cybernetics::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = CYBERNETICS), TYPE_BOOL, (char *)C_config_names[0]},
		{&(req_points = C_COST), TYPE_INT, (char *)C_config_names[1]},
		{&(max_level = C_MAX), TYPE_INT, (char *)C_config_names[2]},
		{&(req_level = C_LEVEL), TYPE_INT, (char *)C_config_names[3]},
		{&(ma_speed = C_MA), TYPE_FLOAT, (char *)C_config_names[4]},
		{&(ha_jp_speed = C_HA_JP), TYPE_FLOAT, (char *)C_config_names[5]}
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	strcpy(upgrade_name, "Cybernetics");
	strcpy(upgrade_description, "Cybernetically enhances leg muscles to improve overall movement speed\n"
					"Heavy Armor has a lowered Speed bonus\n\n"
					"Requires: Level %d, %d Point%s\n\n"
					"Next level [%d]\n\n"
					"%s%s\n\n"
					"0. Exit\n\n\n\n\n\n\n");
	
	team = MARINE;
	max_marine_points += available * max_level * req_points;
}

void Upgrade_Cybernetics::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s                   (Disabled)\n", upgrade_name);
	}else if ( player_cybernetics[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s                    ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s                    ( %3i / %3i )\n", num, upgrade_name, player_cybernetics[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
}

void Upgrade_Cybernetics::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	bool req_correct = player_cybernetics[ID].check_Requirements();
	sprintf(menu, dummy_description,
			req_level + player_cybernetics[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			player_cybernetics[ID].cur_level + 1,
			req_correct ? "2. Buy " : "",
			req_correct ? upgrade_name : ""
			);
	
	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;
	
	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

//////////

void EL_Cybernetics::reset( )
{
	cur_level = 0;
	ma_speed = 0.0;
	ha_jp_speed = 0.0;
	jp_air_speed = 0.0;
}

bool EL_Cybernetics::check_Requirements( )
{
	return ( player_data[ID].level >= data_cybernetics.req_level + cur_level
		&& player_data[ID].points_available >= data_cybernetics.req_points
		&& cur_level < data_cybernetics.max_level );
}

void EL_Cybernetics::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_addPoints(pEntity, data_cybernetics.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_cybernetics.max_level,
							data_cybernetics.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Cybernetics::set_upgrade_values( )
{
	ma_speed = cur_level * data_cybernetics.ma_speed;
	ha_jp_speed = cur_level * data_cybernetics.ha_jp_speed;
	jp_air_speed = ha_jp_speed / 100 * C_JP_SPEED_PERCENTAGE;
}

void EL_Cybernetics::Think( )
{
	
}

void EL_Cybernetics::Think_Post( )
{
	if ( cur_level == 0 )
		return;
	
	switch ( player_data[ID].pClass )
	{
		case CLASS_HEAVY:
		{
			pEntity->v.maxspeed += ha_jp_speed;
			break;
		}
		case CLASS_JETPACK:
		{
			// get the time player touched ground
			if ( pEntity->v.flags & FL_ONGROUND )
				in_air_time_check = gpGlobals->time + C_MAX_TIME_IN_AIR;
			
			// if too long in air he is most likely flying with JP
			if ( in_air_time_check < gpGlobals->time )
				pEntity->v.maxspeed += jp_air_speed;
			else
				pEntity->v.maxspeed += ha_jp_speed;
			break;
		}
		default:
		{
			pEntity->v.maxspeed += ma_speed;
			break;
		}
	}
}








