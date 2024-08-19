#include "upgrade_thickenedskin.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_Thickenedskin data_thickenedskin;
EL_Thickenedskin player_thickenedskin[MAX_PLAYERS_PLUS1];

void Upgrade_Thickenedskin::init()
{
	Config_data upgrade_data[] =
	{
		{&(available = THICKENEDSKIN), TYPE_BOOL, (char*)TS_config_names[0]},
		{&(req_points = TS_COST), TYPE_INT, (char*)TS_config_names[1]},
		{&(max_level = TS_MAX), TYPE_INT, (char*)TS_config_names[2]},
		{&(req_level = TS_LEVEL), TYPE_INT, (char*)TS_config_names[3]},
		{&(health_add_per_class[NS_CLASS_SKULK] = TS_SKULK), TYPE_FLOAT, (char*)TS_config_names[4]},
		{&(health_add_per_class[NS_CLASS_GORGE] = TS_GORGE), TYPE_FLOAT, (char*)TS_config_names[5]},
		{&(health_add_per_class[NS_CLASS_LERK] = TS_LERK), TYPE_FLOAT, (char*)TS_config_names[6]},
		{&(health_add_per_class[NS_CLASS_FADE] = TS_FADE), TYPE_FLOAT, (char*)TS_config_names[7]},
		{&(health_add_per_class[NS_CLASS_ONOS] = TS_ONOS), TYPE_FLOAT, (char*)TS_config_names[8]},
		{&(health_add_per_class[NS_CLASS_GESTATE] = TS_GESTATE), TYPE_FLOAT, (char*)TS_config_names[9]}
	};

	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));

	strcpy(upgrade_name, "Thickened Skin");
	strcpy(upgrade_description, "Thickens skin to increase health. Health bonus varies with life form\n"
		"Max health of [%d] (for current life form)\n\n"
		"Requires: Carapace , Regeneration , Level %d, %d Point%s\n\n"
		"Next level [%d]\n\n"
		"%s%s\n\n"
		"0. Exit\n\n\n\n\n\n\n");

	team = ALIEN;
	max_alien_points += available * max_level * req_points;
}

bool Upgrade_Thickenedskin::add_to_menu(byte ID, int num, int& Keys, char* menu)
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s        (Disabled)\n", upgrade_name);
		//return false;
	} else if ( player_thickenedskin[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s        ( max / %3i )\n", upgrade_name, max_level);
	} else
	{
		Keys |= (1 << (num - 1));
		sprintf(dummy_string, "%d. %s        ( %3i / %3i )\n", num, upgrade_name, player_thickenedskin[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
	return true;
}

void Upgrade_Thickenedskin::show_upgrade_menu(edict_t* pEntity)
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char* dummy_description = upgrade_description;

	int maxhealth = (int)(class_base_hp[player_data[ID].pClass]
		+ health_add_per_class[player_data[ID].pClass] * (player_thickenedskin[ID].cur_level + 1) + 0.5);	// + 0.5 to emulate round()

	bool req_correct = player_thickenedskin[ID].check_Requirements();
	sprintf(menu, dummy_description,
		maxhealth,
		req_level + player_thickenedskin[ID].cur_level,
		req_points,
		req_points > 1 ? "s" : "",
		player_thickenedskin[ID].cur_level + 1,
		req_correct ? "2. Buy " : "",
		req_correct ? upgrade_name : ""
	);

	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;

	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

void Upgrade_Thickenedskin::precache()
{
	if ( isMvM == true )
		return;

	for ( int i = 0; i < TS_MAX_SOUNDS; ++i )
		PRECACHE_SOUND((char*)TS_sound_files[i]);
}

//////////

void EL_Thickenedskin::reset()
{
	cur_level = 0;
	nextRegen = 0.0;
	nextHiveRegen = 0.0;
	nextMetabolizeRegen = 0.0;
	baseHP = 0.0;
	bonusHP = 0.0;
	HiveRegenHP = 0.0;
	RegenHP = 0.0;
}

bool EL_Thickenedskin::check_Requirements()
{
	return (player_data[ID].level >= data_thickenedskin.req_level + cur_level
		&& player_data[ID].points_available >= data_thickenedskin.req_points
		&& cur_level < data_thickenedskin.max_level
		&& UTIL_getMask(pEntity, NS_MASK_REGENERATION)
		&& UTIL_getMask(pEntity, NS_MASK_CARAPACE));
}

void EL_Thickenedskin::buy_upgrade()
{
	if ( check_Requirements() == false )
		return;

	player_data[ID].gestation_Emulation = GESTATE_START;

	++cur_level;

	set_upgrade_values();

	UTIL_addPoints(pEntity, data_thickenedskin.req_points);

	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
		cur_level,
		data_thickenedskin.max_level,
		data_thickenedskin.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Thickenedskin::set_upgrade_values()
{
	setHealthInfo();
	pEntity->v.health += data_thickenedskin.health_add_per_class[player_data[ID].pClass];
}

void EL_Thickenedskin::Think()
{
	if ( cur_level == 0 )
		return;

	BaseTS_heal();
	Metabolize_heal();
	Hive_heal();
}

void EL_Thickenedskin::BaseTS_heal()
{
	if ( pEntity->v.health < baseHP
		|| pEntity->v.health >= player_data[ID].maxHP )
		return;

	if ( gpGlobals->time < nextRegen )
		return;

	nextRegen = gpGlobals->time + TS_HEAL_TIME;

	RegenerateHealth(RegenHP);

	if ( !UTIL_getMask(pEntity, NS_MASK_SILENCE) )
		EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, TS_sound_files[TS_sound_regen], 0.5, ATTN_NORM, 0, PITCH_NORM);
}

void EL_Thickenedskin::Metabolize_heal()
{
	if ( pEntity->v.health < baseHP
		|| pEntity->v.health >= player_data[ID].maxHP )
		return;

	if ( player_data[ID].curWeapon != NS_WEAPON_METABOLIZE
		|| !(pEntity->v.button & IN_ATTACK) )
		return;

	if ( gpGlobals->time < nextMetabolizeRegen )
		return;

	nextMetabolizeRegen = gpGlobals->time + METABOLIZE_HEAL_TIME;

	RegenerateHealth(METABOLIZE_HEAL_HP);

	if ( UTIL_getMask(pEntity, NS_MASK_SILENCE) == true )
		return;

	int random = RANDOM_LONG(TS_sound_metabolize1, TS_sound_metabolize3);
	EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, TS_sound_files[random], 0.5, ATTN_NORM, 0, PITCH_NORM);
}

void EL_Thickenedskin::Hive_heal()
{
	if ( pEntity->v.health < baseHP
		|| pEntity->v.health >= player_data[ID].maxHP )
		return;

	if ( (pEntity->v.team != Hive_ID->v.team
		|| (pEntity->v.origin - Hive_ID->v.origin).Length() > HIVE_HEAL_RANGE)
		&& (pEntity->v.team != Hive_ID2->v.team
			|| (pEntity->v.origin - Hive_ID2->v.origin).Length() > HIVE_HEAL_RANGE) )
		return;

	if ( gpGlobals->time < nextHiveRegen )
		return;

	nextHiveRegen = gpGlobals->time + TS_HIVE_HEAL_TIME;

	RegenerateHealth(HiveRegenHP);

	if ( !UTIL_getMask(pEntity, NS_MASK_SILENCE) )
		EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, TS_sound_files[TS_sound_regen], 0.5, ATTN_NORM, 0, PITCH_NORM);
}

void EL_Thickenedskin::RegenerateHealth(float Regen_Amount)
{
	// if HP is max base HP start TS healing
	// BUT if AP is not max AP regeneration is only some percent ( to not overpower it but still heal )
	float percentage = 1.0;		// 1 = 100%
	if ( pEntity->v.armorvalue < player_data[ID].maxAP )
		percentage = TS_HEALRATE_NON_MAX_AP;

	pEntity->v.health += (Regen_Amount * percentage);
	if ( pEntity->v.health > player_data[ID].maxHP )
		pEntity->v.health = player_data[ID].maxHP;
	//else if ( pEntity->v.health > 999.0 )
	//	pEntity->v.health = 999.0;

	// update data for Gestate
	player_data[ID].player_gestate_hp += (Regen_Amount * percentage);
	if ( player_data[ID].player_gestate_hp > player_data[ID].maxHP )
		player_data[ID].player_gestate_hp = player_data[ID].maxHP;
	//else if ( player_data[ID].player_gestate_hp > 999.0 )
	//	player_data[ID].player_gestate_hp = 999.0;
}

void EL_Thickenedskin::setHealthInfo()
{
	bonusHP = data_thickenedskin.health_add_per_class[player_data[ID].pClass];
	baseHP = class_base_hp[player_data[ID].pClass];
	RegenHP = alien_regen_hp[player_data[ID].pClass];
	HiveRegenHP = alien_regen_hp_hive[player_data[ID].pClass];

	bonusHP *= cur_level;
	player_data[ID].maxHP = baseHP + bonusHP;
}
