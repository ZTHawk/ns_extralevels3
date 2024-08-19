#include "upgrade_reinforcedarmor.h"
#include "utilfunctions.h"
#include "player_data.h"

Upgrade_Reinforcedarmor data_reinforcedarmor;
EL_Reinforcedarmor player_reinforcedarmor[MAX_PLAYERS_PLUS1];

void Upgrade_Reinforcedarmor::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = REINFORCEDARMOR), TYPE_BOOL, (char *)RA_config_names[0]},
		{&(req_points = RA_COST), TYPE_INT, (char *)RA_config_names[1]},
		{&(max_level = RA_MAX), TYPE_INT, (char *)RA_config_names[2]},
		{&(req_level = RA_LEVEL), TYPE_INT, (char *)RA_config_names[3]},
		{&(ma_armor = RA_MA), TYPE_FLOAT, (char *)RA_config_names[4]},
		{&(ha_armor = RA_HA), TYPE_FLOAT, (char *)RA_config_names[5]}
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	strcpy(upgrade_name, "Reinforced Armor");
	strcpy(upgrade_description, "Reinforces armor with stronger materials\n"
					"Max armor of [%d] (for current armor type)\n\n"
					"Requires: Armor 3 , Level %d, %d Point%s\n\n"
					"Next level [%d]\n\n"
					"%s%s\n\n0. Exit\n\n\n\n\n\n\n");
	
	team = MARINE;
	max_marine_points += available * max_level * req_points;
}

bool Upgrade_Reinforcedarmor::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s          (Disabled)\n", upgrade_name);
		//return false;
	}else if ( player_reinforcedarmor[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s          ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s          ( %3i / %3i )\n", num, upgrade_name, player_reinforcedarmor[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
	return true;
}

void Upgrade_Reinforcedarmor::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	float bonus = ( ( player_data[ID].pClass == NS_CLASS_HEAVY ) ? ha_armor : ma_armor )
			* ( player_reinforcedarmor[ID].cur_level + 1 );
	int maxarmor = (int)(class_base_ap_lvl3[player_data[ID].pClass]
				+ bonus + 0.5);		// + 0.5 to emulate round()
	
	bool req_correct = player_reinforcedarmor[ID].check_Requirements();
	sprintf(menu, dummy_description,
			maxarmor,
			req_level + player_reinforcedarmor[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			player_reinforcedarmor[ID].cur_level + 1,
			req_correct ? "2. Buy " : "",
			req_correct ? upgrade_name : ""
			);
	
	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;
	
	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

void Upgrade_Reinforcedarmor::precache( )
{
	if ( isAvA == true )
		return;
	
	for ( int i = 0; i < RA_MAX_SOUNDS; ++i )
		PRECACHE_SOUND((char *)RA_sound_files[i]);
}

//////////

void EL_Reinforcedarmor::reset( )
{
	cur_level = 0;
	ma_armor = 0.0;
	ha_armor = 0.0;
	weldingRA = false;
	weldedRA = false;
	nextWeld = 0.0;
}

bool EL_Reinforcedarmor::check_Requirements( )
{
	return ( player_data[ID].level >= data_reinforcedarmor.req_level + cur_level
		&& player_data[ID].points_available >= data_reinforcedarmor.req_points
		&& cur_level < data_reinforcedarmor.max_level
		&& UTIL_getMask(pEntity, NS_MASK_ARMOR3) );
}

void EL_Reinforcedarmor::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_addPoints(pEntity, data_reinforcedarmor.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_reinforcedarmor.max_level,
							data_reinforcedarmor.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Reinforcedarmor::set_upgrade_values( )
{
	ma_armor = (float)cur_level * data_reinforcedarmor.ma_armor;
	ha_armor = (float)cur_level * data_reinforcedarmor.ha_armor;
	
	setArmorInfo();
	pEntity->v.armorvalue += ( player_data[ID].pClass == NS_CLASS_HEAVY ?
					data_reinforcedarmor.ha_armor : data_reinforcedarmor.ma_armor );
}

void EL_Reinforcedarmor::setArmorInfo( )
{
	player_data[ID].maxAP = class_base_ap_lvl3[player_data[ID].pClass]
				+ ( ( player_data[ID].pClass == NS_CLASS_HEAVY ) ? ha_armor : ma_armor );
}

/*void EL_Reinforcedarmor::Think( )
{
	if ( cur_level == 0 )
		return;
	
	if ( pEntity->v.armorvalue < class_base_ap_lvl3[player_data[ID].pClass]
		|| pEntity->v.armorvalue >= player_data[ID].maxAP )
		return;
	
	if ( player_data[ENTINDEX(pEntity)].curWeapon != NS_WEAPON_WELDER
		|| !( pEntity->v.button & IN_ATTACK ) )
		return;
	
	if ( !weldedRA )
	{
		if ( gpGlobals->time < nextWeld )
			return;
		
		nextWeld = gpGlobals->time + WELD_TIME;
		
		Vector aimVec;
		Vector eyeVec = pEntity->v.origin + pEntity->v.view_ofs;
		AngleVectors(pEntity->v.v_angle, aimVec, NULL, NULL);
		TraceResult trResult;
		Vector traceVec = eyeVec + aimVec * WELD_DISTANCE;
		TRACE_LINE(eyeVec, traceVec, 0, pEntity, &trResult);
		
		edict_t *aimEntity = FNullEnt(trResult.pHit) ? NULL : trResult.pHit;
		if ( aimEntity == NULL )
			return;
		
		if ( ENTINDEX(aimEntity) < 1
			|| ENTINDEX(aimEntity) > gpGlobals->maxClients
			|| (pEntity->v.origin - Hive_ID->v.origin).Length() < WELD_RANGE
			|| pEntity->v.team != aimEntity->v.team
			|| UTIL_isAlive(aimEntity) == false )
			return;
		
		byte aimID = ENTINDEX(aimEntity);
		if ( class_base_ap_lvl3[player_data[aimID].pClass] <= aimEntity->v.armorvalue
			&& aimEntity->v.armorvalue < player_data[aimID].maxAP
			&& player_data[aimID].team == MARINE )
		{
			aimEntity->v.armorvalue += ARMOR_ADD_PER_WELD;
			if ( aimEntity->v.armorvalue > player_data[aimID].maxAP )
				aimEntity->v.armorvalue = player_data[aimID].maxAP;
			
			weldedRA = false;
			if ( !weldingRA )
			{
				EMIT_SOUND_DYN2(pEntity, CHAN_AUTO, RA_sound_files[RA_sound_welderhit], 0.5, ATTN_NORM, 0, PITCH_NORM);
				EMIT_SOUND_DYN2(pEntity, CHAN_STREAM, RA_sound_files[RA_sound_welderidle], 0.5, ATTN_NORM, 0, PITCH_NORM);
				weldingRA = true;
			}
		}else if ( weldingRA )
			weldedRA = true;
		
	}else if ( weldedRA || weldingRA )
	{
		EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, RA_sound_files[RA_sound_welderhit], 0.0, ATTN_NORM, SND_STOP, PITCH_NORM);
		EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, RA_sound_files[RA_sound_welderidle], 0.0, ATTN_NORM, SND_STOP, PITCH_NORM);
		
		weldedRA = false;
		weldingRA = false;
	}
}
*/

void EL_Reinforcedarmor::Think( )
{
	if ( data_reinforcedarmor.available == false )
		return;
	
	//if ( pEntity->v.armorvalue < class_base_ap_lvl3[player_data[ID].pClass]
	//	|| pEntity->v.armorvalue >= player_data[ID].maxAP )
	//	return;
	
	if ( player_data[ID].curWeapon == NS_WEAPON_WELDER
		&& pEntity->v.button & IN_ATTACK
		&& !weldedRA )
	{
		if ( gpGlobals->time < nextWeld )
			return;
		
		nextWeld = gpGlobals->time + WELD_TIME;
		
		Vector aimVec;
		Vector eyeVec = pEntity->v.origin + pEntity->v.view_ofs;
		AngleVectors(pEntity->v.v_angle, aimVec, NULL, NULL);
		TraceResult trResult;
		Vector traceVec = eyeVec + aimVec * WELD_DISTANCE;
		TRACE_LINE(eyeVec, traceVec, 0, pEntity, &trResult);
		
		edict_t *aimEntity = FNullEnt(trResult.pHit) ? NULL : trResult.pHit;
		if ( aimEntity == NULL )
			return;
		
		byte aimID = ENTINDEX(aimEntity);
		if ( aimID < 1
			|| aimID > gpGlobals->maxClients
			|| (pEntity->v.origin - aimEntity->v.origin).Length() > WELD_RANGE
			|| pEntity->v.team != aimEntity->v.team
			|| UTIL_isAlive(aimEntity) == false )
			return;
		
		if ( class_base_ap_lvl3[player_data[aimID].pClass] <= aimEntity->v.armorvalue
			&& aimEntity->v.armorvalue < player_data[aimID].maxAP
			&& player_data[aimID].team == MARINE )
		{
			aimEntity->v.armorvalue += ARMOR_ADD_PER_WELD;
			if ( aimEntity->v.armorvalue > player_data[aimID].maxAP )
				aimEntity->v.armorvalue = player_data[aimID].maxAP;
			
			weldedRA = false;
			if ( !weldingRA )
			{
				EMIT_SOUND_DYN2(pEntity, CHAN_AUTO, RA_sound_files[RA_sound_welderhit], 0.5, ATTN_NORM, 0, PITCH_NORM);
				EMIT_SOUND_DYN2(pEntity, CHAN_STREAM, RA_sound_files[RA_sound_welderidle], 0.5, ATTN_NORM, 0, PITCH_NORM);
				weldingRA = true;
			}
		}else if ( weldingRA )
			weldedRA = true;
	}else if ( weldedRA || weldingRA )
	{
		EMIT_SOUND_DYN2(pEntity, CHAN_AUTO, RA_sound_files[RA_sound_welderhit], 0.0, ATTN_NORM, SND_STOP, PITCH_NORM);
		EMIT_SOUND_DYN2(pEntity, CHAN_STREAM, RA_sound_files[RA_sound_welderidle], 0.0, ATTN_NORM, SND_STOP, PITCH_NORM);
		
		weldedRA = false;
		weldingRA = false;
	}
}

void EL_Reinforcedarmor::Think_Post( )
{
	if ( data_reinforcedarmor.available == false )
		return;
	
	if ( cur_level > 0 )
	{
		pEntity->v.fuser2 = MAX_ARMOR_VALUE / player_data[ID].maxAP * pEntity->v.armorvalue;
	}
}
