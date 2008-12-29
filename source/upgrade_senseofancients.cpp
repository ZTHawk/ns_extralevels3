// check if pev_viewmodel and pev_weaponmodel need to be updated every time or just HideWeapons

#include "upgrade_senseofancients.h"
#include "upgrade_etherealshift.h"
#include "upgrade_bloodlust.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_Senseofancients data_senseofancients;
EL_Senseofancients player_senseofancients[MAX_PLAYERS_PLUS1];

void Upgrade_Senseofancients::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = SENSEOFANCIENTS), TYPE_BOOL, (char *)SOA_config_names[0]},
		{&(req_points = SOA_COST), TYPE_INT, (char *)SOA_config_names[1]},
		{&(max_level = SOA_MAX), TYPE_INT, (char *)SOA_config_names[2]},
		{&(req_level = SOA_LEVEL), TYPE_INT, (char *)SOA_config_names[3]},
		{&(ParasiteChanceInit = SOA_PARASITE_INIT), TYPE_FLOAT, (char *)SOA_config_names[4]},
		{&(ParasiteChanceBonus = SOA_PARASITE_ADD), TYPE_FLOAT, (char *)SOA_config_names[5]},
		{&(ParasiteDMGPercentage = SOA_PARASITE_DMG), TYPE_FLOAT, (char *)SOA_config_names[6]},
		{&(HealsprayPercentage = SOA_HEALSPRAY_DMG), TYPE_FLOAT, (char *)SOA_config_names[7]},
		{&(SporeDMG = SOA_GASDAMAGE), TYPE_FLOAT, (char *)SOA_config_names[8]},
		{&(BlinkEnergyPercentage = SOA_BLINK_ENERGY_BONUS), TYPE_FLOAT, (char *)SOA_config_names[9]},
		{&(DevourAdder = SOA_DEVOUR_ADDER), TYPE_INT, (char *)SOA_config_names[10]},
		{&(DevourTimeInit = SOA_DEVOURTIME_INIT), TYPE_FLOAT, (char *)SOA_config_names[11]},
		{&(DevourTimeBonus = SOA_DEVOURTIME_BONUS), TYPE_FLOAT, (char *)SOA_config_names[12]},
		{&(GestateArmorBonus = SOA_GESTATE_ARMOR_ADD), TYPE_FLOAT, (char *)SOA_config_names[13]}
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	//req_level = 10;
	
	strcpy(upgrade_name, "Sense of Ancients");
	strcpy(upgrade_description, "Skulk: Parasite [+%d%%] damage, infects players in range by chance [%d%%] over 3 seconds\n"
					"Gorge: Stronger Healspray [+%d%%]\nLerk: Gas with [%d] damage to armor of HA\n"
					"Fade: Blink energy reduced by [%d%%]\n"
					"Onos: Devour [%d] more player%s, with a cooldown time of [%2.1f] second%s between devours\n"
					"Gestate: Armor increased by [+%d]\n\n"
					"Requires: 1 Upgrade of each Upgrade Chamber , Level %d, %d Point%s\n\n"
					"Next level [%d]\n\n"
					"%s%s\n\n"
					"0. Exit\n\n\n\n\n\n\n\n\n\n\n");
	
	team = ALIEN;
	max_marine_points += available * max_level * req_points;
	
	Spore_Num = 0;
	nextSporeTime = 0.0;
}

void Upgrade_Senseofancients::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s   (Disabled)\n", upgrade_name);
	}else if ( player_senseofancients[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s   ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s   ( %3i / %3i )\n", num, upgrade_name, player_senseofancients[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
}

void Upgrade_Senseofancients::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	int plevel = player_senseofancients[ID].cur_level + 1;
	float dev_time_mult = ( plevel % DevourAdder == 1 ) ? 1.0 : (float)( player_senseofancients[ID].DevourTimeMultiplier + 1 );
	float devour_time = DevourTimeInit - ( dev_time_mult * DevourTimeBonus );
	int players_to_devour = ((plevel - 1) / DevourAdder) + 1;
	
	bool req_correct = player_senseofancients[ID].check_Requirements();
	sprintf(menu, dummy_description,
			(int)(ParasiteDMGPercentage * (float)plevel),
			(int)(ParasiteChanceInit + (float)plevel * ParasiteChanceBonus),
			(int)(HealsprayPercentage * (float)plevel),
			(int)(SporeDMG * (float)plevel),
			(int)(BlinkEnergyPercentage * (float)plevel),
			players_to_devour,
			( players_to_devour > 1 ) ? "s" : "",
			devour_time,
			( devour_time != 1.0 ) ? "s" : "",
			(int)((float)plevel * GestateArmorBonus),
			req_level + player_senseofancients[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			plevel,
			req_correct ? "2. Buy " : "",
			req_correct ? upgrade_name : ""
			);
	
	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;
	
	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

void Upgrade_Senseofancients::SporeEmulationTimer( )
{
	if ( gpGlobals->time < nextSporeTime )
		return;
	
	nextSporeTime = gpGlobals->time + SPORE_EMU_TIME;
	
	if ( SporeData.empty() == true )
		return;
	
	byte targetID;
	edict_t *pEntity = NULL;
	vector<SporeData_t>::iterator iter = SporeData.begin();
	while ( iter != SporeData.end() )
	{
		if ( (*iter).Status >= 6 )
		{
			iter = SporeData.erase(iter);
			
			// this check is needed otherwise it will crash
			if ( SporeData.empty() == true )
				break;
			
			continue;
		}
		
		for ( targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
		{
			if ( player_data[targetID].ingame == false )
				continue;
			
			// check if in spore ( prevent stacking )
			if ( player_senseofancients[targetID].In_Spore )
				continue;
			
			// check if player is MARINE ( if not than spore has no affect )
			if ( player_data[targetID].team != MARINE )
			{
				player_senseofancients[targetID].In_Spore = true;
				continue;
			}
			
			// check distance ( if inside spore )
			pEntity = INDEXENT(targetID);
			if ( (pEntity->v.origin - (Vector)(*iter).Origin).Length() > SPORE_RANGE )
				continue;
			
			// check if player is alive and heavy
			if ( UTIL_isAlive(pEntity)
				&& player_data[targetID].pClass == CLASS_HEAVY )
			{
				pEntity->v.armorvalue -= ( (float)((int)(*iter).SporeLevel) * SporeDMG );
				if ( pEntity->v.armorvalue < 0.0 )
					pEntity->v.armorvalue = 0.0;
			}
			
			// mark player as checked
			player_senseofancients[targetID].In_Spore = true;
		}
		
		++(*iter).Status;
		
		// get next element
		++iter;
	}
	
	for ( targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
		player_senseofancients[targetID].In_Spore = false;
}

//////////

void EL_Senseofancients::reset( )
{
	cur_level = 0;
	nextParasiteTime = 0.0;
	ParasiteChance = 0;
	reset_parasite_vars();
	
	In_Spore = false;
	
	Fade_Energy = 0.0;
	JustBlinked = false;
	got_BloodLust = 0.0;
	BlinkEnergyReducer = 0.0;
	
	DevourTime = 0.0;
	DevourTimeMultiplier = 0;
	NextDevour = 0.0;
	EvolveMsgBlock_time = 0.0;
	JustDevoured = false;
	DevourMaxAmount = 0;
	JustRedeemed = false;
	JustFreed = false;
	reset_devour_vars();
	
	Vector Origin_before_Redeem = Vector(0.0, 0.0, 0.0);
}

void EL_Senseofancients::reset_basic( )
{
	reset_parasite_vars();
	JustBlinked = false;
	reset_devour_vars();
}

void EL_Senseofancients::reset_parasite_vars( )
{
	ParasiteMode = PARASITE_NONE;
	Infected_by = 0;
	MyParasiter = 0;
}

void EL_Senseofancients::reset_devour_vars( )
{
	if ( MyDigester > 0 )
	{
		if ( ID == player_senseofancients[MyDigester].CurrentlyDigesting )
			player_senseofancients[MyDigester].CurrentlyDigesting = 0;
		
		// make sure we are not setting negativ values
		if ( player_senseofancients[MyDigester].DevourPlayersNum > 0 )
			--player_senseofancients[MyDigester].DevourPlayersNum;
	}
	
	MyDigester = 0;
	DigestTime = 0.0;
	DevourPlayersNum = 0;
}

bool EL_Senseofancients::check_Requirements( )
{
	return ( player_data[ID].level >= data_senseofancients.req_level + cur_level
		&& player_data[ID].points_available >= data_senseofancients.req_points
		&& cur_level < data_senseofancients.max_level
		&& ( UTIL_getMask(pEntity, MASK_CARAPACE)
			|| UTIL_getMask(pEntity, MASK_REGENERATION)
			|| UTIL_getMask(pEntity, MASK_REDEMPTION) )
		&& ( UTIL_getMask(pEntity, MASK_CELERITY)
			|| UTIL_getMask(pEntity, MASK_ADRENALINE)
			|| UTIL_getMask(pEntity, MASK_SILENCE) )
		&& ( ( UTIL_getMask(pEntity, MASK_CLOAKING)
				|| player_etherealshift[ID].Shifting )
			|| UTIL_getMask(pEntity, MASK_FOCUS)
			|| UTIL_getMask(pEntity, MASK_SCENTOFFEAR) ) );
}

void EL_Senseofancients::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	player_data[ID].gestation_Emulation = GESTATE_START;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_addPoints(pEntity, data_senseofancients.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_senseofancients.max_level,
							data_senseofancients.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Senseofancients::set_upgrade_values( )
{
	ParasiteChance = (byte)(data_senseofancients.ParasiteChanceInit + (float)cur_level * data_senseofancients.ParasiteChanceBonus);
	
	BlinkEnergyReducer = (float)cur_level * data_senseofancients.BlinkEnergyPercentage;
	
	DevourTimeMultiplier = cur_level % data_senseofancients.DevourAdder;
	if ( DevourTimeMultiplier == 0 )
		DevourTimeMultiplier = data_senseofancients.DevourAdder;
	DevourMaxAmount = ((cur_level - 1) / data_senseofancients.DevourAdder) + 1;
	DevourTime = data_senseofancients.DevourTimeInit - ( (float)DevourTimeMultiplier * data_senseofancients.DevourTimeBonus );
	
	if ( player_data[ID].pClass == CLASS_SKULK )
		//|| player_data[ID].pClass == CLASS_GORGE )
		setWeaponData_Dmg();
}

void EL_Senseofancients::respawned( )
{
	SpawnTime = gpGlobals->time + 0.1;
}

void EL_Senseofancients::Think( )
{
	// no level check here cause enemies are those who need the check
	Parasite_Players();
	
	if ( MyDigester > 0 )
		Digest_Player();
	else if ( JustFreed == true )
		pEntity->v.takedamage = DAMAGE_AIM;
	else if ( player_data[ID].pClass != CLASS_ONOS )
		find_Digester();
	
	if ( cur_level == 0 )
		return;
	
	// VERY IMPORTANT CRASH FIX
	// seems that after a spawn a players weapon sometimes does not have the private data set immediatly
	// so wait a little bit before accessing it
	if ( SpawnTime > 0.0 )
	{
		if ( gpGlobals->time > SpawnTime )
		{
			setWeaponData_Dmg();
			SpawnTime = 0.0;
		}
		
		return;
	}
	
	Blink_Energy();
	check_for_next_Devour();
}

void EL_Senseofancients::Think_Post( )
{
	if ( JustFreed == false )
		return;
	
	JustFreed = false;
	pEntity->v.velocity = Vector(0.0, 0.0, 0.0);
}

void EL_Senseofancients::Parasite_Players( )
{
	switch ( ParasiteMode )
	{
		case PARASITE_INIT:
		{
			if ( UTIL_getMask(pEntity, MASK_PARASITED) == false )
				break;
			
			ParasiteMode = PARASITE_BY_SKULK_1;
			Infected_by = 0;
			nextParasiteTime = gpGlobals->time;
			break;
		}
		case PARASITE_BY_SKULK_1:
		case PARASITE_BY_SKULK_2:
		case PARASITE_BY_SKULK_3:
		{
			if ( gpGlobals->time >= nextParasiteTime )
			{
				nextParasiteTime = gpGlobals->time + PARASITE_TIME;
				
				byte chance_mode = player_senseofancients[MyParasiter].ParasiteChance;
				
				if ( chance_mode > MAX_PARASITE_CHANCE )
					chance_mode = MAX_PARASITE_CHANCE;
				
				edict_t *targetEntity = NULL;
				byte targetID = 1;
				byte chance = 0;
				byte team = pEntity->v.team;
				byte player_chance = 0;
				for ( targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
				{
					if ( targetID == ID )
						continue;
					
					if ( player_data[targetID].ingame == false )
						continue;
					
					targetEntity = INDEXENT(targetID);
					if ( targetEntity->v.team != team
						|| UTIL_isAlive(targetEntity) == false
						|| (targetEntity->v.origin - pEntity->v.origin).Length() > PARASITE_RANGE )
						continue;
					
					player_chance = RANDOM_LONG(1, 100);
					for ( chance = 0; chance < chance_mode; ++chance )
					{
						if ( rand_para_chance[chance] != player_chance )
							continue;
						
						UTIL_setMask(targetEntity, MASK_PARASITED, true);
						player_senseofancients[targetID].ParasiteMode = PARASITE_BY_MARINE_1;
						player_senseofancients[targetID].Infected_by = ID;
					}
					
					break;
				}
				
				++ParasiteMode;
			}
			if ( ParasiteMode == PARASITE_BY_SKULK_3 )
			{
				ParasiteMode = PARASITE_DONE;
				
				// to force HUD update
				UTIL_resetHUD(pEntity);
				player_data[ID].lastHUD_display = 0.0;
			}
			break;
		}
		case PARASITE_BY_MARINE_1:
		{
			ParasiteMode = PARASITE_BY_MARINE_2;
			nextParasiteTime = gpGlobals->time + PARASITE_TIME;
			break;
		}
		case PARASITE_BY_MARINE_2:
		{
			if ( nextParasiteTime > gpGlobals->time )
				break;
			
			ParasiteMode = PARASITE_BY_MARINE_3;
			nextParasiteTime = gpGlobals->time + PARASITE_TIME;
			break;
		}
		case PARASITE_BY_MARINE_3:
		{
			if ( nextParasiteTime > gpGlobals->time )
				break;
			
			ParasiteMode = PARASITE_BY_MARINE_4;
			nextParasiteTime = gpGlobals->time + PARASITE_TIME;
			break;
		}
		case PARASITE_BY_MARINE_4:
		{
			if ( nextParasiteTime > gpGlobals->time )
				break;
			
			ParasiteMode = PARASITE_DONE;
			break;
		}
		
	}
}

void EL_Senseofancients::check_Parasite( )
{
	// already parasited
	if ( ParasiteMode > PARASITE_NONE )
		return;
	
	edict_t *entAttackerWeapon = pEntity->v.dmg_inflictor;
	if ( FNullEnt(entAttackerWeapon) )
		return;
	
	// after spawn weapons sometimes do not have private data set immediatly
	// so we need a check to prevent a crash
	if ( entAttackerWeapon->pvPrivateData == NULL )
		return;
	
	// make sure it is a correct entity so its private data has correct size (prevent rare crash)
	if ( strcmp(STRING(entAttackerWeapon->v.classname), "weapon_parasite") != 0 )
		return;
	
	if ( get_private(entAttackerWeapon, MAKE_OFFSET(WEAPON_ID)) != WEAPON_PARASITE )
		return;
	
	ParasiteMode = PARASITE_INIT;
	MyParasiter = ENTINDEX(entAttackerWeapon->v.owner);
}

void EL_Senseofancients::setWeaponData_Dmg( )
{
	if ( cur_level == 0 )
		return;
	
	float damage_to_set = 0.0;
	byte foundID = 0;
	int WeaponID = 0;
	edict_t *Ent = NULL;
	for ( int entID = gpGlobals->maxClients + 1; entID <= gpGlobals->maxEntities; ++entID )
	{
		Ent = INDEXENT(entID);
		if ( FNullEnt(Ent) )
			continue;
		
		if ( Ent->v.owner != pEntity )
			continue;
		
		WeaponID = get_private(Ent, MAKE_OFFSET(WEAPON_ID));
		switch ( WeaponID )
		{
			case WEAPON_PARASITE:
			{
				damage_to_set = BasicDmg[WeaponID] + (float)cur_level * ( BasicDmg[WeaponID] / 100.0 * data_senseofancients.ParasiteDMGPercentage );
				foundID = WeaponID;
				break;
			}
		}
		
		if ( !foundID )
			continue;
		
		UTIL_setWeaponDamage(Ent, damage_to_set);
		
		break;
	}
}

void EL_Senseofancients::check_HealingSpray( )
{
	if ( cur_level == 0 )
		return;
	
	edict_t *targetEnt = NULL;
	for ( int entID = 1; entID <= gpGlobals->maxEntities; ++entID )
	{
		targetEnt = INDEXENT(entID);
		if ( FNullEnt(targetEnt) )
			continue;
		
		if ( pEntity->v.team != targetEnt->v.team )
			continue;
		
		if ( UTIL_isAlive(targetEnt) == false )
			continue;
		
		if ( entID == ID )
		{
			add_hp_ap(targetEnt,
				SLEF_HEALSPRAY_HP + (float)cur_level * (SLEF_HEALSPRAY_HP / 100.0 * data_senseofancients.HealsprayPercentage),
				SELF_HEALSPRAY_AP + (float)cur_level * (SELF_HEALSPRAY_AP / 100.0 * data_senseofancients.HealsprayPercentage));
			
			continue;
		}
		
		if ( UTIL_is_in_viewcone(pEntity, targetEnt) == false )
			continue;
		
		switch ( targetEnt->v.iuser3 )
		{
			case AVH_USER3_ALIEN_PLAYER1:	// Skulk
			{
				if ( (pEntity->v.origin - targetEnt->v.origin).Length() < OTHER_HEALSPRAY_RANGE )
					add_hp_ap(targetEnt,
						SKULK_HEALSPRAY_HP + (float)cur_level * (SKULK_HEALSPRAY_HP / 100.0 * data_senseofancients.HealsprayPercentage),
						SKULK_HEALSPRAY_AP + (float)cur_level * (SKULK_HEALSPRAY_AP / 100.0 * data_senseofancients.HealsprayPercentage));
				break;
			}
			case AVH_USER3_ALIEN_PLAYER2:	// Gorge
			{
				if ( (pEntity->v.origin - targetEnt->v.origin).Length() < OTHER_HEALSPRAY_RANGE )
					add_hp_ap(targetEnt,
						GORGE_HEALSPRAY_HP + (float)cur_level * (GORGE_HEALSPRAY_HP / 100.0 * data_senseofancients.HealsprayPercentage),
						GORGE_HEALSPRAY_AP + (float)cur_level * (GORGE_HEALSPRAY_AP / 100.0 * data_senseofancients.HealsprayPercentage));
				break;
			}
			case AVH_USER3_ALIEN_PLAYER3:	// Lerk
			{
				if ( (pEntity->v.origin - targetEnt->v.origin).Length() < OTHER_HEALSPRAY_RANGE )
					add_hp_ap(targetEnt,
						LERK_HEALSPRAY_HP + (float)cur_level * (LERK_HEALSPRAY_HP / 100.0 * data_senseofancients.HealsprayPercentage),
						LERK_HEALSPRAY_AP + (float)cur_level * (LERK_HEALSPRAY_AP / 100.0 * data_senseofancients.HealsprayPercentage));
				break;
			}
			case AVH_USER3_ALIEN_PLAYER4:	// Fade
			{
				if ( (pEntity->v.origin - targetEnt->v.origin).Length() < OTHER_HEALSPRAY_RANGE )
					add_hp_ap(targetEnt,
						FADE_HEALSPRAY_HP + (float)cur_level * (FADE_HEALSPRAY_HP / 100.0 * data_senseofancients.HealsprayPercentage),
						FADE_HEALSPRAY_AP + (float)cur_level * (FADE_HEALSPRAY_AP / 100.0 * data_senseofancients.HealsprayPercentage));
				break;
			}
			case AVH_USER3_ALIEN_PLAYER5:	// Onos
			{
				if ( (pEntity->v.origin - targetEnt->v.origin).Length() < ONOS_HEALSPRAY_RANGE )
					add_hp_ap(targetEnt,
						ONOS_HEALSPRAY_HP + (float)cur_level * (ONOS_HEALSPRAY_HP / 100.0 * data_senseofancients.HealsprayPercentage),
						ONOS_HEALSPRAY_AP + (float)cur_level * (ONOS_HEALSPRAY_AP / 100.0 * data_senseofancients.HealsprayPercentage));
				break;
			}
			case AVH_USER3_ALIEN_EMBRYO:	// Gestate
			{
				if ( (pEntity->v.origin - targetEnt->v.origin).Length() < OTHER_HEALSPRAY_RANGE )
					add_hp_ap(targetEnt,
						GESTATE_HEALSPRAY_HP + (float)cur_level * (GESTATE_HEALSPRAY_HP / 100.0 * data_senseofancients.HealsprayPercentage),
						GESTATE_HEALSPRAY_AP + (float)cur_level * (GESTATE_HEALSPRAY_AP / 100.0 * data_senseofancients.HealsprayPercentage));
				break;
			}
			case AVH_USER3_HIVE:
			{
				if ( (pEntity->v.origin - targetEnt->v.origin).Length() < HIVE_HEALSPRAY_RANGE )
					add_hp_ap_structure(targetEnt,
						STRUCTURE_HEALSPRAY_HP + (float)cur_level * (STRUCTURE_HEALSPRAY_HP / 100.0 * data_senseofancients.HealsprayPercentage));
				break;
			}
			case AVH_USER3_DEFENSE_CHAMBER:
			case AVH_USER3_MOVEMENT_CHAMBER:
			case AVH_USER3_OFFENSE_CHAMBER:
			case AVH_USER3_SENSORY_CHAMBER:
			{
				if ( (pEntity->v.origin - targetEnt->v.origin).Length() < CHAMBER_HEALSPRAY_RANGE )
					add_hp_ap_structure(targetEnt,
						STRUCTURE_HEALSPRAY_HP + (float)cur_level * (STRUCTURE_HEALSPRAY_HP / 100.0 * data_senseofancients.HealsprayPercentage));
				break;
			}
			case AVH_USER3_ALIENRESTOWER:
			{
				if ( (pEntity->v.origin - targetEnt->v.origin).Length() < ALIEN_RT_HEALSPRAY_RANGE )
					add_hp_ap_structure(targetEnt,
						STRUCTURE_HEALSPRAY_HP + (float)cur_level * (STRUCTURE_HEALSPRAY_HP / 100.0 * data_senseofancients.HealsprayPercentage));
				break;
			}
		}
	}
}

void EL_Senseofancients::add_hp_ap( edict_t *targetEntity , float health , float armor )
{
	byte targetID = ENTINDEX(targetEntity);
	if ( targetEntity->v.health < player_data[targetID].maxHP )
	{
		if ( targetEntity->v.health + health <= player_data[targetID].maxHP )
			targetEntity->v.health += health;
		else
			targetEntity->v.health = player_data[targetID].maxHP;
	}else if ( targetEntity->v.armorvalue < player_data[targetID].maxAP )
	{
		if ( targetEntity->v.armorvalue + armor <= player_data[targetID].maxAP )
			targetEntity->v.armorvalue += armor;
		else
			targetEntity->v.armorvalue = player_data[targetID].maxAP;
	}
}

void EL_Senseofancients::add_hp_ap_structure( edict_t *targetEntity , float health )
{
	short targetID = ENTINDEX(targetEntity);
	if ( targetEntity->v.health < targetEntity->v.max_health )
	{
		if ( targetEntity->v.health + health <= targetEntity->v.max_health )
			targetEntity->v.health += health;
		else
			targetEntity->v.health = targetEntity->v.max_health;
	}
}

void EL_Senseofancients::add_Spore( const edict_t *sporeEntity )
{
	if ( cur_level == 0 )
		return;
	
	SporeData_t SporeData_to_add = {cur_level, 0, sporeEntity->v.origin};
	data_senseofancients.SporeData.push_back(SporeData_to_add);
	++data_senseofancients.Spore_Num;
}

void EL_Senseofancients::Blink_Energy( )
{
	if ( player_data[ID].curWeapon != WEAPON_BLINK )
		return;
	
	if ( pEntity->v.button & IN_ATTACK
		&& JustBlinked == false )
	{
		JustBlinked = true;
	}else
	{
		float energy_Bonus = 0.0;
		if ( got_BloodLust > 0.0 )
		{
			energy_Bonus = Fade_Energy - (pEntity->v.fuser3 - got_BloodLust);
			got_BloodLust = 0.0;
		}else
			energy_Bonus = Fade_Energy - pEntity->v.fuser3;
		
		if ( energy_Bonus > 0.0 )
		{
			pEntity->v.fuser3 += energy_Bonus / 100.0 * BlinkEnergyReducer;
			JustBlinked = false;
		}
	}
	
	Fade_Energy = pEntity->v.fuser3;
}

void EL_Senseofancients::Digest_Player( )
{
	if ( ID == player_senseofancients[MyDigester].CurrentlyDigesting )
		return;
	
	edict_t *pDigester = INDEXENT(MyDigester);
	
	// update origin of player being digested
	SET_SIZE(pEntity, pEntity->v.mins, pEntity->v.maxs);
	SET_ORIGIN(pEntity, pDigester->v.origin);
	
	//pEntity->v.viewmodel = ALLOC_STRING("");
	//pEntity->v.weaponmodel = ALLOC_STRING("");
	int hl_strings_model_id = hl_strings.find("");
	pEntity->v.viewmodel = hl_strings_model_id;
	pEntity->v.weaponmodel = hl_strings_model_id;
	
	if ( gpGlobals->time < DigestTime )
		return;
	
	DigestTime = gpGlobals->time + DIGEST_TIME;
	
	add_hp_ap(pDigester, DEVOUR_HP_REGEN, DEVOUR_HP_REGEN);
	
	if ( pEntity->v.health - DEVOUR_HP_DEGEN < 1.0 )
	{
		// this dummy ID is needed cause after death "MyDigester" will be reseted
		// and to not mess much with other function a temp var will do all we need
		byte dummyID = MyDigester;
		player_data[ID].killPlayer();
		player_data[ID].newDeahthMsg(dummyID, "devour");
		reset_devour_vars();
	}else
		pEntity->v.health -= DEVOUR_HP_DEGEN;
}

void EL_Senseofancients::check_for_next_Devour( )
{
	if ( UTIL_getMask(pEntity, MASK_DIGESTING) )
	{
		if ( JustDevoured == false )
		{
			JustDevoured = true;
			NextDevour = gpGlobals->time;
			++DevourPlayersNum;
		}else if ( gpGlobals->time - NextDevour > DevourTime
			&& DevourPlayersNum < DevourMaxAmount + 1 )
		{
			UTIL_setMask(pEntity, MASK_DIGESTING, false);
			NextDevour = gpGlobals->time;
			JustDevoured = false;
		}
	}else
		JustDevoured = false;
}

void EL_Senseofancients::find_Digester( )
{
	if ( UTIL_getMask(pEntity, MASK_DIGESTING) == false	// check if being digested
		|| !( pEntity->v.effects & EF_NODRAW ) )	// check if player is really being digested or devouring someone else
		return;
	
	byte Onos_digesting_me = get_MyOnos();
	if ( player_senseofancients[Onos_digesting_me].cur_level <= 0 )
		return;
	
	MyDigester = Onos_digesting_me;
	player_senseofancients[MyDigester].CurrentlyDigesting = ID;
}

byte EL_Senseofancients::get_MyOnos( )
{
	// Onos with shortest distance must be the Digester of current player
	float range = 1000.0;
	byte MyOnos_ID = 0;
	float temp_range = 0.0;
	edict_t *targetEntity = NULL;
	for ( byte targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( player_data[targetID].ingame == false )
			continue;
		
		if ( targetID == ID )
			continue;
		
		if ( player_data[targetID].pClass != CLASS_ONOS )
			continue;
		
		targetEntity = INDEXENT(targetID);
		temp_range = (targetEntity->v.origin - pEntity->v.origin).Length();
		if ( temp_range >= range )
			continue;
		
		range = temp_range;
		MyOnos_ID = targetID;
	}
	
	return MyOnos_ID;
}

void EL_Senseofancients::free_digested_Players( )
{
	set_private(pEntity, MAKE_OFFSET(DIGESTING_PLAYER), 0);	// the ID of player the onos is digesting right now
	
	edict_t *targetEntity = NULL;
	for ( byte targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( player_data[targetID].ingame == false )
			continue;
		
		if ( player_senseofancients[targetID].MyDigester != ID )
			continue;
		
		targetEntity = INDEXENT(targetID);
		if ( targetEntity->v.health < 1.0 )
			continue;
		
		UTIL_setMask(targetEntity, MASK_DIGESTING, 0);
		targetEntity->v.controller[2] = 0;
		targetEntity->v.solid = 3;
		targetEntity->v.effects = 0;
		targetEntity->v.flags = targetEntity->v.flags & ~FL_ONGROUND | FL_DUCKING;	// remove ONGROUND but add DUCKING
		targetEntity->v.weaponanim = 2;
		targetEntity->v.flFallVelocity = 0.0;
		targetEntity->v.fuser2 = 1000.0;
		targetEntity->v.fuser3 = 0.0;
		
		// remove the effect that makes the sound being muffled
		set_private(targetEntity, MAKE_OFFSET(SOUND_EFFECTS), 0);
		
		// just to be sure
		targetEntity->v.rendermode = kRenderNormal;
		
		// set correct origin of digested player
		SET_SIZE(targetEntity, targetEntity->v.mins, targetEntity->v.maxs);
		SET_ORIGIN(targetEntity, Origin_before_Redeem);
		
		// prevention so player will not die after released from onos
		targetEntity->v.velocity = Vector(0.0, 0.0, 0.0);
		// some players may get godmode, so we need to fix that in next PreThink
		player_senseofancients[targetID].JustFreed = true;
		
		player_senseofancients[targetID].set_Player_Weaponmodel();
		
		player_senseofancients[targetID].reset_devour_vars();
	}
	
	reset_devour_vars();
}

void EL_Senseofancients::set_Player_Weaponmodel( )
{
	byte weapon_array = MODEL_KNIFE;	// default, WEAPON_KNIFE
	int weapons = pEntity->v.weapons & ~(1<<31);
	if ( weapons & (1<<WEAPON_GRENADE_GUN) )
		weapon_array = MODEL_GL;
	else if ( weapons & (1<<WEAPON_HMG) )
		weapon_array = MODEL_HMG;
	else if ( weapons & (1<<WEAPON_SHOTGUN) )
		weapon_array = MODEL_SHOTGUN;
	else if ( weapons & (1<<WEAPON_LMG) )
		weapon_array = MODEL_LMG;
	else if ( weapons & (1<<WEAPON_PISTOL) )
		weapon_array = MODEL_PISTOL;
	
	int hl_strings_model_id = 0;
	if ( player_data[ID].pClass == CLASS_HEAVY )
	{
		//pEntity->v.viewmodel = ALLOC_STRING(View_Models_Heavy[weapon_array]);
		hl_strings_model_id = hl_strings.find(View_Models_Heavy[weapon_array]);
		pEntity->v.viewmodel = hl_strings_model_id;
	}else
	{
		//pEntity->v.viewmodel = ALLOC_STRING(View_Models[weapon_array]);
		hl_strings_model_id = hl_strings.find(View_Models[weapon_array]);
		pEntity->v.viewmodel = hl_strings_model_id;
	}

	//pEntity->v.weaponmodel = ALLOC_STRING(Weapon_Models[weapon_array]);
	hl_strings_model_id = hl_strings.find(Weapon_Models[weapon_array]);
	pEntity->v.viewmodel = hl_strings_model_id;
}

void EL_Senseofancients::Player_Redeemed( float *Origin )
{
	if ( cur_level == 0 )
		return;
	
	if ( player_data[ID].pClass != CLASS_ONOS )
		return;
	
	if ( JustRedeemed == false )
	{
		Origin_before_Redeem = Vector(Origin[0], Origin[1], Origin[2]);
		JustRedeemed = true;	// Cloak Event is send twice, so prevent following function from being called twice
		free_digested_Players();
	}else
		JustRedeemed = false;
}

void EL_Senseofancients::setGestateAP( )
{
	player_data[ID].player_gestate_ap += data_senseofancients.GestateArmorBonus * (float)cur_level;
	player_data[ID].maxAP += data_senseofancients.GestateArmorBonus * (float)cur_level;
}

void EL_Senseofancients::ReadyRoom( )
{
	if ( cur_level == 0 )
		return;
	
	Origin_before_Redeem = pEntity->v.origin;
	free_digested_Players();
}

bool EL_Senseofancients::ImpulseCheckBlock( )
{
	if ( cur_level == 0 )
		return false;
	
	if ( player_data[ID].pClass != CLASS_ONOS )
		return false;
	
	if ( DevourPlayersNum == 0
		&& UTIL_getMask(pEntity, MASK_DIGESTING) == false )
		return false;
	
	if ( gpGlobals->time >= EvolveMsgBlock_time )
		UTIL_showPopup(pEntity, (char *)No_Gestate_Digest_Msg);
	
	EvolveMsgBlock_time = gpGlobals->time + EVOLVE_TIME;
	
	return true;
}

bool EL_Senseofancients::setHUDText( byte vID , bool is_marine , hudtextparms_t &hud_params , char *CoreT_GL_reload_Shift_text)
{
	if ( ParasiteMode < PARASITE_BY_MARINE_1
		|| ParasiteMode > PARASITE_BY_MARINE_4 )
		return false;
	
	if ( nextParasiteTime <= gpGlobals->time )
		return false;
	
	sprintf(CoreT_GL_reload_Shift_text, "<< Infected by %s: You have been parasited >>\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", player_data[Infected_by].name);		// 24 * \n
	UTIL_setHUD(hud_params
		, is_marine ? MARINE_HUD_COLOR_R : ALIEN_HUD_COLOR_R
		, is_marine ? MARINE_HUD_COLOR_G : ALIEN_HUD_COLOR_G
		, is_marine ? MARINE_HUD_COLOR_B : ALIEN_HUD_COLOR_B
		, -1.0, UTIL_isAlive(INDEXENT(vID)) ? 0.421 : 0.351, 0, 0.0, 3600.0, 0.0, 0.0, HUD_CHANNEL);
	
	return true;
}

