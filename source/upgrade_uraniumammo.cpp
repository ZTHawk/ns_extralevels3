
#include "upgrade_uraniumammo.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_Uraniumammo data_uraniumammo;
EL_Uraniumammo player_uraniumammo[MAX_PLAYERS_PLUS1];

void Upgrade_Uraniumammo::init()
{
	Config_data upgrade_data[] =
	{
		{&(available = URANIUMAMMO), TYPE_BOOL, (char*)UA_config_names[0]},
		{&(req_points = UA_COST), TYPE_INT, (char*)UA_config_names[1]},
		{&(max_level = UA_MAX), TYPE_INT, (char*)UA_config_names[2]},
		{&(req_level = UA_LEVEL), TYPE_INT, (char*)UA_config_names[3]},
		{&(BulletDmgPercentage = UA_BULLETDMG), TYPE_FLOAT, (char*)UA_config_names[4]},
		{&(GrenadeDmgPercentage = UA_GRENDMG), TYPE_FLOAT, (char*)UA_config_names[5]}
	};

	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));

	//req_level = 10;

	strcpy(upgrade_name, "Uranium Ammo");
	strcpy(upgrade_description, "Ammunition contains depleted uranium to enhance damage for all weapons except (Knife, Welder)\n"
					"Damage increased by: Bullets [+%d%%] / Grenades [+%d%%]\n\n"
					"Requires: Weapons 3 , Level %d, %d Point%s\n\n"
					"Next level [%d]\n\n"
					"%s%s\n\n"
					"0. Exit\n\n\n\n\n\n\n");

	team = MARINE;
	max_marine_points += available * max_level * req_points;
}

bool Upgrade_Uraniumammo::add_to_menu(byte ID, int num, int& Keys, char* menu)
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s               (Disabled)\n", upgrade_name);
		//return false;
	} else if ( player_uraniumammo[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s               ( max / %3i )\n", upgrade_name, max_level);
	} else
	{
		Keys |= (1 << (num - 1));
		sprintf(dummy_string, "%d. %s               ( %3i / %3i )\n", num, upgrade_name, player_uraniumammo[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
	return true;
}

void Upgrade_Uraniumammo::show_upgrade_menu(edict_t* pEntity)
{
	int ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char* dummy_description = upgrade_description;

	bool req_correct = player_uraniumammo[ID].check_Requirements();
	sprintf(menu, dummy_description,
			(int)(BulletDmgPercentage * (float)(player_uraniumammo[ID].cur_level + 1)),
			(int)(GrenadeDmgPercentage * (float)(player_uraniumammo[ID].cur_level + 1)),
			req_level + player_uraniumammo[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			player_uraniumammo[ID].cur_level + 1,
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

void EL_Uraniumammo::reset()
{
	cur_level = 0;

	resetGunPointer();
}

void EL_Uraniumammo::resetGunPointer()
{
	entPistolID = 0;
	entPrimaryGunID = 0;
	entPrimaryGunOldID = 0;
	PrimaryGunID = 0;
	entHandGrenadeID = 0;
}

bool EL_Uraniumammo::check_Requirements()
{
	return (player_data[ID].level >= data_uraniumammo.req_level + cur_level
		&& player_data[ID].points_available >= data_uraniumammo.req_points
		&& cur_level < data_uraniumammo.max_level
		&& UTIL_getMask(pEntity, NS_MASK_WEAPONS3));
}

void EL_Uraniumammo::buy_upgrade()
{
	if ( check_Requirements() == false )
		return;

	++cur_level;

	set_upgrade_values();

	UTIL_addPoints(pEntity, data_uraniumammo.req_points);

	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_uraniumammo.max_level,
							data_uraniumammo.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Uraniumammo::set_upgrade_values()
{
	// if first level then we need to collect data first
	if ( cur_level == 1 )
	{
		// prevent extra check, cause player just bought the upgrade
		SpawnTime = 0.0;

		findWeaponData();
	} else
	{
		float dmg_to_set = 0.0;
		edict_t* entWeapon = NULL;
		// Primary Weapon
		if ( !FNullEnt((entWeapon = INDEXENT(entPrimaryGunID))) )
		{
			if ( PrimaryGunID == NS_WEAPON_GRENADE_GUN )
				dmg_to_set = BasicDmg[PrimaryGunID] + (float)cur_level * (BasicDmg[PrimaryGunID] / 100.0 * data_uraniumammo.GrenadeDmgPercentage);
			else
				dmg_to_set = BasicDmg[PrimaryGunID] + (float)cur_level * (BasicDmg[PrimaryGunID] / 100.0 * data_uraniumammo.BulletDmgPercentage);

			UTIL_setWeaponDamage(entWeapon, dmg_to_set);
		}

		// Pistol
		if ( !FNullEnt((entWeapon = INDEXENT(entPistolID))) )
		{
			dmg_to_set = BasicDmg[NS_WEAPON_PISTOL] + (float)cur_level * (BasicDmg[NS_WEAPON_PISTOL] / 100.0 * data_uraniumammo.BulletDmgPercentage);
			UTIL_setWeaponDamage(entWeapon, dmg_to_set);
		}

		// HandGrenade
		if ( !FNullEnt((entWeapon = INDEXENT(entHandGrenadeID))) )
		{
			dmg_to_set = BasicDmg[NS_WEAPON_GRENADE] + (float)cur_level * (BasicDmg[NS_WEAPON_GRENADE] / 100.0 * data_uraniumammo.GrenadeDmgPercentage);
			UTIL_setWeaponDamage(entWeapon, dmg_to_set);
		}
	}
}

void EL_Uraniumammo::respawned()
{
	SpawnTime = gpGlobals->time + 0.1;
}

void EL_Uraniumammo::Think()
{
	if ( cur_level == 0 )
		return;

	// VERY IMPORTANT CRASH FIX
	// players always spawn with LMG even if purchased HMG , Shotgun or GL
	// if player has purchased an advanced Weapon this weapon is given 0.1 sec after spawn by NS itself
	if ( SpawnTime > 0.0 )
	{
		if ( gpGlobals->time > SpawnTime )
		{
			setWeaponData_Dmg();
			SpawnTime = 0.0;
		}

		return;
	}

	if ( UTIL_isAlive(pEntity) == false )
		return;

	bool get_new_data = false;
	// check Pistol ID
	if ( UTIL_hasWeapon(pEntity, NS_WEAPON_PISTOL) == false )
	{
		if ( entPistolID != 0 )
			entPistolID = 0;
	} else if ( entPistolID == 0
		|| FNullEnt(INDEXENT(entPistolID)) )
		get_new_data = true;

	// check Grenade ID
	if ( UTIL_hasWeapon(pEntity, NS_WEAPON_GRENADE) == false )
	{
		if ( entHandGrenadeID != 0 )
			entHandGrenadeID = 0;
	} else if ( entHandGrenadeID == 0
		|| FNullEnt(INDEXENT(entHandGrenadeID)) )
		get_new_data = true;

	// check Primary Weapon ID
	if ( UTIL_hasWeapon(pEntity, NS_WEAPON_LMG) == false
		&& UTIL_hasWeapon(pEntity, NS_WEAPON_HMG) == false
		&& UTIL_hasWeapon(pEntity, NS_WEAPON_SHOTGUN) == false
		&& UTIL_hasWeapon(pEntity, NS_WEAPON_GRENADE_GUN) == false )
	{
		if ( entPrimaryGunID != 0 )
			entPrimaryGunID = 0;
	} else if ( entPrimaryGunID == 0
		|| FNullEnt(INDEXENT(entPrimaryGunID)) )
		get_new_data = true;

	if ( get_new_data == false )
		return;

	// reset all Data and collect it again
	resetGunPointer();
	findWeaponData();
}

void EL_Uraniumammo::setWeaponData_Dmg(byte mode)
{
	if ( cur_level == 0 )
		return;

	if ( mode == UA_BOUGHT_NEW_WEAPON )
	{
		entPrimaryGunOldID = entPrimaryGunID;
	} else
	{
		resetGunPointer();	// clear old weapon IDs
	}

	findWeaponData(mode);
}

void EL_Uraniumammo::findWeaponData(byte mode)
{
	float damage_to_set = 0.0;
	byte foundID = 0;
	byte foundNum = 0;
	byte WeaponID = 0;
	int weaponsAmount = UTIL_hasWeapon(pEntity, NS_WEAPON_GRENADE) ? 3 : 2;
	edict_t* Ent = NULL;
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
			case NS_WEAPON_SHOTGUN:
			case NS_WEAPON_HMG:
			case NS_WEAPON_LMG:
			case NS_WEAPON_PISTOL:
			{
				// mode = 1 means player purchased new weapon
				// in this case do not give extra ammo to pistol
				if ( mode == UA_BOUGHT_NEW_WEAPON
					&& WeaponID == NS_WEAPON_PISTOL )
					continue;

				damage_to_set = BasicDmg[WeaponID] + (float)cur_level * (BasicDmg[WeaponID] / 100.0 * data_uraniumammo.BulletDmgPercentage);
				foundID = WeaponID;
				break;
			}
			case NS_WEAPON_GRENADE_GUN:
			case NS_WEAPON_GRENADE:
			{
				damage_to_set = BasicDmg[WeaponID] + (float)cur_level * (BasicDmg[WeaponID] / 100.0 * data_uraniumammo.GrenadeDmgPercentage);
				foundID = WeaponID;
				break;
			}
		}

		if ( !foundID )
			continue;

		if ( foundID == NS_WEAPON_PISTOL )
		{
			entPistolID = entID;
		} else if ( foundID == NS_WEAPON_GRENADE )
		{
			entHandGrenadeID = entID;
		} else
		{
			// when new weapon is purchase player has 3 weapons
			// pistol + old primary + new primary
			// make sure we do not save old primary to upgrade data
			if ( entID == entPrimaryGunOldID )
			{
				foundID = 0;		// important to clear this or otherr entities would be found
				continue;
			}

			entPrimaryGunID = entID;
			PrimaryGunID = foundID;
		}

		UTIL_setWeaponDamage(Ent, damage_to_set);

		foundID = 0;
		++foundNum;

		// player can only have pistol + primary weap ( = 2 ), + grenade ( = 3 )
		// so stop further search
		if ( foundNum == weaponsAmount )
		{
			break;
		}
	}
}






