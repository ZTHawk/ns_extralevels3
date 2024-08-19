
#include "upgrade_nanoarmor.h"
#include "upgrade_reinforcedarmor.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_Nanoarmor data_nanoarmor;
EL_Nanoarmor player_nanoarmor[MAX_PLAYERS_PLUS1];

void Upgrade_Nanoarmor::init()
{
	Config_data upgrade_data[] =
	{
		{&(available = NANOARMOR), TYPE_BOOL, (char*)NA_config_names[0]},
		{&(req_points = NA_COST), TYPE_INT, (char*)NA_config_names[1]},
		{&(max_level = NA_MAX), TYPE_INT, (char*)NA_config_names[2]},
		{&(req_level = NA_LEVEL), TYPE_INT, (char*)NA_config_names[3]},
		{&(ma_nanoarmor = NA_MA), TYPE_FLOAT, (char*)NA_config_names[4]},
		{&(ha_nanoarmor = NA_HA), TYPE_FLOAT, (char*)NA_config_names[5]}
	};

	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));

	strcpy(upgrade_name, "Nano Armor");
	strcpy(upgrade_description, "Armor is created with tiny nano bots that weld your armor\n"
		"Self weld every two seconds is [%d] (for current armor type)\n\n"
		"Requires: Reinforced Armor 1 , Level %d, %d Point%s\n\n"
		"Next level [%d]\n\n"
		"%s%s\n\n"
		"0. Exit\n\n\n\n\n\n\n");

	team = MARINE;
	max_marine_points += available * max_level * req_points;
}

bool Upgrade_Nanoarmor::add_to_menu(byte ID, int num, int& Keys, char* menu)
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s                    (Disabled)\n", upgrade_name);
		//return false;
	} else if ( player_nanoarmor[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s                     ( max / %3i )\n", upgrade_name, max_level);
	} else
	{
		Keys |= (1 << (num - 1));
		sprintf(dummy_string, "%d. %s                     ( %3i / %3i )\n", num, upgrade_name, player_nanoarmor[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);

	return true;
}

void Upgrade_Nanoarmor::show_upgrade_menu(edict_t* pEntity)
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char* dummy_description = upgrade_description;

	int nano_weld;
	if ( player_data[ID].pClass == NS_CLASS_HEAVY )
		nano_weld = (int)ha_nanoarmor * (player_nanoarmor[ID].cur_level + 1);
	else
		nano_weld = (int)ma_nanoarmor * (player_nanoarmor[ID].cur_level + 1);

	bool req_correct = player_nanoarmor[ID].check_Requirements();
	sprintf(menu, dummy_description,
		nano_weld,
		req_level + player_nanoarmor[ID].cur_level,
		req_points,
		req_points > 1 ? "s" : "",
		player_nanoarmor[ID].cur_level + 1,
		req_correct ? "2. Buy " : "",
		req_correct ? upgrade_name : ""
	);

	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;

	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

void Upgrade_Nanoarmor::precache()
{
	if ( isAvA == true )
		return;

	for ( int i = 0; i < NA_MAX_SOUNDS; ++i )
		PRECACHE_SOUND((char*)NA_sound_files[i]);
}

//////////

void EL_Nanoarmor::reset()
{
	cur_level = 0;
	ma_nanoarmor = 0.0;
	ha_nanoarmor = 0.0;
	welding_self = false;
	nextNanoweld = 0.0;

	// stop background welding sound
	EMIT_SOUND_DYN2(pEntity, CHAN_STREAM, NA_sound_files[NA_sound_welderidle], 0.0, ATTN_NORM, SND_STOP, PITCH_NORM);
}

bool EL_Nanoarmor::check_Requirements()
{
	return (player_data[ID].level >= data_nanoarmor.req_level + cur_level
		&& player_data[ID].points_available >= data_nanoarmor.req_points
		&& cur_level < data_nanoarmor.max_level
		&& player_reinforcedarmor[ID].cur_level > 0);
}

void EL_Nanoarmor::buy_upgrade()
{
	if ( check_Requirements() == false )
		return;

	++cur_level;

	set_upgrade_values();

	UTIL_addPoints(pEntity, data_nanoarmor.req_points);

	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
		cur_level,
		data_nanoarmor.max_level,
		data_nanoarmor.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Nanoarmor::set_upgrade_values()
{
	ma_nanoarmor = (float)cur_level * data_nanoarmor.ma_nanoarmor;
	ha_nanoarmor = (float)cur_level * data_nanoarmor.ha_nanoarmor;
}

void EL_Nanoarmor::Think()
{
	if ( cur_level == 0 )
		return;

	if ( gpGlobals->time < nextNanoweld )
		return;

	nextNanoweld = gpGlobals->time + NANO_TIME;

	if ( pEntity->v.armorvalue < player_data[ID].maxAP )
	{
		if ( !welding_self )
			EMIT_SOUND_DYN2(pEntity, CHAN_STREAM, NA_sound_files[NA_sound_welderidle], 0.4, ATTN_NORM, 0, PITCH_NORM);

		welding_self = true;
		EMIT_SOUND_DYN2(pEntity, CHAN_AUTO, NA_sound_files[NA_sound_welderstop], 0.8, ATTN_NORM, 0, PITCH_NORM);
	} else if ( welding_self )
	{
		// as we already have max AP so stop selfweld and sound
		welding_self = false;
		EMIT_SOUND_DYN2(pEntity, CHAN_STREAM, NA_sound_files[NA_sound_welderidle], 0.0, ATTN_NORM, SND_STOP, PITCH_NORM);
		return;
	} else
		return;

	float nanoarmor_add = 0;
	if ( player_data[ID].pClass == NS_CLASS_MARINE
		|| player_data[ID].pClass == NS_CLASS_JETPACK )
		nanoarmor_add = ma_nanoarmor;
	else if ( player_data[ID].pClass == NS_CLASS_HEAVY )
		nanoarmor_add = ha_nanoarmor;

	pEntity->v.armorvalue += nanoarmor_add;
	if ( pEntity->v.armorvalue > player_data[ID].maxAP )
		pEntity->v.armorvalue = player_data[ID].maxAP;
}








