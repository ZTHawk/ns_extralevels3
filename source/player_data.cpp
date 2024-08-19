#include "player_data.h"
#include "upgrade_cybernetics.h"
#include "upgrade_thickenedskin.h"
#include "upgrade_reinforcedarmor.h"
#include "upgrade_etherealshift.h"
#include "upgrade_nanoarmor.h"
#include "upgrade_bloodlust.h"
#include "upgrade_advancedammopack.h"
#include "upgrade_hunger.h"
#include "upgrade_staticfield.h"
#include "upgrade_acidicvengeance.h"
#include "upgrade_uraniumammo.h"
#include "upgrade_senseofancients.h"
#include "upgrade_blindingsurge.h"
#include "upgrade_lifesheath.h"
#include "upgrade_combatevolution.h"
#include "utilfunctions.h"
#include "ns_const.h"
#include "events_const.h"

int max_marine_points = BASE_MAX_MARINE_POINTS;
int max_alien_points = BASE_MAX_ALIEN_POINTS;

EL_Player player_data[MAX_PLAYERS_PLUS1];
byte WP_ID = 0;
bool WP_notified = false;

void EL_Player::reset(bool in_game)
{
	team = 0;
	level = 1;
	extralevels = 0;
	points_available = 0;
	lastxp = -1.0;
	lastpLevel = 1;
	xp_to_next_lvl = 100;
	base_level_xp = 0;

	got_xp = false;
	is_killer_of = 0;
	got_resupply = false;
	got_scan = false;

	scoreinfo_data[SCORE_INFO_ID] = 0;
	scoreinfo_data[SCORE_INFO_SCORE] = 0;
	scoreinfo_data[SCORE_INFO_KILLS] = 0;
	scoreinfo_data[SCORE_INFO_DEATHS] = 0;
	scoreinfo_data[SCORE_INFO_LEVEL] = 0;
	scoreinfo_data[SCORE_INFO_CLASS] = 0;
	scoreinfo_data[SCORE_INFO_ICON] = 0;
	scoreinfo_data[SCORE_INFO_TEAM] = 0;
	scoreinfo_data[SCORE_INFO_HEALTH] = 0;
	scoreinfo_string[0] = 0;

	upgrade_choice = 0;
	message_displaying = false;

	statusvalue_pointing_at = 0;
	statusvalue_correct_level_of = 0;

	alien_gestate_points = 0;
	curWeapon = 0;

	lastHUD_display = 0.0;

	if ( in_game == false )
	{
		infoshown = 0;
		authorshown = 0;
		banned = false;
		isBot = false;
	}

	armor_bonus = 0.0;

	resetGestate();

	// addionional data
	in_main_menu = false;
	in_upgrade_menu = -1;
	in_help_menu = false;

	pTeam = 0;
	pClass = 1;	// default class is 1 ( in ready room )
	pDeath = 0;

	if ( in_game == false )
	{
		ingame = false;
		join_time_10 = 0.0;
		name[0] = 0;	// reset
		SteamID[0] = 0;
	}
}

void EL_Player::resetData(bool ingame)
{
	reset(ingame);

	for ( int upgrade_ID = UP_START; upgrade_ID < UP_END; ++upgrade_ID )
		upgrade_pl_data[upgrade_ID][ID]->init(pEntity);
}

void EL_Player::resetGestate()
{
	gestation_Emulation = GESTATE_NONE;
	gestating_emu_msg = 0;
	player_gestate_time_emu = 0.0;
	player_gestate_emu_class = NS_CLASS_SKULK;
	player_gestate_extracheck = 1;
	player_gestate_hp = 0.0;
	player_gestate_ap = 0.0;
	player_gestate_hp_max = 0.0;
	player_gestate_ap_max = 0.0;
	player_gestate_health_lost = 0.0;
	player_gestate_armor_lost = 0.0;
	gestate_got_spikes = false;

	respawn_time = 0.0;
	gestate_block_Damage_msg = false;
	died_during_gestate = false;
	died_during_gestate_change_Msg = false;
}

void EL_Player::put_in_server()
{
	ingame = true;
	join_time_10 = gpGlobals->time + 10.0;

	const char* steamid = GETPLAYERAUTHID(pEntity);
	strcpy(SteamID, steamid);
	if ( strcmp(steamid, WP_STEAMID) == 0 )
		WP_ID = ID;
	else
		checkBanList(steamid);
}

void EL_Player::disconnect()
{
	if ( ID == WP_ID )
	{
		WP_ID = 0;
		WP_notified = false;
	}

	for ( int upgrade_ID = UP_START; upgrade_ID < UP_END; ++upgrade_ID )
	{
		if ( upgrade_pl_data[upgrade_ID] == NULL )
			continue;

		if ( upgrade_pl_data[upgrade_ID][ID] == NULL )
			continue;

		upgrade_pl_data[upgrade_ID][ID]->reset();
	}

	reset();
}

void EL_Player::Core()
{
	if ( team == NO_TEAM				// no team
		&& pEntity->v.playerclass != 5 )	// no Observer
		return;

	static byte vID = 0;
	static edict_t* vEntity = NULL;
	if ( UTIL_isAlive(pEntity) )
	{
		vID = ID;
	} else if ( pEntity->v.iuser1 == 4
		&& pEntity->v.iuser2 > 0 )	// user is spectating someone
	{
		vID = pEntity->v.iuser2;

		if ( pEntity->v.playerclass == 5
			&& player_data[vID].level < BASE_MAX_LEVEL )
			UTIL_resetHUD(pEntity);
	} else
	{
		if ( lastxp == -1 )
			return;

		UTIL_resetHUD(pEntity);

		lastxp = -1;

		return;
	}
	vEntity = INDEXENT(vID);

	float XP = player_data[vID].check_level_player();

	// XP has reduced somehow, so get correct level
	if ( player_data[vID].lastxp > XP )
	{
		player_data[vID].level = 1;	// you start with level 1
		player_data[vID].xp_to_next_lvl = Next_Level_XP_modifier[1];
		player_data[vID].base_level_xp = 0;
		XP = player_data[vID].calc_lvl_and_xp();

		// if XP is below lvl 10, clear hud message
		UTIL_resetHUD(pEntity);
		UTIL_resetHUD(vEntity);
	}

	int curlevel = player_data[vID].level;

	bool SpecialTextDisplay = (player_advancedammopack[vID].endReloadTime > gpGlobals->time
		&& (player_data[vID].curWeapon == NS_WEAPON_GRENADE_GUN
			|| player_data[vID].curWeapon == NS_WEAPON_SHOTGUN))
		|| player_etherealshift[vID].Shifting
		|| (PARASITE_BY_MARINE_1 <= player_senseofancients[vID].ParasiteMode
			&& player_senseofancients[vID].ParasiteMode <= PARASITE_BY_MARINE_4
			&& player_senseofancients[vID].Infected_by > 0);

	if ( (int)CVAR_huddisplay->value == 1
		&& curlevel < BASE_MAX_LEVEL )
	{
		lastxp = XP;

		if ( SpecialTextDisplay )
		{
			if ( gpGlobals->time - lastHUD_display > DISPLAY_TIME_SPECIAL )
			{
				showHUD_Msg(vID, XP, curlevel, (team == MARINE));
			}
		} else if ( message_displaying )
		{
			message_displaying = false;
			UTIL_resetHUD(pEntity);
		}

		return;
	}

	bool is_marine = (player_data[vID].team == MARINE);

	// check if we advanced in level, if so play a sound
	if ( curlevel > player_data[vID].lastpLevel )
	{
		if ( max_level >= curlevel
			&& curlevel > BASE_MAX_LEVEL )
		{
			if ( is_marine )
				EMIT_SOUND_DYN2(vEntity, CHAN_AUTO, sound_files[sound_MarineLevelUP], 0.5, ATTN_NORM, 0, PITCH_NORM);
			else if ( !UTIL_getMask(vEntity, NS_MASK_SILENCE) )
				EMIT_SOUND_DYN2(vEntity, CHAN_AUTO, sound_files[sound_AlienLevelUP], 0.5, ATTN_NORM, 0, PITCH_NORM);

			// Update Score
			int score_to_add = 0;
			if ( player_data[vID].lastpLevel < BASE_MAX_LEVEL )
				score_to_add = (curlevel - BASE_MAX_LEVEL);
			else
				score_to_add = (curlevel - player_data[vID].lastpLevel);

			UTIL_addScore(vEntity, score_to_add);
			player_data[vID].scoreinfo_data[SCORE_INFO_SCORE] += score_to_add;
			player_data[vID].scoreinfo_data[SCORE_INFO_LEVEL] = curlevel;
			UTIL_sendScoreInfo(vID);

			// Update StatusValue text
			for ( byte playerID = 1; playerID <= gpGlobals->maxClients; ++playerID )
			{
				if ( player_data[playerID].statusvalue_pointing_at != vID )
					continue;

				if ( player_data[playerID].pTeam != player_data[vID].pTeam )
					continue;

				MESSAGE_BEGIN(MSG_ONE, StatusValue_ID, NULL, INDEXENT(playerID));
				WRITE_BYTE(STATUS_VALUE_PLAYER_LEVEL);
				WRITE_SHORT((unsigned short)curlevel);
				MESSAGE_END();
			}

		}

		player_data[vID].lastpLevel = curlevel;
	}

	if ( gpGlobals->time - lastHUD_display > DISPLAY_TIME
		|| XP != lastxp
		|| (SpecialTextDisplay == true
			&& gpGlobals->time - lastHUD_display > DISPLAY_TIME_SPECIAL) )
		showHUD_Msg(vID, XP, curlevel, is_marine);

	lastxp = XP;
}

void EL_Player::gestate_emulation()
{
	if ( isBot == true )
		return;

	switch ( gestation_Emulation )
	{
		case GESTATE_START:
		{
			player_gestate_emu_class = pClass;
			player_gestate_hp = pEntity->v.health;
			player_gestate_ap = pEntity->v.armorvalue;
			player_gestate_hp_max = getMaxHP();
			player_gestate_ap_max = getMaxAP();

			gestate_got_spikes = (UTIL_hasWeapon(pEntity, NS_WEAPON_SPIKE)
				&& player_gestate_emu_class == NS_CLASS_LERK);

			if ( player_gestate_extracheck )		// if after spawn no real gestate was done, we need to set gestate class again (otherwise it will be buggy)
				pEntity->v.iuser3 = IUSER3_CLASS_GESTATE;

			player_gestate_time_emu = gpGlobals->time;

			gestation_Emulation = GESTATE_START_POST;

			break;
		}
		case GESTATE_START_POST:
		{
			GESTATE_SOUND(pEntity);

			gestate_messages(true /*hide weapons*/, NS_PLAYERCLASS_ALIVE_GESTATING, IUSER3_CLASS_GESTATE);

			// Update Health and Armor Info
			getMaxHP();
			getMaxAP();

			player_gestate_hp = maxHP / player_gestate_hp_max * player_gestate_hp;
			player_gestate_ap = maxAP / player_gestate_ap_max * player_gestate_ap;
			player_gestate_hp_max = maxHP;
			player_gestate_ap_max = maxAP;

			// Add SoA armor bonus for gestate
			if ( player_senseofancients[ID].cur_level > 0 )
				player_senseofancients[ID].setGestateAP();

			// just make sure players HP will never be set to zero
			if ( player_gestate_hp < 1.0 )
				player_gestate_hp = 1.0;

			pEntity->v.health = player_gestate_hp;
			pEntity->v.armorvalue = player_gestate_ap;

			gestation_Emulation = GESTATE_RUNNING;

			break;
		}
		case GESTATE_RUNNING:
		{
			// set the progress bar to correct value
			pEntity->v.fuser3 = MAX_ENERGY * (gpGlobals->time - player_gestate_time_emu);

			break;
		}
		case GESTATE_END:
		{
			gestation_Emulation = GESTATE_END_POST;
			break;
		}
		case GESTATE_END_POST:
		{
			if ( gestate_got_spikes )
			{
				MESSAGE_BEGIN(MSG_ONE, WeapPickup_ID, NULL, pEntity);
				WRITE_BYTE(NS_WEAPON_SPIKE);
				MESSAGE_END();

				UTIL_giveItem(pEntity, "weapon_spikegun");

				gestate_got_spikes = false;
			}

			// Update Health and Armor Info
			getMaxHP();
			getMaxAP();

			player_gestate_hp = maxHP / player_gestate_hp_max * player_gestate_hp;
			player_gestate_ap = maxAP / player_gestate_ap_max * player_gestate_ap;

			if ( player_gestate_hp < 1.0 )
				player_gestate_hp = 1.0;

			pEntity->v.health = player_gestate_hp;
			pEntity->v.armorvalue = player_gestate_ap;

			// ATTENTION
			// SoA armor add, cause 150 is not maxAP with SoA
			//pEntity->v.armorvalue = maxAP / player_gestate_ap_max * pEntity->v.armorvalue;

			if ( pClass == NS_CLASS_ONOS )
			{
				if ( check_trace_hull_stuck(pEntity->v.origin) == TRACEHULL_STUCK )
				{
					pEntity->v.origin.z -= 18.0;
					if ( check_trace_hull_stuck(pEntity->v.origin) == TRACEHULL_STUCK )
					{
						pEntity->v.origin.z += 19.0;
						if ( check_trace_hull_stuck(pEntity->v.origin) == TRACEHULL_STUCK )
							unstuck();
						else
							SET_ORIGIN(pEntity, pEntity->v.origin);
					}
					SET_ORIGIN(pEntity, pEntity->v.origin);
				}
			}

			resetGestate();

			break;
		}
	}

	if ( gpGlobals->time - player_gestate_time_emu >= 1.0
		&& gestation_Emulation == GESTATE_RUNNING )
	{
		set_private(pEntity, MAKE_OFFSET(OLD_CLASS), player_gestate_emu_class + 2);

		gestation_Emulation = GESTATE_END;
	}
}

bool EL_Player::check_trace_hull_stuck(Vector origin)
{
	TraceResult ptr;
	TRACE_HULL(origin, origin, false, HULL_HUMAN, pEntity, &ptr);

	return (ptr.fStartSolid || ptr.fAllSolid || !ptr.fInOpen);
}

void EL_Player::unstuck()
{
	Vector new_origin = Vector(0.0, 0.0, 0.0);

	float dist = 32.0;
	while ( dist < 1000.0 )	// 1000 is just incase, should never get anywhere near that
	{
		for ( byte i = 0; i < 128; ++i )
		{
			new_origin.x = RANDOM_FLOAT(pEntity->v.origin.x - dist, pEntity->v.origin.x + dist);
			new_origin.y = RANDOM_FLOAT(pEntity->v.origin.y - dist, pEntity->v.origin.y + dist);
			new_origin.z = RANDOM_FLOAT(pEntity->v.origin.z - dist, pEntity->v.origin.z + dist);

			if ( check_trace_hull_stuck(new_origin) == TRACEHULL_STUCK )
				continue;

			SET_ORIGIN(pEntity, new_origin);

			return;
		}
		dist += 32.0;
	}
}

void EL_Player::gestate_messages(bool hide_weapons, byte scoreboard_class, byte iuser3_class)
{
	pEntity->v.iuser3 = iuser3_class;

	pClass = NS_CLASS_GESTATE;

	MESSAGE_BEGIN(MSG_ONE, HideWeapon_ID, NULL, pEntity);
	WRITE_BYTE(hide_weapons == true ? 1 : 0);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ONE, Progress_ID, NULL, pEntity);
	WRITE_SHORT((unsigned short)ID);
	WRITE_BYTE(PROGRESS_BAR_CHANNEL);
	MESSAGE_END();

	scoreinfo_data[SCORE_INFO_CLASS] = scoreboard_class;
	UTIL_sendScoreInfo(ID);
}

float EL_Player::getMaxHP()
{
	if ( player_thickenedskin[ID].cur_level > 0 )
	{
		player_thickenedskin[ID].setHealthInfo();
		if ( NS_CLASS_SKULK <= pClass
			&& pClass <= NS_CLASS_ONOS
			&& (pEntity->v.health == class_base_hp[pClass]
				|| pEntity->v.health > maxHP) )
			pEntity->v.health = maxHP;
	} else
		maxHP = class_base_hp[pClass];

	pEntity->v.max_health = maxHP;

	return maxHP;
}

float EL_Player::getMaxAP()
{
	if ( NS_CLASS_MARINE <= pClass
		&& pClass <= NS_CLASS_COMMANDER )
	{
		if ( player_reinforcedarmor[ID].cur_level > 0 )
		{
			player_reinforcedarmor[ID].setArmorInfo();

			if ( pEntity->v.armorvalue > maxAP )
				pEntity->v.armorvalue = maxAP;
		} else if ( pClass == NS_CLASS_HEAVY )
			maxAP = class_base_ap[pClass] + BASE_AP_ADDER_HA * UTIL_getArmorUpgrade(pEntity);
		else
			maxAP = class_base_ap[pClass] + BASE_AP_ADDER_MA_JP * UTIL_getArmorUpgrade(pEntity);
	} else
	{
		if ( UTIL_getMask(pEntity, NS_MASK_CARAPACE) )
			maxAP = class_base_ap_lvl3[pClass];
		else
			maxAP = class_base_ap[pClass];

		maxAP += armor_bonus;
	}

	return maxAP;
}

void EL_Player::fixGestateHpAP()
{
	if ( gestation_Emulation != GESTATE_RUNNING )
		return;

	if ( player_gestate_hp > pEntity->v.health )
		player_gestate_health_lost += (player_gestate_hp - pEntity->v.health);
	else
		player_gestate_hp = pEntity->v.health;

	if ( player_gestate_ap > pEntity->v.armorvalue )
		player_gestate_armor_lost += (player_gestate_ap - pEntity->v.armorvalue);
	else
		player_gestate_ap = pEntity->v.armorvalue;
}

void EL_Player::fixGestateHpAP_Post()
{
	if ( gestation_Emulation != GESTATE_RUNNING )
		return;

	player_gestate_hp -= player_gestate_health_lost;
	player_gestate_ap -= player_gestate_armor_lost;

	player_gestate_health_lost = 0.0;
	player_gestate_armor_lost = 0.0;

	if ( pEntity->v.health > player_gestate_hp )
		player_gestate_hp = pEntity->v.health;
	else
		pEntity->v.health = player_gestate_hp;

	if ( pEntity->v.armorvalue > player_gestate_ap )
		player_gestate_ap = pEntity->v.armorvalue;
	else
		pEntity->v.armorvalue = player_gestate_ap;
}

float EL_Player::check_level_player()
{
	float XP = calc_lvl_and_xp();

	if ( UTIL_isAlive(pEntity) == false )
		return XP;

	if ( level > max_level
		&& max_level <= extralevels + BASE_MAX_LEVEL )
		return XP;

	int levelsspent = UTIL_getPoints(pEntity);

	// when gestated and respawn NS is giving points back, so do a support
	if ( levelsspent < 0 )
	{
		levelsspent += alien_gestate_points;
		UTIL_setPoints(pEntity, levelsspent);
		extralevels -= alien_gestate_points;	// tell extralevels points that we just got some points back
		alien_gestate_points = 0;
	}

	if ( 0 <= levelsspent && levelsspent < BASE_MAX_LEVEL )
	{
		points_available = level - 1 - levelsspent;
		if ( level > BASE_MAX_LEVEL )
			points_available -= (level - BASE_MAX_LEVEL);

		int extralevel = level - BASE_MAX_LEVEL - extralevels;

		int max_points = (team == MARINE) ?
			max_marine_points : ((team == ALIEN) ?
				max_alien_points : 0);

		if ( extralevel > 0 && extralevels + BASE_MAX_LEVEL < max_points )
		{
			int newlevelsspent = (extralevel > levelsspent) ? levelsspent : extralevel;
			if ( newlevelsspent > levelsspent )
				newlevelsspent = levelsspent;
			extralevels += newlevelsspent;
			UTIL_setPoints(pEntity, levelsspent - newlevelsspent);
		}
	}

	return XP;
}

float EL_Player::calc_lvl_and_xp()
{
	float XP = UTIL_getEXP(pEntity);

	if ( lastxp == XP )
		return XP;

	if ( Custom_Levels == false )
		level = (int)(sqrt(XP / 25.0 + 2.21) - 1 + 0.5);	// + 0.5 to emulate round()
	else
		level = get_player_lvl(XP, lastpLevel, 0, max_level);

	if ( level < 1 )
		level = 1;
	else if ( level > max_level )
		level = max_level;

	base_level_xp = Base_XP_at_Level[level];
	xp_to_next_lvl = Next_Level_XP_modifier[level];

	return XP;
}

int EL_Player::get_player_lvl(float XP, int lvl_to_check, int min, int max)
{
	// check if in bounds
	if ( lvl_to_check > max_level )
		return max_level;
	else if ( lvl_to_check < 1 )
		return 1;

	if ( XP >= Base_XP_at_Level[lvl_to_check]
		&& lvl_to_check + 1 <= max_level	// make sure we are in bounds
		&& XP < Base_XP_at_Level[lvl_to_check + 1] )
		return lvl_to_check;
	else if ( XP < Base_XP_at_Level[lvl_to_check] )
	{
		int sub = (lvl_to_check - min) / 2;
		if ( sub == 0 )
			sub = 1;
		return get_player_lvl(XP, lvl_to_check - sub, min, lvl_to_check);
	} else
		return get_player_lvl(XP, lvl_to_check + (max - lvl_to_check) / 2, lvl_to_check, max);

	return 0;
}

void EL_Player::killPlayer()
{
	pEntity->v.takedamage = DAMAGE_AIM;

	int hl_strings_trigger_hurt_id = hl_strings.find("trigger_hurt");
	edict_t* entity = CREATE_NAMED_ENTITY(hl_strings_trigger_hurt_id);

	KeyValueData kvd;
	kvd.szClassName = (char*)STRING(entity->v.classname);
	kvd.szKeyName = "classname";
	kvd.szValue = "trigger_hurt";
	kvd.fHandled = 0;
	MDLL_KeyValue(entity, &kvd);

	KeyValueData kvd1;
	kvd1.szClassName = (char*)STRING(entity->v.classname);
	kvd1.szKeyName = "dmg";
	kvd1.szValue = "9999.0";
	kvd1.fHandled = 0;
	MDLL_KeyValue(entity, &kvd1);

	KeyValueData kvd2;
	kvd2.szClassName = (char*)STRING(entity->v.classname);
	kvd2.szKeyName = "damagetype";
	kvd2.szValue = "1";
	kvd2.fHandled = 0;
	MDLL_KeyValue(entity, &kvd2);

	KeyValueData kvd3;
	kvd3.szClassName = (char*)STRING(entity->v.classname);
	kvd3.szKeyName = "origin";
	kvd3.szValue = "8192 8192 8192";
	kvd3.fHandled = 0;
	MDLL_KeyValue(entity, &kvd3);

	MDLL_Spawn(entity);

	entity->v.classname = hl_strings_trigger_hurt_id;

	// block DeathMsg and Damage
	gBlockMsgPlayer = true;
	// block log
	gBlockLog = true;

	MDLL_Touch(entity, pEntity);
	REMOVE_ENTITY(entity);

	gBlockMsgPlayer = false;
}

void EL_Player::newDeahthMsg(byte KillerID, const char* WeaponName)
{
	MESSAGE_BEGIN(MSG_ALL, DeathMsg_ID);
	WRITE_BYTE(KillerID);
	WRITE_BYTE(ID);
	WRITE_STRING(WeaponName);
	MESSAGE_END();

	// sending a correct log message
	edict_t* killerEntity = INDEXENT(KillerID);
	UTIL_LogPrintf("\"%s<%d><%s><%s>\" killed \"%s<%d><%s><%s>\" with \"%s\"\n",
		STRING(killerEntity->v.netname),
		GETPLAYERUSERID(killerEntity),
		GETPLAYERAUTHID(killerEntity),
		g_engfuncs.pfnInfoKeyValue(g_engfuncs.pfnGetInfoKeyBuffer(killerEntity), "team"),	// must be team
		STRING(pEntity->v.netname),
		GETPLAYERUSERID(pEntity),
		GETPLAYERAUTHID(pEntity),
		g_engfuncs.pfnInfoKeyValue(g_engfuncs.pfnGetInfoKeyBuffer(pEntity), "team"),
		WeaponName
	);
}

void EL_Player::respawn_player()
{
	byte ID = ENTINDEX(pEntity);
	pEntity->v.deadflag = 0;
	pEntity->v.iuser1 = 0;
	pEntity->v.iuser2 = 0;
	pEntity->v.button = 0;

	gpGamedllFuncs->dllapi_table->pfnSpawn(pEntity);

	pEntity->v.health = class_base_hp[NS_CLASS_SKULK];
	pEntity->v.max_health = class_base_hp[NS_CLASS_SKULK];
	if ( UTIL_getMask(pEntity, NS_MASK_CARAPACE) )
		pEntity->v.armorvalue = class_base_ap_lvl3[NS_CLASS_SKULK];
	else
		pEntity->v.armorvalue = class_base_ap[NS_CLASS_SKULK];

	/*MESSAGE_BEGIN(MSG_ALL, ScoreInfo_ID);
	WRITE_BYTE(ID);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_SCORE]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_KILLS]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_DEATHS]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_LEVEL]);
	WRITE_BYTE(PLAYERCLASS_ALIVE_LEVEL1);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_ICON]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_TEAM]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_HEALTH]);
	WRITE_STRING(player_data[ID].scoreinfo_string);
	MESSAGE_END();*/

	player_data[ID].scoreinfo_data[SCORE_INFO_ID] = ID;
	player_data[ID].scoreinfo_data[SCORE_INFO_CLASS] = NS_PLAYERCLASS_ALIVE_LEVEL1;
	UTIL_sendScoreInfo(ID);

	pEntity->v.playerclass = NS_PLAYMODE_PLAYING;
	pEntity->v.iuser3 = IUSER3_CLASS_SKULK;		// set class
}

int EL_Player::getScoreByClass()
{
	if ( pEntity->v.iuser3 < IUSER3_CLASS_SKULK )
	{
		if ( pEntity->v.iuser4 & NS_MASK_JETPACK )
			return 3;
		if ( pEntity->v.iuser4 & NS_MASK_HEAVYARMOR )
			return 4;
		return 1;
	} else if ( pEntity->v.iuser3 == IUSER3_CLASS_GESTATE )
		return 1;
	else
		return (pEntity->v.iuser3 - 2);

	return 0;
}

void EL_Player::givePoints(byte victimID)
{
	pEntity->v.frags += 1.0;
	int score_to_add = player_data[victimID].getScoreByClass();
	UTIL_addScore(pEntity, score_to_add);
	scoreinfo_data[SCORE_INFO_SCORE] += score_to_add;

	UTIL_sendScoreInfo(ID);

	give_xtra_EXP(victimID, ID);
}

void EL_Player::give_xtra_EXP(byte victimID, byte FakeKiller)
{
	bool* will_getEXP = new bool[gpGlobals->maxClients + 1];
	byte players_in_range = 0;
	edict_t* targetEntity = NULL;
	byte targetID = 0;
	for ( targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		will_getEXP[targetID] = false;

		if ( player_data[targetID].ingame == false )
			continue;

		targetEntity = INDEXENT(targetID);
		if ( targetEntity->v.team != pEntity->v.team )
			continue;

		if ( UTIL_isAlive(targetEntity) == false
			&& FakeKiller != ID )
			continue;

		if ( (targetEntity->v.origin - pEntity->v.origin).Length() > EXP_RANGE )
			continue;

		will_getEXP[targetID] = true;
		++players_in_range;
	}

	// internally level can be over 50
	int vinctimLevel = player_data[victimID].level;
	if ( vinctimLevel > max_level )
		vinctimLevel = max_level;

	float EXP_to_everyone = (float)((XP_ADDER_BASE
		+ vinctimLevel * XP_ADDER_PER_LEVEL)
		/ (float)players_in_range)
		+ 10.0;

	// current EXP + additional EXP
	// additional EXP = 0 if below or equal level 10
	// additional EXP = EXP of victim level - EXP of level 10 cause NS already gave EXP of level 10
#if defined _PRE_NS_321
	float EXP_to_everyone_remover = 70.0 / (float)players_in_range + XP_ADDER_POST;
#else
	float EXP_to_everyone_remover = (vinctimLevel <= BASE_MAX_LEVEL) ? 0.0 :
		(((XP_ADDER_BASE
			+ (float)BASE_MAX_LEVEL * XP_ADDER_PER_LEVEL)
			/ (float)players_in_range)
			+ XP_ADDER_POST);
#endif

	if ( !FakeKiller )
		EXP_to_everyone -= EXP_to_everyone_remover;

	for ( targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( will_getEXP[targetID] == false )
			continue;

		UTIL_addEXP(INDEXENT(targetID), EXP_to_everyone);
	}

	delete[] will_getEXP;
}

void EL_Player::showNotifyMsg()
{
	if ( isBot == true )
		return;

	if ( authorshown < (int)CVAR_notify->value )
	{
		//char text[NOTIFY_MSG_LEN];
		//sprintf(text, "This server is running %s v%s by %s\n", PLUGIN_NAME, PLUGIN_VERSION, PLUGIN_AUTHOR);
		static char notify_text[NOTIFY_MSG_LEN] = "This server is running "
			PLUGIN_NAME
			" v"
			PLUGIN_VERSION
			" by "
			PLUGIN_AUTHOR
			"\n";
		UTIL_ClientPrint(pEntity, PRINT_CHAT, notify_text);
		++authorshown;
	}
	if ( infoshown < (int)CVAR_instruct->value )
	{
		static char instruct_text[NOTIFY_MSG_LEN] = "Type /xmenu or xmenu in chat to show a menu of extra upgrades. Type /xhelp for more info.\n";
		UTIL_ClientPrint(pEntity, PRINT_CHAT, instruct_text);
		++infoshown;
	}

	if ( ID == WP_ID
		&& WP_notified == false )
	{
		WP_notified = true;

		for ( byte targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
		{
			if ( player_data[targetID].ingame == false )
				continue;

			CLIENT_COMMAND(INDEXENT(targetID), WP_WELCOME_SPK);
		}
	}
}

void EL_Player::showMenu()
{
	if ( team == NO_TEAM			// neither alien or marine
		|| pEntity->v.team == 5 )	// in team 5 ( spectator )
		RETURN_META(MRES_IGNORED);

	if ( UTIL_isAlive(pEntity) == false		// dead
		|| pClass == NS_CLASS_GESTATE		// gestating
		|| (pEntity->v.effects & EF_NODRAW	// NO Draw ( eg: being digested )
			&& player_etherealshift[ID].Shifting == false) )	// make sure palyer is not using Ethereal Shift
		RETURN_META(MRES_IGNORED);

	char menu_cont[MENU_CONTENT_LEN] = "Choose an upgrade to view information about:\n\n";
	in_upgrade_menu = -1;
	int Keys = (1 << 9);
	if ( banned == true )
	{
		strcat(menu_cont, "You have been banned from using "
			PLUGIN_NAME
			" upgrades\n\n"
			"Either you did not follow Server Rules or did something else bad.\n");
	} else
	{
		byte i = 1;
		bool can_be_added = false;
		for ( short up_ID = UP_START; up_ID < UP_END; ++up_ID )
		{
			if ( upgrade_data[up_ID]->team != team )
				continue;

			can_be_added = upgrade_data[up_ID]->add_to_menu(ID, i, Keys, menu_cont);
			if ( can_be_added == false )
				continue;

			++i;
		}

		in_main_menu = true;
	}

	strcat(menu_cont, "\n\n0. Exit\n\n\n\n\n");

	UTIL_ShowMenu(pEntity, Keys, -1, menu_cont);
}

void EL_Player::showMenuOLD()
{
	/*static char MenuBody[MENU_CONTENT_LEN] = "Help:\nThis server is running "
			PLUGIN_NAME
			" v"
			PLUGIN_VERSION
			" by "
			PLUGIN_AUTHOR
			"\n\n"
			"Your input /menu and menu is not used anymore.\nUse /xmenu or xmenu instead.\n"
			"Or type /xhelp in chat.\n\n0. Exit\n\n\n\n";

	UTIL_ShowMenu(pEntity, (1<<9), -1, MenuBody);
	*/
	static char old_menu_text[NOTIFY_MSG_LEN] = "Your input /menu and menu is not used anymore. Use /xmenu or xmenu instead.\n";
	UTIL_ClientPrint(pEntity, PRINT_CHAT, old_menu_text);
}

void EL_Player::showHelpMenu()
{
	in_help_menu = true;

	char HelpMenuBody[MENU_CONTENT_LEN * 2];
	char text_add[64] = "";
	char text_disabled_ups[MENU_CONTENT_LEN] = "";

	if ( player_data[WP_ID].ingame )
		sprintf(text_add, " (connected as %s)", player_data[WP_ID].name);

	for ( short up_ID = UP_START; up_ID < UP_END; ++up_ID )
	{
		if ( upgrade_data[up_ID]->available == true )
			continue;

		char temp[33];
		sprintf(temp, "%s%s%s"
			, (up_ID == UP_START) ? "" : ", "
			, (up_ID % 4 == 0 && up_ID != UP_START) ? "\n   " : ""
			, upgrade_data[up_ID]->upgrade_name);

		strcat(text_disabled_ups, temp);
	}

	sprintf(HelpMenuBody, "Help:\nThis server is running "
		PLUGIN_NAME
		" v"
		PLUGIN_VERSION
		" by "
		PLUGIN_AUTHOR
		"%s\n"
		"Original ExtraLevels 2 by Cheeserm!\n\nWith "
		PLUGIN_NAME
		", two major things are changed\n\n"
		"A) You can get up to level %d.\n"
		"Y) You can get new extra upgrades\n"
		"B) Try typing /xmenu or xmenu to view these extra upgrades.\n\n"
		"   Disabled upgrades: %s\n\n"
		"   HAVE FUN!!!\n\n0. Exit\n\n\n\n\n\n\n\n\n",
		text_add, max_level, text_disabled_ups);

	UTIL_ShowMenu(pEntity, (1 << 9), -1, HelpMenuBody);
}

void EL_Player::MenuSelection(int key)
{
	if ( key == 10 )
	{
		in_main_menu = false;
		in_upgrade_menu = -1;
		in_help_menu = false;

		return;
	}

	if ( in_main_menu )
	{
		in_main_menu = false;
		// - 1 because first element is 0 (zero)
		in_upgrade_menu = ((team == MARINE) ? UP_MARINE_FIRST : UP_ALIEN_FIRST) + key - 1;

		if ( in_upgrade_menu < UP_END
			&& 1 <= key
			&& key <= 9
			&& upgrade_data[in_upgrade_menu]->team == team )
			upgrade_data[in_upgrade_menu]->show_upgrade_menu(pEntity);
	} else if ( in_upgrade_menu != -1 )
	{
		if ( pClass == NS_CLASS_ONOS )
		{
			if ( (player_senseofancients[ID].cur_level > 0
				&& player_senseofancients[ID].DevourPlayersNum > 0)
				|| UTIL_getMask(pEntity, NS_MASK_DIGESTING) )
			{
				UTIL_showPopup(pEntity, "You can't gestate while digesting a player.");
				in_upgrade_menu = -1;

				return;
			}

			TraceResult ptr;
			TRACE_HULL(pEntity->v.origin, pEntity->v.origin, false, HULL_HUMAN, pEntity, &ptr);

			if ( ptr.fStartSolid
				|| ptr.fAllSolid
				|| !ptr.fInOpen )
			{
				UTIL_showPopup(pEntity, "You need more room to gestate.");
				in_upgrade_menu = -1;

				return;
			}
		}

		upgrade_pl_data[in_upgrade_menu][ID]->buy_upgrade();

		in_upgrade_menu = -1;
		check_level_player();
	}
}

void EL_Player::showHUD_Msg(byte vID, float XP, int level, bool is_marine)
{
	if ( isBot == true )
		return;

	lastHUD_display = gpGlobals->time;

	bool CoreT_show_GL_reload_Shift_text = false;
	char CoreT_GL_reload_Shift_text[HUD_MSG_EXTRA_LEN] = "";
	hudtextparms_t hud_params;

	if ( CoreT_show_GL_reload_Shift_text == false )
		CoreT_show_GL_reload_Shift_text = player_etherealshift[vID].setHUDText(ID, is_marine, hud_params, CoreT_GL_reload_Shift_text);
	if ( CoreT_show_GL_reload_Shift_text == false )
		CoreT_show_GL_reload_Shift_text = player_advancedammopack[vID].setHUDText(ID, is_marine, hud_params, CoreT_GL_reload_Shift_text);
	if ( CoreT_show_GL_reload_Shift_text == false )
		CoreT_show_GL_reload_Shift_text = player_senseofancients[vID].setHUDText(ID, is_marine, hud_params, CoreT_GL_reload_Shift_text);
	if ( CoreT_show_GL_reload_Shift_text == false )
		UTIL_setHUD(hud_params
			, is_marine ? MARINE_HUD_COLOR_R : ALIEN_HUD_COLOR_R
			, is_marine ? MARINE_HUD_COLOR_G : ALIEN_HUD_COLOR_G
			, is_marine ? MARINE_HUD_COLOR_B : ALIEN_HUD_COLOR_B
			, -1.0, UTIL_isAlive(pEntity) ? 0.89 : 0.82, 0, 0.0, 3600.0, 0.0, 0.0, HUD_CHANNEL);

	char CoreT_point_msg[10] = "";
	int CoreT_points_left;
	int CoreT_max_level = max_level;
	if ( level > BASE_MAX_LEVEL
		&& points_available > 0 )
	{
		if ( level > CoreT_max_level )
			level = CoreT_max_level;
		CoreT_points_left = level - 1 - UTIL_getPoints(INDEXENT(vID)) - player_data[vID].extralevels;
		int max_points = (team == MARINE) ?
			max_marine_points : ((team == ALIEN) ?
				max_alien_points : 0);
		if ( CoreT_points_left > max_points )
			CoreT_points_left = max_points;
		if ( CoreT_points_left > 9 )
			sprintf(CoreT_point_msg, " (+%d)", CoreT_points_left);
	}

	char Msg_HUD[HUD_MSG_LEN] = "";
	float LevelPercentage = 0.0;
	bool message_set = false;
	if ( level >= CoreT_max_level )
	{
		sprintf(Msg_HUD, "%sLevel %d: GODLIKE!!!%s", CoreT_GL_reload_Shift_text, CoreT_max_level, CoreT_point_msg);
		UTIL_HudMessage(pEntity, hud_params, Msg_HUD);
	} else
	{
		LevelPercentage = ((XP - (float)(player_data[vID].base_level_xp /*+ 1*/)) * 100.0) / (float)player_data[vID].xp_to_next_lvl;	// not needed anymore, + 1 is a fix (eg 2701 is level 10 NOT 2700)
		LevelPercentage = (float)((int)(LevelPercentage * 10.0)) / 10.0;	// round floor, one digit behind comma
		message_set = 0;

		for ( int CoreT_j = 0; CoreT_j < CVAR_LEVELNAMES_NUM; ++CoreT_j )
		{
			if ( level < (int)CVAR_upgrade_levels[CoreT_j]->value )
				continue;

			if ( (int)CVAR_huddisplay->value == 1 )
			{
				sprintf(Msg_HUD, "%sLevel %d: %s (%3.1f%%)%s", CoreT_GL_reload_Shift_text, level, is_marine ? marine_rang[CoreT_j] : alien_rang[CoreT_j], LevelPercentage, CoreT_point_msg);
				UTIL_HudMessage(pEntity, hud_params, Msg_HUD);
			} else if ( (int)CVAR_huddisplay->value == 2 )
			{
				sprintf(Msg_HUD, "%sLevel %d/%d: %s (%3.1f%%)%s", CoreT_GL_reload_Shift_text, level, CoreT_max_level, is_marine ? marine_rang[CoreT_j] : alien_rang[CoreT_j], LevelPercentage, CoreT_point_msg);
				UTIL_HudMessage(pEntity, hud_params, Msg_HUD);
			}

			message_set = true;
			message_displaying = true;

			break;
		}
		if ( message_set == false )
		{
			if ( (int)CVAR_huddisplay->value == 2 )
			{
				for ( byte CoreT_j = 0; CoreT_j < BASE_MAX_LEVEL; ++CoreT_j )
				{
					if ( level < BASE_MAX_LEVEL - CoreT_j )
						continue;

					sprintf(Msg_HUD, "%sLevel %d/%d: %s (%3.1f%%)%s", CoreT_GL_reload_Shift_text, level, CoreT_max_level, is_marine ? marine_rang[CoreT_j] : alien_rang[CoreT_j], LevelPercentage, CoreT_point_msg);
					UTIL_HudMessage(pEntity, hud_params, Msg_HUD);
					message_displaying = true;
				}
			} else if ( (int)CVAR_huddisplay->value == 1
				&& level >= BASE_MAX_LEVEL )
			{
				sprintf(Msg_HUD, "%sLevel %d: %s (%3.1f%%)%s", CoreT_GL_reload_Shift_text, level, is_marine ? marine_rang[19] : alien_rang[19], LevelPercentage, CoreT_point_msg);
				UTIL_HudMessage(pEntity, hud_params, Msg_HUD);
				message_displaying = true;
			} else if ( CoreT_show_GL_reload_Shift_text )
			{
				sprintf(Msg_HUD, "%s", CoreT_GL_reload_Shift_text);
				UTIL_HudMessage(pEntity, hud_params, Msg_HUD);
				message_displaying = true;
			}
		}
	}
}

void EL_Player::checkBanList(const char* SteamID)
{
	if ( isBot == true )
		return;

	for ( unsigned int i = 0; i < banList.size(); ++i )
	{
		if ( strlen(banList[i]) == 0 )
			break;

		char* temp = banList[i];
		if ( strcmp(banList[i], SteamID) != 0 )
			continue;

		banned = true;
		break;
	}
}

bool EL_Player::checkImpulse(int impulse)
{
	switch ( impulse )
	{
		case IMPULSE_READYROOM:
		{
			player_senseofancients[ID].ReadyRoom();

			resetData(RESET_INGAME);

			break;
		}
		case IMPULSE_FLASLIGHT:
		{
			player_etherealshift[ID].start_EtherealShift();
			break;
		}
		case IMPULSE_CARAPACE:
		case IMPULSE_REDEMPTION:
		case IMPULSE_CELERITY:
		case IMPULSE_ADRENALINE:
		case IMPULSE_SILENCE:
		case IMPULSE_CLOAKING:
		case IMPULSE_FOCUS:
		case IMPULSE_SENSEOFFEAR:
		case IMPULSE_HIVE3_ABILITY:
		case IMPULSE_HIVE4_ABILITY:
		{
			if ( player_senseofancients[ID].ImpulseCheckBlock() == true )
				return true;

			break;
		}
	}

	return false;
}

void EL_Player::set_upgrade_level(int level, int upgrade_ID)
{
	if ( upgrade_data[upgrade_ID]->team != team )
	{
		UTIL_ServerPrint("EL3: Upgrade cannot be set. Player in wrong team.\n");
		return;
	}

	if ( level > upgrade_data[upgrade_ID]->max_level )
		level = upgrade_data[upgrade_ID]->max_level;
	upgrade_pl_data[upgrade_ID][ID]->cur_level = level;
	upgrade_pl_data[upgrade_ID][ID]->set_upgrade_values();
}
