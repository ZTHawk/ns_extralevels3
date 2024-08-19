#include "upgrade_staticfield.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_Staticfield data_staticfield;
EL_Staticfield player_staticfield[MAX_PLAYERS_PLUS1];

void Upgrade_Staticfield::init()
{
	Config_data upgrade_data[] =
	{
		{&(available = STATICFIELD), TYPE_BOOL, (char*)SF_config_names[0]},
		{&(req_points = SF_COST), TYPE_INT, (char*)SF_config_names[1]},
		{&(max_level = SF_MAX), TYPE_INT, (char*)SF_config_names[2]},
		{&(req_level = SF_LEVEL), TYPE_INT, (char*)SF_config_names[3]},
		{&(baseRange = SF_INITIALRANGE), TYPE_FLOAT, (char*)SF_config_names[4]},
		{&(bonusRange = SF_LEVELRANGE), TYPE_FLOAT, (char*)SF_config_names[5]},
		{&(base_shock = SF_INITSHOCK), TYPE_FLOAT, (char*)SF_config_names[6]},
		{&(shock_adder = SF_SHOCKADDER), TYPE_FLOAT, (char*)SF_config_names[7]}
	};

	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));

	strcpy(upgrade_name, "StaticField");
	strcpy(upgrade_description, "Uses a special electric shock to weaken the natural toughness of enemies (cloacked aliens are ignored)\n"
		"Enemies in range of [%d] are weakened by [%d%%] by lowering max health and max armor\n\n"
		"Requires: Motion Tracking , Weapons 2 , Scan Area , Level %d, %d Point%s\n\n"
		"Next level [%d]\n\n"
		"%s%s\n\n"
		"0. Exit\n\n\n\n\n\n\n");

	team = MARINE;
	max_marine_points += available * max_level * req_points;
}

bool Upgrade_Staticfield::add_to_menu(byte ID, int num, int& Keys, char* menu)
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s                       (Disabled)\n", upgrade_name);
		//return false;
	} else if ( player_staticfield[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s                       ( max / %3i )\n", upgrade_name, max_level);
	} else
	{
		Keys |= (1 << (num - 1));
		sprintf(dummy_string, "%d. %s                       ( %3i / %3i )\n", num, upgrade_name, player_staticfield[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
	return true;
}

void Upgrade_Staticfield::show_upgrade_menu(edict_t* pEntity)
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char* dummy_description = upgrade_description;

	// init range + ( levelrange * rangelevelmultiplier )
	int staticrange = (int)(baseRange + bonusRange * (player_staticfield[ID].cur_level + 1));

	int percentage = (int)(base_shock + (player_staticfield[ID].cur_level + 1) * shock_adder);
	if ( percentage > 99 )
		percentage = 99;	// prevent to kill player with Static Field

	bool req_correct = player_staticfield[ID].check_Requirements();
	sprintf(menu, dummy_description,
		staticrange,
		percentage,
		req_level + player_staticfield[ID].cur_level,
		req_points,
		req_points > 1 ? "s" : "",
		player_staticfield[ID].cur_level + 1,
		req_correct ? "2. Buy " : "",
		req_correct ? upgrade_name : ""
	);

	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;

	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

void Upgrade_Staticfield::precache()
{
	if ( isAvA == true )
		return;

	for ( int i = 0; i < SF_MAX_SOUNDS; ++i )
		PRECACHE_SOUND((char*)SF_sound_files[i]);
}

//////////

void EL_Staticfield::reset()
{
	cur_level = 0;
	nextStaticfieldTime = 0.0;
	StaticfieldRange = 0.0;
	StaticfieldPercentage = 0.0;
}

bool EL_Staticfield::check_Requirements()
{
	return (player_data[ID].level >= data_staticfield.req_level + cur_level
		&& player_data[ID].points_available >= data_staticfield.req_points
		&& cur_level < data_staticfield.max_level
		&& player_data[ID].got_scan == true
		&& UTIL_getMask(pEntity, NS_MASK_WEAPONS2)
		&& UTIL_getMask(pEntity, NS_MASK_MOTION));
}

void EL_Staticfield::buy_upgrade()
{
	if ( check_Requirements() == false )
		return;

	++cur_level;

	set_upgrade_values();

	UTIL_addPoints(pEntity, data_staticfield.req_points);

	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
		cur_level,
		data_staticfield.max_level,
		data_staticfield.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Staticfield::set_upgrade_values()
{
	StaticfieldRange = data_staticfield.baseRange + cur_level * data_staticfield.bonusRange;

	// 100% minus how much to weaken enemy
	StaticfieldPercentage = 100.0 - (data_staticfield.base_shock + (float)cur_level * data_staticfield.shock_adder);
	if ( StaticfieldPercentage > SF_MAX_STRENGTH )
		StaticfieldPercentage = SF_MAX_STRENGTH;
}

void EL_Staticfield::Think()
{
	if ( cur_level == 0 )
		return;

	if ( gpGlobals->time < nextStaticfieldTime )
		return;

	if ( UTIL_isAlive(pEntity) == false )
		return;

	if ( UTIL_getMask(pEntity, NS_MASK_DIGESTING) == true )
		return;

	nextStaticfieldTime = gpGlobals->time + SF_TIME;

	bool play_static_sound = false;
	float cur_max_health;
	float cur_max_armor;
	float static_hp;
	float static_ap;
	edict_t* targetEntity = NULL;
	for ( byte targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( player_data[targetID].ingame == false )
			continue;

		targetEntity = INDEXENT(targetID);
		if ( UTIL_isAlive(targetEntity) == false )
			continue;

		if ( targetEntity->v.team == pEntity->v.team )
			continue;

		if ( (targetEntity->v.origin - pEntity->v.origin).Length() > StaticfieldRange )
			continue;

		if ( player_data[targetID].team != MARINE
			&& (player_data[targetID].team != ALIEN
				|| UTIL_isCloaked(targetEntity) == true) )
			continue;

		if ( targetEntity->v.health > player_data[targetID].maxHP )
			cur_max_health = targetEntity->v.health;
		else
			cur_max_health = player_data[targetID].maxHP;

		cur_max_armor = player_data[targetID].maxAP;

		static_hp = cur_max_health / 100.0 * StaticfieldPercentage;
		static_ap = cur_max_armor / 100.0 * StaticfieldPercentage;

		if ( targetEntity->v.health > static_hp )
		{
			if ( static_hp < 1.0 )
				static_hp = 1.0;

			targetEntity->v.health = static_hp;
			play_static_sound = true;
		}
		if ( targetEntity->v.armorvalue > static_ap )
		{
			if ( static_ap < 1.0 )
				static_ap = 1.0;

			targetEntity->v.armorvalue = static_ap;
			play_static_sound = true;
		}
	}

	if ( play_static_sound )
		EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, SF_sound_files[SF_sound_elecspark], 0.5, ATTN_NORM, 0, PITCH_NORM);
}



