#include "upgrade_advancedammopack.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_AdvancedAmmopack data_advancedammopack;
EL_AdvancedAmmopack player_advancedammopack[MAX_PLAYERS_PLUS1];

void Upgrade_AdvancedAmmopack::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = ADVAMMOPACK), TYPE_BOOL, (char *)AA_config_names[0]},
		{&(req_points = AA_COST), TYPE_INT, (char *)AA_config_names[1]},
		{&(max_level = AA_MAX), TYPE_INT, (char *)AA_config_names[2]},
		{&(req_level = AA_LEVEL), TYPE_INT, (char *)AA_config_names[3]},
		{&(Ammo_per_WeaponID[NS_WEAPON_PISTOL] = AA_PISTOL), TYPE_FLOAT, (char *)AA_config_names[4]},
		{&(Ammo_per_WeaponID[NS_WEAPON_LMG] = AA_LMG), TYPE_FLOAT, (char *)AA_config_names[5]},
		{&(Ammo_per_WeaponID[NS_WEAPON_SHOTGUN] = AA_SHOTGUN), TYPE_FLOAT, (char *)AA_config_names[6]},
		{&(Ammo_per_WeaponID[NS_WEAPON_HMG] = AA_HMG), TYPE_FLOAT, (char *)AA_config_names[7]},
		{&(Ammo_per_WeaponID[NS_WEAPON_GRENADE_GUN] = AA_GL), TYPE_FLOAT, (char *)AA_config_names[8]}
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	strcpy(upgrade_name, "Advanced Ammopack");
	strcpy(upgrade_description, "Your Ammopacks will be enhanced to provide more max Ammunition\n"
					"Pistol [%d]    / LMG [%d]    / Shotgun [%d]\n"
					"HMG [%d]    / GL [%d]\n\n"
					"Requires: Resupply , Armor 1 , Weapons 1 , Level %d, %d Point%s\n\n"
					"Next level [%d]\n\n"
					"%s%s\n\n"
					"0. Exit\n\n\n\n\n\n\n\n");
	
	team = MARINE;
	max_marine_points += available * max_level * req_points;
}

bool Upgrade_AdvancedAmmopack::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s    (Disabled)\n", upgrade_name);
		//return false;
	}else if ( player_advancedammopack[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s    ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s    ( %3i / %3i )\n", num, upgrade_name, player_advancedammopack[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
	return true;
}

void Upgrade_AdvancedAmmopack::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	// max new Ammo = ( ( level + 1 ) * amount to increase per level ) + default max Ammo
	int new_pistol_add = (int)( (float)( player_advancedammopack[ID].cur_level + 1 ) * Ammo_per_WeaponID[NS_WEAPON_PISTOL]
				+ (float)BasicAmmo[NS_WEAPON_PISTOL] );
	int new_lmg_add = (int)( (float)( player_advancedammopack[ID].cur_level + 1 ) * Ammo_per_WeaponID[NS_WEAPON_LMG]
				+ (float)BasicAmmo[NS_WEAPON_LMG] );
	int new_shotgun_add = (int)( (float)( player_advancedammopack[ID].cur_level + 1 ) * Ammo_per_WeaponID[NS_WEAPON_SHOTGUN]
				+ (float)BasicAmmo[NS_WEAPON_SHOTGUN] );
	int new_hmg_add = (int)( (float)( player_advancedammopack[ID].cur_level + 1 ) * Ammo_per_WeaponID[NS_WEAPON_HMG]
				+ (float)BasicAmmo[NS_WEAPON_HMG] );
	int new_gl_add = (int)( (float)( player_advancedammopack[ID].cur_level + 1 ) * Ammo_per_WeaponID[NS_WEAPON_GRENADE_GUN]
				+ (float)BasicAmmo[NS_WEAPON_GRENADE_GUN] );
	
	bool req_correct = player_advancedammopack[ID].check_Requirements();
	sprintf(menu, dummy_description,
			new_pistol_add,
			new_lmg_add,
			new_shotgun_add,
			new_hmg_add,
			new_gl_add,
			req_level + player_advancedammopack[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			player_advancedammopack[ID].cur_level + 1,
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

void EL_AdvancedAmmopack::reset( )
{
	cur_level = 0;
	
	// reset for each weapon ( 32 weapons )
	for ( byte i = 0; i < 33; ++i )
		freeAmmo_at_lvl[i] = 0;
	
	endReloadTime = 0.0;		// time when reload will be done
	GL_ReloadTime_reducer = 0.0;	// amount of time the reload time will be reduced
	SpawnTime = 0.0;
	
	resetGunPointer();
	resetExtra();
}

void EL_AdvancedAmmopack::resetGunPointer( )
{
	entPistolID = 0;
	entPrimaryGunID = 0;
	entPrimaryGunOldID = 0;
	PrimaryGunID = 0;
}

void EL_AdvancedAmmopack::resetExtra( )
{
	if ( curReloadingWeapon == NS_WEAPON_SHOTGUN )
	{
		if ( !FNullEnt(INDEXENT(entPrimaryGunID)) )
		{
			addWeaponClip(INDEXENT(entPrimaryGunID), Shotgun_bulltes_stolen - 1 + Shotgun_bulltes_xtra_stolen);
		}
		
		Shotgun_bulltes_stolen = 0;
		Shotgun_bulltes_xtra_stolen = 0;
	}
	/*else if ( !FNullEnt(INDEXENT(entPrimaryGunID)) )
	{
		addWeaponClip(INDEXENT(entPrimaryGunID), bullets_stolen);
		addWeaponReserve(curReloadingWeapon, -bullets_stolen);
	}*/
	
	Shotgun_bulltes_to_steal = 0;
	
	bullets_stolen = 0;
	
	curReloadingWeapon = 0;
	ReloadingStarted = false;
	Ammo_before_reload = 0;
	Reserve_before_reload = 0;
	curWeaponMaxAmmoBasic = 0;
}

void EL_AdvancedAmmopack::reset_basic( )
{
	if ( cur_level == 0 )
		return;
	
	if ( player_data[ID].curWeapon == NS_WEAPON_GRENADE_GUN
		&& endReloadTime > gpGlobals->time
		&& UTIL_getWeaponClip(INDEXENT(entPrimaryGunID)) > 0 )
	{
		// round to ceil
		int bullets_got_to_much = (int)ceil(( endReloadTime - gpGlobals->time ) / RELOAD_TIME_GL);
		int correct_gl_ammo = (BasicAmmo[NS_WEAPON_GRENADE_GUN] + bonusAmmo[NS_WEAPON_GRENADE_GUN]) - bullets_got_to_much;
		int ammo_corrector = UTIL_getWeaponClip(INDEXENT(entPrimaryGunID)) - correct_gl_ammo;
		if ( ammo_corrector > 0 )
		{
			addWeaponClip(INDEXENT(entPrimaryGunID), -ammo_corrector);
			addWeaponReserve(NS_WEAPON_GRENADE_GUN, ammo_corrector);
		}
	}
	
	resetExtra();
	Reserve_ammo_corrector = 0;	// saving real reserve amount after reload is done ( used for cosmetic fix )
	endReloadTime = 0.0;		// time when reload will be done
}

void EL_AdvancedAmmopack::fixAnimation( )
{
	if ( cur_level == 0 )
		return;
		
	if ( player_data[ID].curWeapon == NS_WEAPON_GRENADE_GUN
		&& endReloadTime < gpGlobals->time )
	{
		pEntity->v.weaponanim = ANIMATION_SWITCHED_TO_GL;
	}
}

bool EL_AdvancedAmmopack::check_Requirements( )
{
	return ( player_data[ID].level >= data_advancedammopack.req_level + cur_level
		&& player_data[ID].points_available >= data_advancedammopack.req_points
		&& cur_level < data_advancedammopack.max_level
		&& player_data[ID].got_resupply == true
		&& UTIL_getMask(pEntity, NS_MASK_ARMOR1)
		&& UTIL_getMask(pEntity, NS_MASK_WEAPONS1) );
}

void EL_AdvancedAmmopack::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_addPoints(pEntity, data_advancedammopack.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_advancedammopack.max_level,
							data_advancedammopack.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_AdvancedAmmopack::set_upgrade_values( )
{
	// set for each weapon ( 32 weapons )
	for ( byte i = 0; i < 33; ++i )
		bonusAmmo[i] = (int)(data_advancedammopack.Ammo_per_WeaponID[i] * (float)cur_level);
	
	// if first level then we need to collect data first
	if ( cur_level == 1 )
	{
		// prevent extra check, cause player just bought the upgrade
		SpawnTime = 0.0;
		
		findWeaponData();
	}else if ( !ReloadingStarted )
	{
		// Primary Weapon
		float ammo_per_Level = 0.0;
		edict_t *entWeapon = NULL;
		if ( !FNullEnt((entWeapon = INDEXENT(entPrimaryGunID))) )
		{
			ammo_per_Level = data_advancedammopack.Ammo_per_WeaponID[PrimaryGunID];
			
			if ( ammo_per_Level <= 1.0
				&& ( bonusAmmo[PrimaryGunID] - ammo_per_Level * freeAmmo_at_lvl[PrimaryGunID] ) >= 1.0 )	// new bonus ammo - last bonus > 1, then give that extra ammo
			{
				freeAmmo_at_lvl[PrimaryGunID] = cur_level;
				ammo_per_Level = 1.0;
			}
			
			addWeaponClip(entWeapon, (int)ammo_per_Level);
			addWeaponReserve(PrimaryGunID, (int)(ammo_per_Level * 2.0));
		}
		
		// Pistol
		if ( !FNullEnt((entWeapon = INDEXENT(entPistolID))) )
		{
			ammo_per_Level = data_advancedammopack.Ammo_per_WeaponID[NS_WEAPON_PISTOL];
			
			if ( ammo_per_Level <= 1.0
				&& ( bonusAmmo[NS_WEAPON_PISTOL] - ammo_per_Level * freeAmmo_at_lvl[NS_WEAPON_PISTOL] ) >= 1.0 )	// new bonus ammo - last bonus > 1, then give that extra ammo
			{
				freeAmmo_at_lvl[NS_WEAPON_PISTOL] = cur_level;
				ammo_per_Level = 1.0;
			}
			
			addWeaponClip(entWeapon, (int)ammo_per_Level);
			addWeaponReserve(NS_WEAPON_PISTOL, (int)(ammo_per_Level * 2.0));
		}
	}
}

void EL_AdvancedAmmopack::respawned( )
{
	SpawnTime = gpGlobals->time + 0.1;
}

void EL_AdvancedAmmopack::setWeaponData_Ammo( byte mode )
{
	if ( cur_level == 0 )
		return;
	
	if ( mode == AA_BOUGHT_NEW_WEAPON )
	{
		entPrimaryGunOldID = entPrimaryGunID;
	}else
	{
		resetGunPointer();	// clear old weapon IDs
	}
	
	resetExtra();		// clear old data to stop old reloading code
	findWeaponData(mode);
}



void EL_AdvancedAmmopack::Think( )
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
			setWeaponData_Ammo();
			SpawnTime = 0.0;
		}else
			return;
	}
	
	byte WeaponID = player_data[ID].curWeapon;
	switch ( WeaponID )
	{
		case NS_WEAPON_PISTOL:
		case NS_WEAPON_LMG:
		case NS_WEAPON_SHOTGUN:
		case NS_WEAPON_HMG:
		case NS_WEAPON_GRENADE_GUN:
		{
			break;
		}
		default:
		{
			// wrong weapon, leave AdvAmmo
			return;
		}
	}
	
	int entWeaponID = ( WeaponID == NS_WEAPON_PISTOL ) ? entPistolID : entPrimaryGunID;
	if ( FNullEnt(INDEXENT(entWeaponID)) )
	{
		// player has a weapon that is AdvAmmopack compatible but its ID is invalid
		// so get new data
		if ( entWeaponID == entPrimaryGunID )
			// needs to be cleared cause if ID is invalid old ID is invalid too
			entPrimaryGunOldID = 0;
		
		findWeaponData();
		return;
	}
	edict_t *entWeapon = INDEXENT(entWeaponID);
	
	Current_Ammo = UTIL_getWeaponClip(entWeapon);
	Reserve_Ammo = UTIL_getWeaponReserve(pEntity, WeaponID);
	int Ammo_to_add = bonusAmmo[WeaponID];
	
	checkReloadCorrectors(WeaponID, &Ammo_to_add);
	
	// check if player tries to reload
	checkReloadStarting(WeaponID, entWeapon);
	
	// check if cur ammo = default value
	// if so trick NS and remove one bullet and add it to reserve
	checkReloadingFullAmmo(WeaponID, entWeapon);
	
	if ( ReloadingStarted == false )
		return;
	
	bool isWeaponReloading = ( get_private(entWeapon, MAKE_OFFSET(WEAPON_RELOADING)) != 0 );
	ReloadInitiate(WeaponID, Ammo_to_add, isWeaponReloading);
	
	// check if player's weapon is shotgun
	if ( WeaponID == NS_WEAPON_SHOTGUN )
		ReloadShotgun(entWeapon);
	else if ( curReloadingWeapon )
		ReloadOtherWeapons(WeaponID, entWeapon, Ammo_to_add, isWeaponReloading);
	
	// update HUD
	//if ( gpGlobals->time - player_data[ID].lastHUD_display > DISPLAY_TIME
	//	&& WeaponID == NS_WEAPON_GRENADE_GUN )
	//	player_data[ID].lastHUD_display = gpGlobals->time - 2.0;
}

void EL_AdvancedAmmopack::checkReloadCorrectors( byte WeaponID , int *Ammo_to_add )
{
	// make sure only primary weapon gets this check ( it is not probable that pistol will have over 250 ammo )
	if ( WeaponID != PrimaryGunID )
		return;
	
	// fix so that clip will always be displayed as max 250 ammo
	if ( Reserve_ammo_corrector )
	{
		// we have set Reserve_Ammo_new to smaller 0 (zero) so make it positiv and do action in next server frame
		if ( Reserve_ammo_corrector > 0 )
		{
			UTIL_setWeaponReserve(pEntity, WeaponID, Reserve_ammo_corrector);
			Reserve_ammo_corrector = 0;
		}else
			Reserve_ammo_corrector *= -1;
	}
	
	// check if we have enough reserve as requested
	if ( Reserve_Ammo < *Ammo_to_add )
		*Ammo_to_add = Reserve_Ammo;
}

void EL_AdvancedAmmopack::checkReloadStarting( byte WeaponID , edict_t *entWeapon )
{
	if ( ReloadingStarted == true )
		return;
	
	if ( !( pEntity->v.button & IN_RELOAD ) )
	{
		ReloadingStarted = ( Current_Ammo == 0 && Reserve_Ammo > 0 );
		return;
	}
	
	// if full ammo + full Advammopack OR attacking OR still reloading,
	// do not allow to reload
	if ( Current_Ammo >= BasicAmmo[WeaponID] + bonusAmmo[WeaponID]
		|| gpGlobals->time < endReloadTime
		|| pEntity->v.button & IN_ATTACK )
	{	
		pEntity->v.button = pEntity->v.button & ~IN_RELOAD;
		
		return;
	}else if ( WeaponID == NS_WEAPON_GRENADE_GUN
		&& Current_Ammo >= BasicAmmo[WeaponID] )
	{
		if ( Reserve_Ammo <= 0 )
			return;
		
		// correct GL reloading
		// NS will only make animations if ammo is < default max
		// so check how many grens we need to remove from clip and add to reserve
		
		GL_ReloadTime_reducer = ( ( ( Current_Ammo - BasicAmmo[NS_WEAPON_GRENADE_GUN] ) + 1 ) * RELOAD_TIME_GL );
		Reserve_Ammo += ( Current_Ammo - BasicAmmo[NS_WEAPON_GRENADE_GUN] ) + 1;
		Current_Ammo -= ( Current_Ammo - BasicAmmo[NS_WEAPON_GRENADE_GUN] ) + 1;
		UTIL_setWeaponReserve(pEntity, WeaponID, Reserve_Ammo);
		UTIL_setWeaponClip(entWeapon, Current_Ammo);
	}
	
	ReloadingStarted = true;
}

void EL_AdvancedAmmopack::checkReloadingFullAmmo( byte WeaponID , edict_t *entWeapon )
{
	if ( ReloadingStarted == false )
		return;
	
	if ( Reserve_Ammo <= 0 )
		return;
	
	if ( curReloadingWeapon != 0 )
		return;
	
	if ( WeaponID == NS_WEAPON_SHOTGUN
		&& ( Current_Ammo < BasicAmmo[WeaponID]
			|| Shotgun_bulltes_stolen != 0 ) )	// nothing  stolen yet, to prevent loops
		return;
	
	if ( WeaponID != NS_WEAPON_SHOTGUN
		&& Current_Ammo != BasicAmmo[WeaponID] )
		return;
	
	if ( WeaponID == NS_WEAPON_SHOTGUN )
	{
		// steal difference between basic and current ammo + 1
		Shotgun_bulltes_xtra_stolen = Current_Ammo - BasicAmmo[WeaponID] + 1;
		addWeaponClip(entWeapon, -Shotgun_bulltes_xtra_stolen);
		Shotgun_bulltes_stolen = 1;
	}else
	{
		--Current_Ammo;
		addWeaponClip(entWeapon, -1);
		++Reserve_Ammo;
		addWeaponReserve(WeaponID, 1);
		++bullets_stolen;
	}
}

void EL_AdvancedAmmopack::ReloadInitiate( byte WeaponID , int Ammo_to_add , bool isWeaponReloading )
{
	byte Player_WeaponAnimation = pEntity->v.weaponanim;
	if ( isWeaponReloading == false
		&& ( WeaponID != NS_WEAPON_SHOTGUN		// shotgun does not set reload value
			|| Player_WeaponAnimation != 2 ) )
		return;
	
	if ( curReloadingWeapon != 0 )
		return;
	
	curReloadingWeapon = WeaponID;
	Ammo_before_reload = Current_Ammo;
	Reserve_before_reload = Reserve_Ammo;
	
	endReloadTime = gpGlobals->time;
	switch ( WeaponID )
	{
		case NS_WEAPON_PISTOL:
		{
			endReloadTime += RELOAD_TIME_PISTOL;
			break;
		}
		case NS_WEAPON_LMG:
		{
			endReloadTime += RELOAD_TIME_LMG;
			break;
		}
		case NS_WEAPON_SHOTGUN:
		{
			int additional_time = 0;
			// shotgun reloads each bullet seperatly
			// so check how many bullets we can remove which will be readded later
			if ( Shotgun_bulltes_stolen <= 0 )
			{
				Shotgun_bulltes_to_steal = bonusAmmo[WeaponID];
				Shotgun_bulltes_stolen = 1;			// set this so code does need any changes ( in this case it does not mean anything )
				additional_time = BasicAmmo[WeaponID] - Current_Ammo;	// reload bonus ammo + ( base - current )
				
			}else
			{
				Shotgun_bulltes_to_steal = BasicAmmo[WeaponID] + bonusAmmo[WeaponID] - Current_Ammo - Shotgun_bulltes_stolen - Shotgun_bulltes_xtra_stolen;
				Shotgun_bulltes_stolen = 1;
			}
			
			endReloadTime += ( RELOAD_TIME_SHOTGUN_INIT
					+ ( Shotgun_bulltes_to_steal + Shotgun_bulltes_stolen + additional_time ) * RELOAD_TIME_SHOTGUN
					+ RELOAD_TIME_SHOTGUN_END );
			
			break;
		}
		case NS_WEAPON_HMG:
		{
			endReloadTime += RELOAD_TIME_HMG;
			break;
		}
		case NS_WEAPON_GRENADE_GUN:
		{
			endReloadTime += ( RELOAD_TIME_GL_INIT
					+ ( ( Player_WeaponAnimation - 3 + Ammo_to_add ) * RELOAD_TIME_GL )	// 1 grenade => Player_WeaponAnimation = 4
					- GL_ReloadTime_reducer );
			break;
		}
	}
}

void EL_AdvancedAmmopack::ReloadShotgun( edict_t *entWeapon )
{
	// check if player started attacking with shotgun
	if ( ( pEntity->v.button & IN_ATTACK )
		&& Current_Ammo > 0 )
	{
		resetExtra();
		
		endReloadTime = gpGlobals->time;
		
		pEntity->v.weaponanim = 0;
	// we reloaded shotgun to max OR reserve ammo is empty
	}else if ( Current_Ammo == BasicAmmo[NS_WEAPON_SHOTGUN] )
	{
		// this forces to play the "Reaload Done" animation
		// cause Reload animation ( = 2 ) is a loop animation
		pEntity->v.weaponanim = ANIMATION_RELOAD_DONE_SHOTGUN;
		
		// reload is done so far, reset all unneeded vars
		resetExtra();
		
	// shotgun is still reloading
	}else if ( curReloadingWeapon )
	{
		// check if we reached MAX DEFAULT AMMO - 1 and if we still have bullets to steal
		// steal bullets if needed
		if ( Current_Ammo == BasicAmmo[NS_WEAPON_SHOTGUN] - 1
			&& Shotgun_bulltes_to_steal >= 1
			&& Reserve_Ammo > 0 )
		{
			int temp_ammo;
			if ( Current_Ammo > Shotgun_bulltes_to_steal )
				temp_ammo = Shotgun_bulltes_to_steal;
			else
				temp_ammo = Current_Ammo - 1;
			
			// remove bullets from clip and add it to reserve
			Shotgun_bulltes_stolen += temp_ammo;
			Shotgun_bulltes_to_steal -= temp_ammo;
			addWeaponClip(entWeapon, -temp_ammo);
		}else if ( Shotgun_bulltes_to_steal >= 1
			&& Reserve_Ammo <= 0 )
		{
			endReloadTime -= Shotgun_bulltes_to_steal * RELOAD_TIME_SHOTGUN;
			// this forces to play the "Reaload Done" animation
			// cause Reload animation ( = 2 ) is a loop animation
			pEntity->v.weaponanim = ANIMATION_RELOAD_DONE_SHOTGUN;
			
			// reload is done so far, reset all unneeded vars
			resetExtra();
		}
	}
}

void EL_AdvancedAmmopack::ReloadOtherWeapons( byte WeaponID , edict_t *entWeapon , int Ammo_to_add , bool isWeaponReloading )
{
	// when player has more ammo than normal and reloads, it will be set back to normal and rest is added to reserve
	// so trick our method to think we got ammo and NOT lost some (by switching current reserve and old reserve)
	if ( Current_Ammo < Ammo_before_reload )
	{
		bullets_stolen += ( Ammo_before_reload - Current_Ammo );
		int temp_ammo = Reserve_before_reload;
		Reserve_before_reload = Reserve_Ammo;
		Reserve_Ammo = temp_ammo;
	}
	
	// if ammo is = basic max ammo ( +1 is a fix) AND weapon is not reloading
	// then NS has reloaded that weapon
	if ( ( Current_Ammo != BasicAmmo[WeaponID]
			&& Current_Ammo + 1 != BasicAmmo[WeaponID] )
		|| isWeaponReloading == true )
		return;
	
	// check if player has ammo left in reserve and add Advanced Ammopack if so
	if ( Reserve_Ammo > 0)
	{
		// check if trying to remove more ammo than player has
		if ( Reserve_Ammo < Ammo_to_add )
			Ammo_to_add = Reserve_Ammo;
		
		addWeaponClip(entWeapon, Ammo_to_add);
		addWeaponReserve(WeaponID, -Ammo_to_add);
		
		// GL is reloading grenade by grenade
		// and animation is not syncronized with actual reload
		// so block fire by setting time when player is able to shoot again
		if ( WeaponID == NS_WEAPON_GRENADE_GUN )
		{
			set_private_f(pEntity, MAKE_OFFSET(WEAPON_RELOAD_TIME),
					get_private_f(pEntity, MAKE_OFFSET(WEAPON_RELOAD_TIME))
						+ ( (float)Ammo_to_add * RELOAD_TIME_GL ) - GL_ReloadTime_reducer);
			
			GL_ReloadTime_reducer = 0.0;
		}
	}else
	{
		// GL reloaded and no reserve ammo left
		endReloadTime = 0.0;
	}
	
	// reload is done so far, reset all unneeded vars
	resetExtra();
}

void EL_AdvancedAmmopack::addWeaponClip( edict_t *entWeapon , int ammo_to_add )
{
	UTIL_addWeaponClip(entWeapon, ammo_to_add);
}

void EL_AdvancedAmmopack::addWeaponReserve( byte WeaponID , int ammo_to_add )
{
	int reserve_ammo = UTIL_getWeaponReserve(pEntity, WeaponID) + ammo_to_add;
	if ( reserve_ammo > MAX_VISIBLE_AMMO )
	{
		// need to set ammo in second preThink, so make in next Think positiv (saves a check variable)
		Reserve_ammo_corrector += -reserve_ammo;
		
		UTIL_setWeaponReserve(pEntity, WeaponID, MAX_VISIBLE_AMMO);
	}else
		UTIL_setWeaponReserve(pEntity, WeaponID, reserve_ammo);
}

void EL_AdvancedAmmopack::findWeaponData( byte mode )
{
	byte foundID = 0;
	byte foundNum = 0;
	byte WeaponID = 0;
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
			case NS_WEAPON_SHOTGUN:
			case NS_WEAPON_HMG:
			case NS_WEAPON_GRENADE_GUN:
			case NS_WEAPON_LMG:
			case NS_WEAPON_PISTOL:
			{
				// mode = 1 means player purchased new weapon
				// in this case do not give extra ammo to pistol
				if ( mode == AA_BOUGHT_NEW_WEAPON
					&& WeaponID == NS_WEAPON_PISTOL )
					continue;
				
				foundID = WeaponID;
				break;
			}
		}
		
		if ( !foundID )
			continue;
		
		if ( foundID == NS_WEAPON_PISTOL )
		{
			entPistolID = entID;
		}else
		{
			// when new weapon is purchase player has 3 weapons
			// pistol + old primary + new primary
			// make sure we do not save old primary to upgrade data
			if ( entID == entPrimaryGunOldID )
			{
				foundID = 0;		// important to clear this or other entities would be found
				continue;
			}
			
			entPrimaryGunID = entID;
			PrimaryGunID = foundID;
		}
		
		addWeaponClip(Ent, bonusAmmo[WeaponID]);
		addWeaponReserve(foundID, bonusAmmo[WeaponID] * 2);
		
		foundID = 0;
		++foundNum;
		
		// player can only have pistol + primary weap ( = 2 )
		// so stop further search
		if ( foundNum == 2 )
			break;
	}
}

void EL_AdvancedAmmopack::setAmmoPickupMsg( byte AmmoType )
{
	byte weapID = player_data[ID].curWeapon;
	
	MESSAGE_BEGIN(MSG_ONE, AmmoPickup_ID, NULL, pEntity);
	WRITE_BYTE(AmmoType);
	WRITE_BYTE(BasicAmmo[weapID] + bonusAmmo[weapID]);
	MESSAGE_END();
	
	addWeaponReserve(weapID, bonusAmmo[weapID]);
}

bool EL_AdvancedAmmopack::setHUDText( byte vID , bool is_marine , hudtextparms_t &hud_params , char *CoreT_GL_reload_Shift_text)
{
	if ( cur_level == 0 )
		return false;
	
	if ( endReloadTime <= gpGlobals->time )
		return false;
	
	if ( player_data[vID].curWeapon == NS_WEAPON_GRENADE_GUN )
	{
		sprintf(CoreT_GL_reload_Shift_text, "You are reloading your GL with Advanced Ammo\nReload done in %3.1f second%s\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", endReloadTime - gpGlobals->time, ( endReloadTime - gpGlobals->time >= 2.0 ) ? "s" : "");	// 21 * \n
		UTIL_setHUD(hud_params
			, is_marine ? MARINE_HUD_COLOR_R : ALIEN_HUD_COLOR_R
			, is_marine ? MARINE_HUD_COLOR_G : ALIEN_HUD_COLOR_G
			, is_marine ? MARINE_HUD_COLOR_B : ALIEN_HUD_COLOR_B
			, -1.0, UTIL_isAlive(INDEXENT(vID)) ? 0.46 : 0.39, 0, 0.0, 3600.0, 0.0, 0.0, HUD_CHANNEL);
		return true;
	}else if ( player_data[vID].curWeapon == NS_WEAPON_SHOTGUN )
	{
		int current_ammo = UTIL_getWeaponClip(INDEXENT(entPrimaryGunID)) + Shotgun_bulltes_stolen - 1 + Shotgun_bulltes_xtra_stolen;
		sprintf(CoreT_GL_reload_Shift_text, "You are reloading your Shotgun with Advanced Ammo\n%d of %d Bullets\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", current_ammo, BasicAmmo[NS_WEAPON_SHOTGUN] + bonusAmmo[NS_WEAPON_SHOTGUN]);	// 23 * \n
		UTIL_setHUD(hud_params
			, is_marine ? MARINE_HUD_COLOR_R : ALIEN_HUD_COLOR_R
			, is_marine ? MARINE_HUD_COLOR_G : ALIEN_HUD_COLOR_G
			, is_marine ? MARINE_HUD_COLOR_B : ALIEN_HUD_COLOR_B
			, -1.0, UTIL_isAlive(INDEXENT(vID)) ? 0.421 : 0.351, 0, 0.0, 3600.0, 0.0, 0.0, HUD_CHANNEL);
		return true;
	}
	
	return false;
}


