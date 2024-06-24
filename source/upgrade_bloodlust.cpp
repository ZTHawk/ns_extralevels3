
#include "upgrade_bloodlust.h"
#include "upgrade_senseofancients.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_Bloodlust data_bloodlust;
EL_Bloodlust player_bloodlust[MAX_PLAYERS_PLUS1];

void Upgrade_Bloodlust::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = BLOODLUST), TYPE_BOOL, (char *)BL_config_names[0]},
		{&(req_points = BL_COST), TYPE_INT, (char *)BL_config_names[1]},
		{&(max_level = BL_MAX), TYPE_INT, (char *)BL_config_names[2]},
		{&(req_level = BL_LEVEL), TYPE_INT, (char *)BL_config_names[3]},
		{&(baseAdrenalineRegen = BL_REGEN), TYPE_FLOAT, (char *)BL_config_names[4]},
		{&(OnosPercentage = BL_ONOSPERCENTAGE), TYPE_FLOAT, (char *)BL_config_names[5]},
		{&(baseVampirism = BL_VAMPIRISM), TYPE_FLOAT, (char *)BL_config_names[6]},
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	strcpy(upgrade_name, "BloodLust");
	strcpy(upgrade_description, "Increasing the rate at which you recharge energy\n"
					"Energy recharge is increased by [+%d%%], Onos [+%d%%] and upon kill you get an Adreneline boost\n"
					"When inflicting melee damage, you will steal [%2.1f] lifepoints. Fade and Onos only 50%%.\n"
					"HAs have a 50%% resistance.\n\n"
					"Requires: Adrenaline , Level %d, %d Point%s\n\n"
					"Next level [%d]\n\n"
					"%s%s\n\n"
					"0. Exit\n\n\n\n\n\n\n\n\n");
	
	team = ALIEN;
	max_alien_points += available * max_level * req_points;
}

bool Upgrade_Bloodlust::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s                 (Disabled)\n", upgrade_name);
		//return false;
	}else if ( player_bloodlust[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s                 ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s                 ( %3i / %3i )\n", num, upgrade_name, player_bloodlust[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
	return true;
}

void Upgrade_Bloodlust::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	float bloodlust_percentage = ( baseAdrenalineRegen * 100.0 / BASE_ADRENELINE_REGEN )
					* ( player_bloodlust[ID].cur_level + 1 );
	
	float bloodlust_percentage_onos = bloodlust_percentage / 100.0 * OnosPercentage;
	
	float vampirism = baseVampirism * ( player_bloodlust[ID].cur_level + 1 );
	
	bool req_correct = player_bloodlust[ID].check_Requirements();
	sprintf(menu, dummy_description,
			(int)bloodlust_percentage,
			(int)bloodlust_percentage_onos,
			vampirism,
			req_level + player_bloodlust[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			player_bloodlust[ID].cur_level + 1,
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

void EL_Bloodlust::reset( )
{
	cur_level = 0;
	BloodlustRegen = 0.0;
	nextBloodlust = 0.0;
	lastFrags = 0.0;
	Vampirism = 0.0;
	Vampirism_Fade_Onos = 0.0;
}

bool EL_Bloodlust::check_Requirements( )
{
	return ( player_data[ID].level >= data_bloodlust.req_level + cur_level
		&& player_data[ID].points_available >= data_bloodlust.req_points
		&& cur_level < data_bloodlust.max_level
		&& UTIL_getMask(pEntity, MASK_ADRENALINE) );
}

void EL_Bloodlust::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	player_data[ID].gestation_Emulation = GESTATE_START;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_addPoints(pEntity, data_bloodlust.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_bloodlust.max_level,
							data_bloodlust.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Bloodlust::set_upgrade_values( )
{
	BloodlustRegen = cur_level * data_bloodlust.baseAdrenalineRegen;
	BloodlustRegenOnos = cur_level * data_bloodlust.baseAdrenalineRegen / 100.0 * data_bloodlust.OnosPercentage;
	Vampirism = cur_level * data_bloodlust.baseVampirism;
	Vampirism_Fade_Onos = Vampirism * BL_VAMPIRISM_PERCENTAGE;
}

void EL_Bloodlust::Think( )
{
	if ( cur_level == 0 )
		return;
	
	float energy_to_give = 0.0;
	float base_energy_to_give = 0.0;
	if ( player_data[ID].pClass == CLASS_ONOS )
		base_energy_to_give = BloodlustRegenOnos;
	else
		base_energy_to_give = BloodlustRegen;
	
	// if player killed someone give him some bloodlust boosts (defined by BL_KILL_BOOSTER)
	if ( pEntity->v.frags > lastFrags )
	{
		lastFrags = pEntity->v.frags;
		
		energy_to_give += (base_energy_to_give * BL_KILL_BOOSTER);
	}
	
	if ( gpGlobals->time >= nextBloodlust )
	{
		nextBloodlust = gpGlobals->time + BL_TIME;
		energy_to_give += base_energy_to_give;
	}
	
	if ( pEntity->v.fuser3 + energy_to_give > MAX_ENERGY )
		energy_to_give = MAX_ENERGY - pEntity->v.fuser3;
	
	// not needed anymore due to NS 3.2 charge change
	/*else if ( player_data[ID].pClass == CLASS_ONOS
		&& pEntity->v.fuser3 < ONOS_MIN_ADRENALINE )
	{
		energy_to_give = 0.0;
		pEntity->v.fuser3 = 0.0;	// if onos doing charge and has less than 64 but more than zero:
						// still charging but not enough energy to attack.
						// to prevent onos from not being able to do anything,
						// set energy to zero to stop charge
		
		return;
	}*/
	
	pEntity->v.fuser3 += energy_to_give;
	player_senseofancients[ID].got_BloodLust = energy_to_give;
}

void EL_Bloodlust::drink_my_Blood( )
{
	if ( data_bloodlust.available == false )
		return;
	
	// player must be already dead, so there is nothing to steal
	if ( pEntity->v.health < 1.0 )
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
	
	int attacker_ID = ENTINDEX(entAttackerWeapon->v.owner);
	float health_damage = 0.0;
	switch ( get_private(entAttackerWeapon, MAKE_OFFSET(WEAPON_ID)) )
	{
		// skulk + lerk
		case WEAPON_BITE:
		case WEAPON_BITE2:
		{
			health_damage = player_bloodlust[attacker_ID].Vampirism;
			break;
		}
		// fade + onos
		case WEAPON_SWIPE:
		case WEAPON_CLAWS:
		{
			health_damage = player_bloodlust[attacker_ID].Vampirism_Fade_Onos;
			break;
		}
		default:
		{
			return;
		}
	}
	
	if ( player_data[ID].pClass == CLASS_HEAVY )
		health_damage *= BL_VAMPIRISM_HA_PERCENTAGE;
	
	float health_add = 0.0;
	if ( pEntity->v.health - health_damage < 1.0 )
	{
		// set players points + score before killing victim because victims class will change to CLASS_DEAD
		//player_data[attacker_ID].givePoints(ID);
		//player_data[ID].killPlayer();
		
		// crash when sending message while in message handling (maybe switch this code to Damage Post)
		//player_data[ID].newDeahthMsg(attacker_ID, STRING(entAttackerWeapon->v.classname) /* keep weapon name */);
		
		health_add = pEntity->v.health - 1.0;
		pEntity->v.health = 1.0;
	}else
	{
		pEntity->v.health -= health_damage;
		health_add = health_damage;
	}
	
	edict_t *attackerEntity = entAttackerWeapon->v.owner;
	attackerEntity->v.health += health_add;
	if ( attackerEntity->v.health > player_data[attacker_ID].maxHP )
		attackerEntity->v.health = player_data[attacker_ID].maxHP;
}






