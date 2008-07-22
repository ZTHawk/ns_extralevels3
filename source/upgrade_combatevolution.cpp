
#include "upgrade_combatevolution.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_CombatEvolution data_combatevolution;
EL_CombatEvolution player_combatevolution[MAX_PLAYERS_PLUS1];

void Upgrade_CombatEvolution::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = COMBATEVOLUTION), TYPE_BOOL, (char *)CE_config_names[0]},
		{&(req_points = CE_COST), TYPE_INT, (char *)CE_config_names[1]},
		{&(max_level = CE_MAX), TYPE_INT, (char *)CE_config_names[2]},
		{&(req_level = CE_LEVEL), TYPE_INT, (char *)CE_config_names[3]},
		{&(base_speed_boost = CE_SKULK_SPEED_BOOST), TYPE_FLOAT, (char *)CE_config_names[4]},
		{&(base_heal_rate = CE_GORGE_HEAL_RATE), TYPE_FLOAT, (char *)CE_config_names[5]},
		{&(base_drain_rate = CE_LERK_DRAIN_RATE), TYPE_FLOAT, (char *)CE_config_names[6]},
		{&(base_cripple_rate = CE_FADE_CRIPPLE_RATE), TYPE_FLOAT, (char *)CE_config_names[7]},
		{&(base_armor_rate = CE_ONOS_ARMOR_BOOST), TYPE_FLOAT, (char *)CE_config_names[8]},
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	strcpy(upgrade_name, "Combat Evolution");
	strcpy(upgrade_description, "Supports other teammates with an aura in range depending on your class.\n"
					"Skulk: Max Speed [+%d] / Gorge: [+%d%%] HP every two seconds\n"
					"Lerk: Enemys next to teammates lose [%d] armor every two seconds\n"
					"Fade: Melee attacks cripple enemys: [-%d] speed\n"
					"Onos: Max Armor [+%d%%]\n\n"
					"Requires: Hive Ability 2 , Level %d, %d Point%s\n\n"
					"Next level [%d]\n\n"
					"%s%s\n\n"
					"0. Exit\n\n\n\n\n\n\n\n\n");
	
	team = ALIEN;
	max_alien_points += available * max_level * req_points;
}

void Upgrade_CombatEvolution::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s    (Disabled)\n", upgrade_name);
	}else if ( player_combatevolution[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s    ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s    ( %3i / %3i )\n", num, upgrade_name, player_combatevolution[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
}

void Upgrade_CombatEvolution::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	int plevel = player_combatevolution[ID].cur_level + 1;
	
	bool req_correct = player_combatevolution[ID].check_Requirements();
	sprintf(menu, dummy_description,
			(int)(base_speed_boost * plevel),
			(int)(base_heal_rate * plevel),
			(int)(base_drain_rate * plevel),
			(int)(base_cripple_rate * plevel),
			(int)(base_armor_rate * plevel),
			req_level + player_combatevolution[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			player_combatevolution[ID].cur_level + 1,
			req_correct ? "2. Buy " : "",
			req_correct ? upgrade_name : ""
			);
	
	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;
	
	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

void Upgrade_CombatEvolution::ServerFrame_Think( )
{
	if ( data_combatevolution.available == false )
		return;
	
	static byte targetID = 0;
	static byte targetID2 = 0;
	static edict_t *targetEntity = NULL;
	static edict_t *targetEntity2 = NULL;
	static int auraID = 0;
	static byte temp_got_aura_by[MAX_PLAYERS_PLUS1][AURA_END];
	static int temp_got_aura_level[MAX_PLAYERS_PLUS1][AURA_END];
	
	// initiate check
	for ( targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		for ( auraID = 0; auraID < AURA_END; ++auraID )
		{
			temp_got_aura_by[targetID][auraID] = 0;
			temp_got_aura_level[targetID][auraID] = 0;
		}
	}
	
	// check who is in range of whom and mark them
	for ( targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( player_data[targetID].ingame == false )
			continue;
		
		if ( player_data[targetID].team != data_combatevolution.team )
			continue;
		
		if ( player_combatevolution[targetID].cur_level <= 0 )
			continue;
		
		targetEntity = INDEXENT(targetID);
		if ( UTIL_isAlive(targetEntity) == false )
			continue;
		
		if ( player_data[targetID].pClass == CLASS_GESTATE )
			continue;
		
		auraID = player_data[targetID].pClass - CLASS_SKULK;
		
		for ( targetID2 = 1; targetID2 <= gpGlobals->maxClients; ++targetID2 )
		{
			if ( targetID == targetID2 )
				continue;
			
			if ( player_data[targetID2].ingame == false )
				continue;
			
			if ( player_data[targetID2].pTeam != player_data[targetID].pTeam )
				continue;
			
			targetEntity2 = INDEXENT(targetID2);
			if ( UTIL_isAlive(targetEntity2) == false )
				continue;
			
			if ( (targetEntity->v.origin - targetEntity2->v.origin).Length() > CE_RANGE )
				continue;
			
			if ( player_combatevolution[targetID2].got_aura_level[auraID] > player_combatevolution[targetID].cur_level )
				continue;
			
			temp_got_aura_by[targetID2][auraID] = targetID;
			temp_got_aura_level[targetID2][auraID] = player_combatevolution[targetID].cur_level;
		}
	}
	
	for ( targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( player_data[targetID].ingame == false )
			continue;
		
		if ( player_data[targetID].team != data_combatevolution.team )
			continue;
		
		targetEntity = INDEXENT(targetID);
		if ( UTIL_isAlive(targetEntity) == false )
			continue;
		
		if ( player_data[targetID].pClass == CLASS_GESTATE )
			continue;
		
		for ( auraID = 0; auraID < AURA_END; ++auraID )
		{
			// same level as old, do not do anything
			if ( temp_got_aura_level[targetID][auraID] == player_combatevolution[targetID].got_aura_level[auraID] )
				continue;
			
			player_combatevolution[targetID].remove_Aura(auraID);
			if ( temp_got_aura_by[targetID][auraID] > 0 )
				player_combatevolution[targetID].set_Aura(auraID, temp_got_aura_by[targetID][auraID]);
		}
	}
}

//////////

void EL_CombatEvolution::reset( )
{
	cur_level = 0;
	next_heal_aura = 0.0;
	next_drain_aura = 0.0;
	cripple_rate = 0.0;
	
	for ( int i = 0; i < AURA_END; ++i )
	{
		aura_data[i] = 0.0;
		aura_time[i] = 0.0;
	}
	
	reset_basic();
}

void EL_CombatEvolution::reset_basic( )
{
	remove_Aura(AURA_SPEED);
	remove_Aura(AURA_HEAL);
	remove_Aura(AURA_DRAIN);
	remove_Aura(AURA_CRIPPLE);
	remove_Aura(AURA_ARMOR);
}

bool EL_CombatEvolution::check_Requirements( )
{
	return ( player_data[ID].level >= data_combatevolution.req_level + cur_level
		&& player_data[ID].points_available >= data_combatevolution.req_points
		&& cur_level < data_combatevolution.max_level
		&& UTIL_getHiveAbility(pEntity, HIVE_3_WEAPONS) );
}

void EL_CombatEvolution::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	player_data[ID].gestation_Emulation = GESTATE_START;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_addPoints(pEntity, data_combatevolution.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_combatevolution.max_level,
							data_combatevolution.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_CombatEvolution::set_upgrade_values( )
{
	aura_data[AURA_SPEED] = data_combatevolution.base_speed_boost * cur_level;
	aura_data[AURA_HEAL] = data_combatevolution.base_heal_rate * cur_level * 0.01;	// this value is a percentage
	aura_data[AURA_DRAIN] = data_combatevolution.base_drain_rate * cur_level;
	aura_data[AURA_CRIPPLE] = data_combatevolution.base_cripple_rate * cur_level;
	aura_data[AURA_ARMOR] = data_combatevolution.base_armor_rate * cur_level;
}

void EL_CombatEvolution::Think( )
{
	heal_aura_tick();
	drain_aura_tick();
}

void EL_CombatEvolution::Think_Post( )
{
	if ( got_aura_level[AURA_SPEED] > 0 )
	{
		pEntity->v.maxspeed += got_aura_value[AURA_SPEED];
	}
	
	if ( aura_time[AURA_CRIPPLE] > gpGlobals->time )
	{
		pEntity->v.maxspeed -= cripple_rate;
	}
}

void EL_CombatEvolution::set_Aura( int auraID , byte aura_owner )
{
	got_aura_by[auraID] = aura_owner;
	got_aura_level[auraID] = player_combatevolution[aura_owner].cur_level;
	got_aura_value[auraID] = player_combatevolution[aura_owner].aura_data[auraID];
	
	switch ( auraID )
	{
		case AURA_ARMOR:
		{
			// percentage of maxAP
			player_data[ID].armor_bonus += got_aura_value[auraID];
			player_data[ID].maxAP += got_aura_value[auraID];
			pEntity->v.armorvalue += got_aura_value[auraID];
			if ( pEntity->v.armorvalue > player_data[ID].maxAP )
				pEntity->v.armorvalue = player_data[ID].maxAP;
			
			break;
		}
	}
}

void EL_CombatEvolution::remove_Aura( int auraID )
{
	switch ( auraID )
	{
		case AURA_ARMOR:
		{
			//int temp_armor = got_aura_level[AURA_ARMOR] * data_combatevolution.base_speed_boost;
			player_data[ID].armor_bonus -= got_aura_value[AURA_ARMOR];
			pEntity->v.armorvalue -= got_aura_value[AURA_ARMOR];
			if ( pEntity->v.armorvalue < 0.0 )
				pEntity->v.armorvalue = 0.0;
			
			// recalculate max armor
			player_data[ID].getMaxAP();
			
			break;
		}
	}
	
	got_aura_by[auraID] = 0;
	got_aura_level[auraID] = 0;
	got_aura_value[auraID] = 0.0;
}

void EL_CombatEvolution::heal_aura_tick( )
{
	if ( cur_level == 0 )
		return;
	
	if ( next_heal_aura > gpGlobals->time )
		return;
	
	if ( player_data[ID].pClass != CLASS_GORGE )
		return;
	
	next_heal_aura = gpGlobals->time + CE_AURA_TIME;
	
	static edict_t *targetEntity = NULL;
	for ( byte targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( player_data[targetID].ingame == false )
			continue;
		
		if ( player_combatevolution[targetID].got_aura_by[AURA_HEAL] != ID )
			continue;
		
		targetEntity = INDEXENT(targetID);
		if ( UTIL_isAlive(targetEntity) == false )
			continue;
		
		// prevent players from getting aura too often (gorges leaving area of effect and others gorges entering it)
		if ( aura_time[AURA_HEAL] > gpGlobals->time )
			continue;
		
		aura_time[AURA_HEAL] = gpGlobals->time + CE_HEAL_AURA_DELAY;
		
		targetEntity->v.health += player_data[targetID].maxHP * aura_data[AURA_HEAL];
		if ( targetEntity->v.health > player_data[targetID].maxHP )
			targetEntity->v.health = player_data[targetID].maxHP;
	}
}

void EL_CombatEvolution::drain_aura_tick( )
{
	if ( isMvM == true )
		return;
	
	if ( isMvA == true
		&& player_data[ID].team == ALIEN )
		return;
	
	if ( data_combatevolution.available == false )
		return;
	
	if ( next_drain_aura > gpGlobals->time )
		return;
	
	static byte targetID = 0;
	static byte temp_got_aura_by = 0;
	static int temp_got_aura_level = 0;
	static edict_t *targetEntity = NULL;
	
	// initiate check
	temp_got_aura_by = 0;
	temp_got_aura_level = 0;
	targetEntity = NULL;
	
	// check who is in range of whom and mark them
	for ( targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( player_data[targetID].ingame == false )
			continue;
		
		if ( player_data[targetID].pTeam == player_data[ID].pTeam )
			continue;
		
		targetEntity = INDEXENT(targetID);
		if ( UTIL_isAlive(targetEntity) == false )
			continue;
		
		// check if target has a valid Drain aura
		if ( player_combatevolution[targetID].got_aura_level[AURA_DRAIN] <= temp_got_aura_level )
			continue;
		
		if ( (targetEntity->v.origin - pEntity->v.origin).Length() > CE_DRAIN_RANGE )
			continue;
		
		temp_got_aura_by = player_combatevolution[targetID].got_aura_by[AURA_DRAIN];
		temp_got_aura_level = player_combatevolution[targetID].got_aura_level[AURA_DRAIN];
	}
	
	// no valid drain aura found, skip
	if ( temp_got_aura_level <= 0 )
		return;
	
	pEntity->v.armorvalue -= player_combatevolution[temp_got_aura_by].aura_data[AURA_DRAIN];
	if ( pEntity->v.armorvalue <= 0.0 )
		pEntity->v.armorvalue = 0.0;
	
	next_drain_aura = gpGlobals->time + CE_AURA_TIME;
}

void EL_CombatEvolution::check_Cripple( )
{
	if ( data_combatevolution.available == false )
		return;
	
	// player must be already dead, so there is no need to cripple him
	if ( pEntity->v.health < 1.0 )
		return;
	
	if ( next_cripple_aura > gpGlobals->time )
		return;
	
	edict_t *entAttackerWeapon = pEntity->v.dmg_inflictor;
	if ( FNullEnt(entAttackerWeapon) )
		return;
	
	// after spawn weapons sometimes do not have private data set immediatly
	// so we need a check to prevent a crash
	if ( entAttackerWeapon->pvPrivateData == NULL )
		return;
	
	// make sure it is a correct entity so its private data has correct size (prevent rare crash)
	if ( strncmp(STRING(entAttackerWeapon->v.classname), "weapon_", 7) != 0 )
		return;
	
	switch ( get_private(entAttackerWeapon, MAKE_OFFSET(WEAPON_ID)) )
	{
		// skulk + lerk
		case WEAPON_BITE:
		case WEAPON_BITE2:
		case WEAPON_SWIPE:
		case WEAPON_CLAWS:
		{
			break;
		}
		default:
		{
			return;
		}
	}
	
	int attacker_ID = ENTINDEX(entAttackerWeapon->v.owner);
	cripple_rate = player_combatevolution[attacker_ID].got_aura_value[AURA_CRIPPLE];
	next_cripple_aura = gpGlobals->time + CE_CRIPPLE_AURA_DELAY;
	aura_time[AURA_CRIPPLE] = gpGlobals->time + CE_CRIPPLE_TIME;
}
