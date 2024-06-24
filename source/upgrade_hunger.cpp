
#include "upgrade_hunger.h"
#include "upgrade_bloodlust.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_Hunger data_hunger;
EL_Hunger player_hunger[MAX_PLAYERS_PLUS1];

void Upgrade_Hunger::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = HUNGER), TYPE_BOOL, (char *)H_config_names[0]},
		{&(req_points = H_COST), TYPE_INT, (char *)H_config_names[1]},
		{&(max_level = H_MAX), TYPE_INT, (char *)H_config_names[2]},
		{&(req_level = H_LEVEL), TYPE_INT, (char *)H_config_names[3]},
		{&(bonusSpeed = H_SPEED), TYPE_FLOAT, (char *)H_config_names[4]},
		{&(bonusHealthPercentage = H_HEALTHPERCENTAGE), TYPE_FLOAT, (char *)H_config_names[5]},
		{&(baseTime = H_INITIALTIME), TYPE_FLOAT, (char *)H_config_names[6]},
		{&(bonusTime = H_LEVELTIME), TYPE_FLOAT, (char *)H_config_names[7]}
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	strcpy(upgrade_name, "Hunger");
	strcpy(upgrade_description, "Gives you bonuses when you kill an enemy. These bonuses last for the specified time\n"
					"Lasts [%2.2f] second%s, gain is [+%d%%] of max Health, effects of primalscream and speed bonus by [+%d] upon kill (bonuses stack)\n\n"
					"Requires: Blood Lust , Level %d, %d Point%s\n\n"
					"Next level [%d]\n\n"
					"%s%s\n\n"
					"0. Exit\n\n\n\n\n\n\n");
	
	team = ALIEN;
	max_alien_points += available * max_level * req_points;
}

bool Upgrade_Hunger::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s                      (Disabled)\n", upgrade_name);
		//return false;
	}else if ( player_hunger[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s                      ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s                      ( %3i / %3i )\n", num, upgrade_name, player_hunger[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
	return true;
}

void Upgrade_Hunger::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	float hunger_time = baseTime + bonusTime * ( player_hunger[ID].cur_level + 1  );
	
	bool req_correct = player_hunger[ID].check_Requirements();
	sprintf(menu, dummy_description,
			hunger_time,
			( hunger_time == 1.0 ) ? "" : "s",
			(int)data_hunger.bonusHealthPercentage,
			(int)data_hunger.bonusSpeed,
			req_level + player_hunger[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			player_hunger[ID].cur_level + 1,
			req_correct ? "2. Buy " : "",
			req_correct ? upgrade_name : ""
			);
	
	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;
	
	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

void Upgrade_Hunger::precache( )
{
	if ( isAvA == true )
		return;
	
	for ( int i = 0; i < H_MAX_SOUNDS; ++i )
		PRECACHE_SOUND((char *)H_sound_files[i]);
}

//////////

void EL_Hunger::reset( )
{
	cur_level = 0;
	lastFrags = 0.0;
	HungerTime = 0.0;
	
	reset_basic();
}

void EL_Hunger::reset_basic( )
{
	justKilled = false;
	HungerEndTime = 0.0;
	SpeedBonus = 0.0;
	amount_boosts_got = 0;
}

bool EL_Hunger::check_Requirements( )
{
	return ( player_data[ID].level >= data_hunger.req_level + cur_level
		&& player_data[ID].points_available >= data_hunger.req_points
		&& cur_level < data_hunger.max_level
		&& player_bloodlust[ID].cur_level > 0 );
}

void EL_Hunger::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	player_data[ID].gestation_Emulation = GESTATE_START;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_addPoints(pEntity, data_hunger.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_hunger.max_level,
							data_hunger.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Hunger::set_upgrade_values( )
{
	HungerTime = data_hunger.baseTime + cur_level * data_hunger.bonusTime;
}

void EL_Hunger::Think( )
{
	if ( cur_level == 0 )
		return;
	
	if ( justKilled )		// keep this mask ON untill hungertime runs out
		UTIL_setMask(pEntity, MASK_PRIMALSCREAM, true);
	
	if ( pEntity->v.frags > lastFrags )
	{
		justKilled = true;
		lastFrags = pEntity->v.frags;
		
		if ( amount_boosts_got < H_MAX_HUNGER_BOOSTS )
		{
			++amount_boosts_got;
			HungerEndTime = gpGlobals->time + HungerTime;
			
			pEntity->v.health += ( player_data[ID].maxHP / 100.0 * data_hunger.bonusHealthPercentage );
			SpeedBonus += data_hunger.bonusSpeed;
			
			if ( UTIL_getMask(pEntity, MASK_SILENCE) == false )
				EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, H_sound_files[H_sound_primalscream], 0.5, ATTN_NORM, 0, PITCH_NORM);
		}
	}else if ( gpGlobals->time >= HungerEndTime
		&& justKilled )
	{
		if ( pEntity->v.health > player_data[ID].maxHP )
			pEntity->v.health = player_data[ID].maxHP;
		
		justKilled = false;
		SpeedBonus = 0.0;
		amount_boosts_got = 0;
		
		if ( UTIL_getMask(pEntity, MASK_SILENCE) == false )
			EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, H_sound_files[H_sound_chargekill], 0.5, ATTN_NORM, 0, PITCH_NORM);
	}
}

void EL_Hunger::Think_Post( )
{
	if ( cur_level == 0 )
		return;
	
	if ( justKilled )
		pEntity->v.maxspeed += SpeedBonus;
}



