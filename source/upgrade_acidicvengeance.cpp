
#include "upgrade_acidicvengeance.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_Acidicvengeance data_acidicvengeance;
EL_Acidicvengeance player_acidicvengeance[MAX_PLAYERS_PLUS1];

void Upgrade_Acidicvengeance::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = ACIDICVENGEANCE), TYPE_BOOL, (char *)AV_config_names[0]},
		{&(req_points = AV_COST), TYPE_INT, (char *)AV_config_names[1]},
		{&(max_level = AV_MAX), TYPE_INT, (char *)AV_config_names[2]},
		{&(req_level = AV_LEVEL), TYPE_INT, (char *)AV_config_names[3]},
		{&(MA_HealthDmg = AV_MA_HP), TYPE_FLOAT, (char *)AV_config_names[4]},
		{&(MA_ArmorDmg = AV_MA_AP), TYPE_FLOAT, (char *)AV_config_names[5]},
		{&(HA_HealthDmg = AV_HA_HP), TYPE_FLOAT, (char *)AV_config_names[6]},
		{&(HA_ArmorDmg = AV_HA_AP), TYPE_FLOAT, (char *)AV_config_names[7]},
		{&(GorgeGestateBonus = AV_GORGEGEST_PERCENTAGE), TYPE_FLOAT, (char *)AV_config_names[8]},
		{&(InitRange = AV_INITRANGE), TYPE_FLOAT, (char *)AV_config_names[9]},
		{&(ClassRange = AV_CLASSRANGE), TYPE_FLOAT, (char *)AV_config_names[10]}
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	strcpy(upgrade_name, "Acidic Vengeance");
	strcpy(upgrade_description, "Whenever you are killed, you leave an acidic cloud that damages your enemies.\n"
					"HA for [%2.1f] HP - [%2.1f] AP and others for [%2.1f] HP - [%2.1f] AP.\n"
					"Gorge and Gestate have an enhanced damage by [%d%%]. Range depends on your class, current range [%d].\n\n"
					"Requires: Hive Ability 2 , Sense of Fear , Level %d, %d Point%s\n\n"
					"Next level [%d]\n\n"
					"%s%s\n\n"
					"0. Exit\n\n\n\n\n\n\n\n");
	
	team = ALIEN;
	max_marine_points += available * max_level * req_points;
}

void Upgrade_Acidicvengeance::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s    (Disabled)\n", upgrade_name);
	}else if ( player_acidicvengeance[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s    ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s    ( %3i / %3i )\n", num, upgrade_name, player_acidicvengeance[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
}

void Upgrade_Acidicvengeance::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	float ha_dmg_HP = HA_HealthDmg * ( player_acidicvengeance[ID].cur_level + 1 );
	float ha_dmg_AP = HA_ArmorDmg * ( player_acidicvengeance[ID].cur_level + 1 );
	float ma_dmg_HP = MA_HealthDmg * ( player_acidicvengeance[ID].cur_level + 1 );
	float ma_dmg_AP = MA_ArmorDmg * ( player_acidicvengeance[ID].cur_level + 1 );
	
	bool req_correct = player_acidicvengeance[ID].check_Requirements();
	sprintf(menu, dummy_description,
			ha_dmg_HP,
			ha_dmg_AP,
			ma_dmg_HP,
			ma_dmg_AP,
			(int)GorgeGestateBonus,
			(int)(InitRange + (float)player_data[ID].pClass * ClassRange),
			req_level + player_acidicvengeance[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			player_acidicvengeance[ID].cur_level + 1,
			req_correct ? "2. Buy " : "",
			req_correct ? upgrade_name : ""
			);
	
	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;
	
	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

void Upgrade_Acidicvengeance::precache( )
{
	for ( int i = 0; i < AV_MAX_SOUNDS; ++i )
		PRECACHE_SOUND((char *)AV_sound_files[i]);
}

//////////

void EL_Acidicvengeance::reset( )
{
	cur_level = 0;
	ma_HealthDmg = 0.0;
	ma_ArmorDmg = 0.0;
	ha_HealthDmg = 0.0;
	ha_ArmorDmg = 0.0;
	respawned();
}

void EL_Acidicvengeance::respawned( )
{
	startedAV = AV_NONE;
}

bool EL_Acidicvengeance::check_Requirements( )
{
	return ( player_data[ID].level >= data_acidicvengeance.req_level + cur_level
		&& player_data[ID].points_available >= data_acidicvengeance.req_points
		&& cur_level < data_acidicvengeance.max_level
		&& UTIL_getMask(pEntity, MASK_SCENTOFFEAR)
		&& UTIL_getHiveAbility(pEntity, HIVE_3_WEAPONS) );
}

void EL_Acidicvengeance::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	player_data[ID].gestation_Emulation = GESTATE_START;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_addPoints(pEntity, data_acidicvengeance.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_acidicvengeance.max_level,
							data_acidicvengeance.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Acidicvengeance::set_upgrade_values( )
{
	ha_HealthDmg = data_acidicvengeance.HA_HealthDmg * cur_level;
	ha_ArmorDmg = data_acidicvengeance.HA_ArmorDmg * cur_level;
	ma_HealthDmg = data_acidicvengeance.MA_HealthDmg * cur_level;
	ma_ArmorDmg = data_acidicvengeance.MA_ArmorDmg * cur_level;
}

void EL_Acidicvengeance::Think( )
{
	if ( cur_level == 0 )
		return;
	
	if ( startedAV != AV_INIT )
		return;
	
	startedAV = AV_DONE;
	
	Vector temp = pEntity->v.origin;
	temp.z += 100.0;
	EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, AV_sound_files[AV_xenocide_explode], 1.0, ATTN_NORM, 0, PITCH_NORM);
	PLAYBACK_EVENT_FULL(0, pEntity, Particle_Event_ID, 0.0, temp, Vector(0, 0, 0), 0.0, 0.0, Xenocide_Particle_ID, 0, 0, 0);
	PLAYBACK_EVENT_FULL(0, pEntity, Particle_Event_ID, 0.0, temp, Vector(0, 0, 0), 0.0, 0.0, AcidHit_Particle_ID, 0, 0, 0);
	PLAYBACK_EVENT_FULL(0, pEntity, Particle_Event_ID, 0.0, temp, Vector(0, 0, 0), 0.0, 0.0, BileBomb_Particle_ID, 0, 0, 0);
	
	byte targetClass = 0;
	float health_damage = 0.0;
	float armor_damage = 0.0;
	byte curClass = player_data[ID].pClass;
	if ( curClass == CLASS_GESTATE )
		curClass = CLASS_GORGE;
	
	edict_t *targetEntity = NULL;
	for ( byte targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( targetID == ID )
			continue;
		
		if ( player_data[targetID].ingame == false )
			continue;
		
		if ( player_data[targetID].team == NO_TEAM )
			continue;
		
		targetEntity = INDEXENT(targetID);
		if ( UTIL_isAlive(targetEntity) == false )
			continue;
		
		if ( pEntity->v.team == targetEntity->v.team )
			continue;
		
		// check for invulnerability
		if ( pEntity->v.takedamage == DAMAGE_NO )
			continue;
		
		if ( (targetEntity->v.origin - pEntity->v.origin).Length()
			> ( data_acidicvengeance.InitRange
				+ (float)curClass * data_acidicvengeance.ClassRange ) )
			continue;
		
		targetClass = UTIL_getClass(targetEntity);
		if ( targetClass == CLASS_HEAVY
			|| targetClass == CLASS_ONOS
			|| targetClass == CLASS_FADE )
		{
			health_damage = ha_HealthDmg;
			armor_damage = ha_ArmorDmg;
		}else
		{
			health_damage = ma_HealthDmg;
			armor_damage = ma_ArmorDmg;
		}
		
		// no need to check for CLASS_GESTATE as it has been done before
		if ( curClass == CLASS_GORGE )
		{
			health_damage += ( health_damage / 100.0 * data_acidicvengeance.GorgeGestateBonus );
			armor_damage += ( armor_damage / 100.0 * data_acidicvengeance.GorgeGestateBonus );
		}
		
		if ( targetEntity->v.health - health_damage < 1.0 )
		{
			// set players points + score before killing victim because victims class will change to CLASS_DEAD
			player_data[ID].givePoints(targetID);
			player_data[targetID].killPlayer();
			player_data[targetID].newDeahthMsg(ID, "nuke");
		}else
		{
			targetEntity->v.health -= health_damage;
			targetEntity->v.armorvalue -= armor_damage;
			
			if ( targetEntity->v.armorvalue < 0.0 )
				targetEntity->v.armorvalue = 0.0;
		}
	}
}

void EL_Acidicvengeance::initAV( )
{
	if ( data_acidicvengeance.available == false )
		return;
	
	startedAV = AV_INIT;
}

