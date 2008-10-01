//here should be the new versions of functions we get called with our hooks

#include "plugin.h"
#include "hookedfunctions.h"
#include <usercmd.h>

#include "ns_const.h"
#include "utilfunctions.h"
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
#include "events.h"

//#include <sdk_util.h> //useful almost everywhere

// global
bool gBlockLog = false;
bool gBlockMsgPlayer = false;
enginefuncs_t *g_pEngTable = NULL;
enginefuncs_t *g_pengfuncsTable = NULL;
enginefuncs_t *g_pengfuncsTable_Post = NULL;
DLL_FUNCTIONS *g_pFunctionTable = NULL;
DLL_FUNCTIONS *g_pFunctionTable_Post = NULL;
bool gIgnore_Self_Send_Msg = false;

base_upgrade_data *upgrade_data[UP_END];
base_upgrade_pl_data *upgrade_pl_data[UP_END][MAX_PLAYERS_PLUS1];

// private
bool initialized = false;
short el3_paused = 0;

bool gBlockMsg = false;
float g_next_CVAR_check = 0.0;

int Msg_type = 0;
byte Msg_receiver = 0;
int Msg_arg_num = 0;
int Msg_stored_data = 0;
int Msg_stored_data2 = 0;
bool Msg_correct_data = 0;
char Msg_stored_string[32];

int Msg_type_Post = 0;
byte Msg_receiver_Post = 0;
int Msg_arg_num_Post = 0;
int Msg_stored_data_Post = 0;
bool Msg_correct_data_Post = 0;

int Spawn( edict_t *pEntity )
{
	// check if running classic or combat
	const char *mapname = STRING(gpGlobals->mapname);
	if ( strlen(mapname) < 2
		|| ( mapname[0] != 'c' && mapname[0] != 'C' )
		|| ( mapname[1] != 'o' && mapname[1] != 'O' )
		|| mapname[2] != '_' )
	{
		Cleanup_Hooks();
		el3_paused = -1;
		
		RETURN_META_VALUE(MRES_IGNORED, 0);
	}
	
	Cleanup_Init();
	
	if ( initialized == true )
		RETURN_META_VALUE(MRES_IGNORED, 0);
	
	REG_SVR_COMMAND("el3", el3_main);
	
	// Init upgrade_data
	upgrade_data[UP_C] = &data_cybernetics;
	upgrade_data[UP_TS] = &data_thickenedskin;
	upgrade_data[UP_RA] = &data_reinforcedarmor;
	upgrade_data[UP_ES] = &data_etherealshift;
	upgrade_data[UP_NA] = &data_nanoarmor;
	upgrade_data[UP_BL] = &data_bloodlust;
	upgrade_data[UP_AA] = &data_advancedammopack;
	upgrade_data[UP_H] = &data_hunger;
	upgrade_data[UP_SF] = &data_staticfield;
	upgrade_data[UP_AV] = &data_acidicvengeance;
	upgrade_data[UP_UA] = &data_uraniumammo;
	upgrade_data[UP_SOA] = &data_senseofancients;
	upgrade_data[UP_BS] = &data_blindingsurge;
	upgrade_data[UP_LS] = &data_lifesheath;
	
	// Init upgrade_player_data
	int i = 0;
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_C][i] = &player_cybernetics[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_TS][i] = &player_thickenedskin[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_RA][i] = &player_reinforcedarmor[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_ES][i] = &player_etherealshift[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_NA][i] = &player_nanoarmor[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_BL][i] = &player_bloodlust[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_AA][i] = &player_advancedammopack[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_H][i] = &player_hunger[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_SF][i] = &player_staticfield[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_AV][i] = &player_acidicvengeance[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_UA][i] = &player_uraniumammo[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_SOA][i] = &player_senseofancients[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_BS][i] = &player_blindingsurge[i];
	for ( i = 0; i < MAX_PLAYERS_PLUS1; ++i )
		upgrade_pl_data[UP_LS][i] = &player_lifesheath[i];
	
	el3_config_reload();
	
	initialized = true;
	
	RETURN_META_VALUE(MRES_IGNORED, 0);
}

void el3_main( )
{
	int args = CMD_ARGC();
	if ( args < 2 )
	{
		UTIL_ServerPrint("EL3 commands:\n"
			"on\n"
			"off\n"
			"config_reload\n"
			"set_upgrades\n"
			);
		return;
	}
	
	const char* arg2 = CMD_ARGV(1);
	if ( strcmp(arg2, "on") == 0 )
	{
		if ( el3_paused == 0 )
			UTIL_ServerPrint("[" PLUGIN_NAME "] Already running\n");
		else if ( el3_paused != -1 )
		{
			el3_paused = 0;
			Set_Hooks_basic();
		}
		return;
	}else if ( strcmp(arg2, "off") == 0 )
	{
		if ( el3_paused == 1 )
			UTIL_ServerPrint("[" PLUGIN_NAME "] Already paused\n");
		else if ( el3_paused != -1 )
		{
			el3_paused = 1;
			Cleanup_Hooks_basic();
		}
		return;
	}else if ( strcmp(arg2, "config_reload") == 0 )
	{
		el3_config_reload();
	}else if ( strcmp(arg2, "set_upgrades") == 0 )
	{
		el3_set_upgrade(args);
	}
}

void el3_config_reload( )
{
	for ( unsigned int i = 0; i < banList.size(); ++i )
		delete(banList[i]);
	banList.clear();
	
	UTIL_getBanData();
	
	FILE *file = fopen(config_file, "r");
	if ( file == NULL )
	{
		UTIL_LogPrintf( "[" PLUGIN_NAME "] Unable to load configfile \"%s\"\n", config_file);
		UTIL_ServerPrint( "[" PLUGIN_NAME "] Unable to load configfile \"%s\"\n", config_file);
	}else
		fclose(file);
	
	init_xplevel_data();
	
	// init is setting available points too
	max_marine_points = BASE_MAX_MARINE_POINTS;
	max_alien_points = BASE_MAX_ALIEN_POINTS;
	
	for ( short i = UP_START; i < UP_END; ++i )
		upgrade_data[i]->init();
}

void el3_set_upgrade( int args )
{
	if ( args < 5 )
	{
		if ( args == 3 )
		{
			const char* arg3 = CMD_ARGV(2);
			if( strcmp(arg3, "list") == 0 )
			{
				UTIL_ServerPrint("[" PLUGIN_NAME "] UpgradeSymbols:\n"
					"C = Cyber / RA = Reinforced / NA = Nanoarmor / AA = AdvAmmo\n"
					"SF = StaticField / UA = UraniumAmmo / BS = BlindingSurge\n"
					"TS = ThickenedSkin / ES = EtherealShift / BL = BloodLust / H = Hunger\n"
					"AV = AcidicVengeance / SOA = SenseOfAncients / LS = LifeSheath\n"
					);
				return;
			}
		}
		
		UTIL_ServerPrint("Usage: el3_set_upgrade <PlayerID> <UpgradeSymbol> <NewLevel>\n"
				"or \"el3 set_upgrade list\" to show the list of UpgradeSymbols\n");
		
		return;
	}
		
	byte ID = atoi(CMD_ARGV(2));
	const char *upgrade_symbol = CMD_ARGV(3);
	int level = atoi(CMD_ARGV(4));
	int upgrade_ID = -1;
	for ( int i = 0; i < UP_END; ++i )
	{
		if ( strcmp(upgrade_symbol, upgrade_symbols[i]) != 0 )
			continue;
		
		upgrade_ID = i;
		break;
	}

#ifdef _DEBUG
	if ( ID == 0
		&& upgrade_symbol[0] == '0' )
	{
		if ( level == 0 )
			debug_running = false;
		else
			debug_running = true;
		
		UTIL_ServerPrint("[" PLUGIN_NAME "] Logging has been %s\n", (level != 0) ? "enabled" : "disabled");
		return;
	}
#endif
	
	if ( upgrade_ID == -1 )
	{
		UTIL_ServerPrint("[" PLUGIN_NAME "] Upgrade \"%s\" not found\n", upgrade_symbol);
		return;
	}
	
	if ( level < 0 )
		level = 0;
	
	if ( ID == 0 )
	{
		UTIL_ServerPrint("[" PLUGIN_NAME "] %s has been %s\n", upgrade_data[upgrade_ID]->upgrade_name, (level != 0) ? "enabled" : "disabled");
		if ( level == 0 )
			upgrade_data[upgrade_ID]->available = false;
		else
			upgrade_data[upgrade_ID]->available = true;
		
		return;
	}
	
	if ( player_data[ID].ingame == false )
	{
		UTIL_ServerPrint("[" PLUGIN_NAME "] Player with ID: %d not connected\n", ID);
		return;
	}
	
	player_data[ID].set_upgrade_level(level, upgrade_ID);
}

void KeyValue( edict_t *pentKeyvalue , KeyValueData *pkvd )
{
	if ( Hive_ID != NULL && Hive_ID2 != NULL )
		RETURN_META(MRES_IGNORED);
	
	// make sure string is valid or crash
	if ( pkvd->szClassName == NULL )
		RETURN_META(MRES_IGNORED);
	
	if ( strcmp(pkvd->szClassName, "team_hive") != 0 )
		RETURN_META(MRES_IGNORED);
	
	if ( strcmp(pkvd->szKeyName, "teamchoice") != 0 )
		RETURN_META(MRES_IGNORED);
	
	if ( strcmp(pkvd->szValue, "2") == 0
		|| strcmp(pkvd->szValue, "1") == 0 )	// yes hive for team 1 !!!
							// unbelieveble but true. eg: co_angst, co_faceoff, ns_metal
		Hive_ID = pentKeyvalue;
	else
		Hive_ID2 = pentKeyvalue;
	
	RETURN_META(MRES_IGNORED);
}

void ServerActivate_Post( edict_t *pEdictList , int edictCount , int clientMax )
{
	SAFE_USER_MSG(TextMsg_ID, "TextMsg", -1);
	
	SAFE_USER_MSG(HUDText2_ID, "HudText2", -1);
	SAFE_USER_MSG(AmmoPickup_ID, "AmmoPickup", -1);
	SAFE_USER_MSG(SetTech_ID, "SetTech", -1);
	SAFE_USER_MSG(ScoreInfo_ID, "ScoreInfo", -1);
	SAFE_USER_MSG(HideWeapon_ID, "HideWeapon", -1);
	SAFE_USER_MSG(Progress_ID, "Progress", -1);
	SAFE_USER_MSG(WeapPickup_ID, "WeapPickup", -1);
	SAFE_USER_MSG(StatusValue_ID, "StatusValue", -1);
	SAFE_USER_MSG(ScreenFade_ID, "ScreenFade", -1);
	
	SAFE_USER_MSG(DeathMsg_ID, "DeathMsg", -1);
	SAFE_USER_MSG(CurWeapon_ID, "CurWeapon", -1);
	SAFE_USER_MSG(Particles_ID, "Particles", -1);
	SAFE_USER_MSG(Damage_ID, "Damage", -1);
	SAFE_USER_MSG(TeamInfo_ID, "TeamInfo", -1);
	SAFE_USER_MSG(ShowMenu_ID, "ShowMenu", -1 );
	
	Spore_Event_ID = PRECACHE_EVENT(1, Spore_Event_Name);
	Cloak_Event_ID = PRECACHE_EVENT(1, Cloak_Event_Name);
	Umbra_Event_ID = PRECACHE_EVENT(1, Umbra_Event_Name);
	Particle_Event_ID = PRECACHE_EVENT(1, Particle_Event_Name);
	HealingSpray_Event_ID = PRECACHE_EVENT(1, HealingSpray_Event_Name);
	
	FuseLight = PRECACHE_MODEL("sprites/glow01.spr");
	
	if ( Hive_ID2 == NULL )
		Hive_ID2 = Hive_ID;	// make sure no NULL pointer is used
	
	// Find config path
	UTIL_getConfigFilenames();
	
	precacheSounds();
	for ( short i = 0; i < UP_END; ++i )
		upgrade_data[i]->precache();
	
	RETURN_META(MRES_IGNORED);
}

void ServerDeactivate_Post( void )
{
	data_senseofancients.SporeData.clear();
	
	Set_Hooks();
	
	RETURN_META(MRES_IGNORED);
}

void ClientUserInfoChanged_Post( edict_t *pEntity , char *infobuffer )
{
#ifdef _DEBUG
	UTIL_LogDebug("CUICPs");
#endif
	
	byte ID = ENTINDEX(pEntity);
	if ( player_data[ID].ingame )
	{
		const char *name = INFOKEY_VALUE(infobuffer, "name");
		strcpy(player_data[ID].name, name);
	}else if ( pEntity->v.flags & FL_FAKECLIENT )
	{
		player_data[ID].init(pEntity);
		player_data[ID].ingame = true;
		player_data[ID].isBot = true;
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("CUICPe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void ClientCommand( edict_t *pEntity )
{
#ifdef _DEBUG
	UTIL_LogDebug("CCs");
#endif
	
	const char* szUserCommand = CMD_ARGV(0);
	if( szUserCommand[0] == '\0' )
		return;
	
	byte ID = ENTINDEX(pEntity);
	if ( strcmp(szUserCommand, "readyroom") == 0 )
	{
		if ( player_senseofancients[ID].cur_level > 0 )
			player_senseofancients[ID].Origin_before_Redeem = pEntity->v.origin;
	}else if ( strcmp(szUserCommand, "say") == 0
		|| strcmp(szUserCommand, "say_team") == 0 )
	{
		const char* say_text = CMD_ARGV(1);
		if ( strcmp(say_text, "/xhelp") == 0 )
		{
			player_data[ID].showHelpMenu();
			
#ifdef _DEBUG
			UTIL_LogDebug("CCeh\n");
#endif
			
			RETURN_META(MRES_SUPERCEDE);
		}else if ( strcmp(say_text, "/menu") == 0
			|| strcmp(say_text, "menu") == 0 )
		{
			player_data[ID].showMenuOLD();
			
#ifdef _DEBUG
			UTIL_LogDebug("CCem\n");
#endif
			
			RETURN_META(MRES_SUPERCEDE);
		}else if ( strcmp(say_text, "/xmenu") == 0
			|| strcmp(say_text, "xmenu") == 0 )
		{
			player_data[ID].showMenu();
			
#ifdef _DEBUG
			UTIL_LogDebug("CCexm\n");
#endif
			
			RETURN_META(MRES_SUPERCEDE);
		}
	}else if ( strcmp(szUserCommand, "menuselect") == 0 )
	{
		int Key = atoi(CMD_ARGV(1));		// string (arg) to int
		player_data[ID].MenuSelection(Key);
		
#ifdef _DEBUG
		UTIL_LogDebug("CCems\n");
#endif
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("CCe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}


void CmdStart( const edict_t *player , const struct usercmd_s *_cmd , unsigned int random_seed )
{
#ifdef _DEBUG
	UTIL_LogDebug("CSs");
#endif
	
	byte ID = ENTINDEX(player);
	if( ID < 1 || ID > gpGlobals->maxClients )
	{
		RETURN_META(MRES_IGNORED);
	}
	
	struct usercmd_s *cmd = (struct usercmd_s *)_cmd;
	
	// if true is returned, this impulse has to be blocked
	if ( player_data[ID].checkImpulse(cmd->impulse) == true )
	{
		cmd->impulse = 0;
		
#ifdef _DEBUG
		UTIL_LogDebug("CSei\n");
#endif
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("CSe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

qboolean ClientConnect_Post( edict_t *pEntity , const char *pszName , const char *pszAddress , char szRejectReason[128] )
{
#ifdef _DEBUG
	UTIL_LogDebug("CCOs");
#endif
	
	byte ID = ENTINDEX(pEntity);
	player_data[ID].init(pEntity);
	
#ifdef _DEBUG
	UTIL_LogDebug("CCoe\n");
#endif
	
	RETURN_META_VALUE(MRES_IGNORED, 0);
}

void ClientPutInServer_Post( edict_t *pEntity )
{
#ifdef _DEBUG
	UTIL_LogDebug("CPISs");
#endif
	player_data[ENTINDEX(pEntity)].put_in_server();
	
#ifdef _DEBUG
	UTIL_LogDebug("CPISe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void ClientDisconnect( edict_t *pEntity )
{
#ifdef _DEBUG
	UTIL_LogDebug("CDs");
#endif
	
	player_data[ENTINDEX(pEntity)].disconnect();
	
#ifdef _DEBUG
	UTIL_LogDebug("CDe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void ClientPreThink( edict_t *pEntity )
{
#ifdef _DEBUG
	UTIL_LogDebug("CPTs");
	char debuglogs[300] = "";
#endif
	
	byte ID = ENTINDEX(pEntity);
	// Team change
	if ( player_data[ID].pTeam  != pEntity->v.team )
	{
		// close menus
		CLIENT_COMMAND(pEntity, "slot10\n");
		
		player_data[ID].pTeam = pEntity->v.team;
	}
	
	byte newClass = UTIL_getClass(pEntity);
	// Class change
	if ( player_data[ID].pClass != newClass )
	{
		byte oldClass = player_data[ID].pClass;
		player_data[ID].pClass = newClass;
		
		// Class has changed so recalculate HP AP settings
		player_data[ID].getMaxHP();
		player_data[ID].getMaxAP();
		
		// close menus
		if ( newClass == CLASS_GESTATE )
		{
			CLIENT_COMMAND(pEntity, "slot10\n");
			player_data[ID].player_gestate_extracheck = 0;
			
			if ( pEntity->v.health > player_data[ID].maxHP )
				pEntity->v.health = player_data[ID].maxHP;
		}
		if ( oldClass == CLASS_GESTATE )
		{
			player_data[ID].player_gestate_extracheck = 0;
			
			if ( player_data[ID].pClass == CLASS_SKULK )
				// || player_data[ID].pClass == CLASS_GORGE )
				player_senseofancients[ID].setWeaponData_Dmg();
		}
	}
	
	// Player Spawn
	if ( player_data[ID].pDeath != pEntity->v.deadflag )
	{
		if ( pEntity->v.deadflag == DEAD_NO )
		{
			player_data[ID].player_gestate_extracheck = 1;
			if ( player_data[ID].died_during_gestate == true )
			{
				player_data[ID].died_during_gestate = false;
				player_data[ID].respawn_time = gpGlobals->time + 0.1;
				
				player_data[ID].gestate_block_Damage_msg = true;
				player_data[ID].killPlayer();
				// correct deaths, due to fake kill
				UTIL_setDeaths(pEntity, UTIL_getDeaths(pEntity) - 1);
			}else
			{
				// player spawned, so update armor
				pEntity->v.armorvalue = player_data[ID].maxAP;
				
				// player spawned and got new weapons ( new entities )
				// so regive ammo and update data
				for ( int upgrade_ID = UP_START; upgrade_ID < UP_END; ++upgrade_ID )
					upgrade_pl_data[upgrade_ID][ID]->respawned();
			}
		}
		
		player_data[ID].pDeath = pEntity->v.deadflag;
	}
	
	if ( player_data[ID].respawn_time > 0.0
		&& gpGlobals->time > player_data[ID].respawn_time )
	{
		player_data[ID].respawn_player();
		player_data[ID].respawn_time = 0.0;
		
		for ( byte j = 0; j < 4; ++j )
		{
			MESSAGE_BEGIN(MSG_ONE, WeapPickup_ID, NULL, pEntity);
			WRITE_BYTE(alien_weapon_num[j]);
			MESSAGE_END();
			
			UTIL_giveItem(pEntity, alien_weapon_list[j]);
		}
	}
	
	if ( player_data[ID].gestation_Emulation != GESTATE_NONE
		&& player_data[ID].gestation_Emulation != GESTATE_RUNNING )
	{
		// while GESTATE_RUNNING only Progress bar is updated
		// Progress bar needs to be set after PreThink or otherwise it will not be shown
		player_data[ID].gestate_emulation();
	}
	
	for ( int upgrade_ID = UP_START; upgrade_ID < UP_END; ++upgrade_ID )
	{
#ifdef _DEBUG
		char msg[10];
		sprintf(msg, "CPT%d", upgrade_ID);
		strcat(debuglogs, msg);
#endif
		upgrade_pl_data[upgrade_ID][ID]->Think();
	}
	
	player_data[ID].fixGestateHpAP();
	
#ifdef _DEBUG
	//UTIL_LogDebug("CPTCORE");
	strcat(debuglogs, "CPTCORE");
#endif
	
	player_data[ID].Core();
	
#ifdef _DEBUG
	strcat(debuglogs, "CPTe\n");
	UTIL_LogDebug(debuglogs);
	//UTIL_LogDebug("CPTe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void ClientPreThink_Post( edict_t *pEntity )
{
#ifdef _DEBUG
	UTIL_LogDebug("CPTPs");
#endif
	
	byte ID = ENTINDEX(pEntity);
	
	if ( player_data[ID].gestation_Emulation == GESTATE_RUNNING )
	{
		// while GESTATE_RUNNING only Progress bar is updated
		// Progress bar needs to be set after PreThink or otherwise it will not be shown
		player_data[ID].gestate_emulation();
	}
	
	player_data[ID].fixGestateHpAP_Post();
	
	for ( int upgrade_ID = UP_START; upgrade_ID < UP_END; ++upgrade_ID )
		upgrade_pl_data[upgrade_ID][ID]->Think_Post();
	
#ifdef _DEBUG
	UTIL_LogDebug("CPTPe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void ServerFrame( void )
{
#ifdef _DEBUG
	if ( UTIL_getLogCount() > 10000 )
		UTIL_ClearLog();
	UTIL_LogDebug("SFs");
#endif
	
	data_senseofancients.SporeEmulationTimer();
	
	for ( byte ID = 1; ID <= gpGlobals->maxClients; ++ID )
	{
		if ( player_data[ID].ingame == false )
			continue;
		
		if ( player_data[ID].got_xp == false )
			continue;
		
		player_data[ID].got_xp = false;
		player_data[ID].give_xtra_EXP(player_data[ID].is_killer_of);
	}
	
	if ( gpGlobals->time > g_next_CVAR_check )
	{
		g_next_CVAR_check = gpGlobals->time + CVAR_CHECK_DELAY;
		if ( (int)CVAR_maxlevel->value != max_level )
		{
			max_level = (int)CVAR_maxlevel->value;
			initCVARS_values();
			pre_calc_level_data();
		}
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("SFe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnMessageBegin( int msg_dest , int msg_type , const float *pOrigin , edict_t *ed )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("MBs");
#endif
	
	if ( msg_type == HUDText2_ID
		|| msg_type == AmmoPickup_ID
		|| msg_type == ScoreInfo_ID
		|| msg_type == HideWeapon_ID
		|| msg_type == Progress_ID
		|| msg_type == WeapPickup_ID
		|| msg_type == DeathMsg_ID
		|| msg_type == CurWeapon_ID
		|| msg_type == Particles_ID
		|| msg_type == Damage_ID )
	{
		Msg_type = msg_type;
		Msg_receiver = ENTINDEX(ed);
		Msg_arg_num = 1;
		Msg_stored_data = 0;
		Msg_stored_data2 = 0;
		Msg_correct_data = false;
		
		if ( msg_type == DeathMsg_ID
			|| msg_type == Damage_ID )
		{
			if ( gBlockMsgPlayer )
				gBlockMsg = true;
			else if ( msg_type == Damage_ID
				&& player_data[Msg_receiver].gestate_block_Damage_msg )
			{
				gBlockMsg = true;
				player_data[Msg_receiver].gestate_block_Damage_msg = false;
			}
		}else if ( msg_type == AmmoPickup_ID
			&& player_advancedammopack[Msg_receiver].cur_level )
		{
			gBlockMsg = true;
		}
	}
	
	if ( gBlockMsg )
	{
#ifdef _DEBUG
		UTIL_LogDebug("MBeb\n");
#endif
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("MBe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnWriteByte( int value )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("WBs");
#endif
	
	if ( Msg_type == CurWeapon_ID )
	{
		EVENT_CurWeapon(Msg_receiver, value, Msg_arg_num, Msg_correct_data);
	}else if ( Msg_type == ScoreInfo_ID )
	{
		if ( Msg_arg_num == 1 )
			Msg_receiver = value;
		
		player_data[Msg_receiver].scoreinfo_data[Msg_arg_num] = value;
	}else if ( Msg_type == Damage_ID )
	{
		EVENT_DamageMsg(Msg_receiver, Msg_arg_num, value);
	}else if ( Msg_type == DeathMsg_ID )
	{
		EVENT_DeathMsg(Msg_receiver, Msg_arg_num, Msg_stored_data, value);
	}else if ( Msg_type == AmmoPickup_ID )
	{
		if ( Msg_arg_num == 1 )
			Msg_stored_data = value;
	}
	
	++Msg_arg_num;
	
	if ( gBlockMsg )
	{
#ifdef _DEBUG
		UTIL_LogDebug("WBeb\n");
#endif
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("WBe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnWriteLong( int value )
{
#ifdef _DEBUG
	UTIL_LogDebug("WLs");
#endif
	
	++Msg_arg_num;
	
	if ( gBlockMsg )
	{
#ifdef _DEBUG
		UTIL_LogDebug("WLeb\n");
#endif
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("WLe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnWriteShort( int value )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("WSHs");
#endif
	
	if ( Msg_type == ScoreInfo_ID )
	{
		player_data[Msg_receiver].scoreinfo_data[Msg_arg_num] = value;
		
		EVENT_ScoreInfo(Msg_arg_num, Msg_receiver);
	}
	
	++Msg_arg_num;
	
	if ( gBlockMsg )
	{
#ifdef _DEBUG
		UTIL_LogDebug("WSHeb\n");
#endif
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("WSHe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnWriteCoord( float fvalue )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("WCs");
#endif
	
	++Msg_arg_num;
	
	if ( gBlockMsg )
	{
#ifdef _DEBUG
		UTIL_LogDebug("WCeb\n");
#endif
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("WCe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnWriteString( const char *string )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("WSTs");
#endif
	
	if ( Msg_type == HUDText2_ID )
	{
		Msg_correct_data = EVENT_HUDText2(Msg_receiver, string);
	}else if ( Msg_type == ScoreInfo_ID )
	{
		player_data[Msg_receiver].scoreinfo_string = (char *)string;
	}else if ( Msg_type == Particles_ID )
	{
		if ( Msg_arg_num == 2 )
			EVENT_Particles(string);
	}else if ( Msg_type == DeathMsg_ID )
	{
		strncpy(Msg_stored_string, string, 31);
	}
	
	++Msg_arg_num;
	
	if ( gBlockMsg )
	{
#ifdef _DEBUG
		UTIL_LogDebug("WSTeb\n");
#endif
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("WSTe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnMessageEnd( void )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("MEs");
#endif
	
	if ( Msg_type == DeathMsg_ID )
	{
		EVENT_DeathMsg_END(Msg_receiver, Msg_stored_data, Msg_stored_string);
	}else if ( Msg_type == AmmoPickup_ID )
	{
		// message is about to be blocked
		// so value must be changed
		if ( gBlockMsg )
		{
			player_advancedammopack[Msg_receiver].setAmmoPickupMsg(Msg_stored_data);
		}
	}
	
	Msg_type = 0;
	Msg_receiver = 0;
	Msg_stored_string[0] = 0;
	
	if ( gBlockMsg )
	{
#ifdef _DEBUG
		UTIL_LogDebug("MEeb\n");
#endif
		
		gBlockMsg = false;
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("MEe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnPlaybackEvent( int flags , const edict_t *pInvoker , unsigned short eventindex , float delay , float *origin , float *angles , float fparam1 , float fparam2 , int iparam1 , int iparam2 , int bparam1 , int bparam2 )
{
#ifdef _DEBUG
	UTIL_LogDebug("PEs");
#endif
	
	if ( eventindex == Spore_Event_ID )
	{
		byte ID = ENTINDEX(pInvoker->v.owner);
		if ( player_data[ID].ingame )
			player_senseofancients[ID].add_Spore(pInvoker);
	}else if ( eventindex == Cloak_Event_ID )
	{
		byte ID = ENTINDEX(pInvoker);
		if ( player_data[ID].ingame )
			player_senseofancients[ID].Player_Redeemed(origin);
	}else if ( eventindex == HealingSpray_Event_ID )
	{
		byte ID = ENTINDEX(pInvoker);
		if ( player_data[ID].ingame )
			player_senseofancients[ID].check_HealingSpray();
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("PEe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnAlertMessage( ALERT_TYPE atype , char *szFmt , ... )
{
	if ( atype != at_logged )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("AMs");
#endif
	
	if ( gBlockLog )
	{	// block dual kill by gestate emulation
		
#ifdef _DEBUG
		UTIL_LogDebug("AMeb\n");
#endif
		
		gBlockLog = false;
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
	// get the log message
	char buffer[2048];
	va_list argptr;
	va_start(argptr, szFmt);
	vsprintf(buffer, szFmt, argptr);
	va_end(argptr);
	
	char *c = strstr(buffer, "changed role");
	if ( c == NULL )
		RETURN_META(MRES_IGNORED);	// log does not contain what we are looking for
	
	char *buffer_start = (char *)buffer;
	++buffer_start;		// first char of buffer is   "   so skip it
	
	int counter = 0;
	while ( c > buffer_start )
	{
		--c;
		if ( *c != '<' )
			continue;
		
		++counter;
		if ( counter == 3 )
			break;
	}
	
	int name_len = ( c - buffer_start ) - 1;
	byte ID = 0;
	for ( byte targetID = 1; targetID <= gpGlobals->maxClients; ++targetID )
	{
		if ( player_data[targetID].ingame == false )
			continue;
		
		if ( strncmp(player_data[targetID].name, buffer_start, name_len) != 0 )	// if 0 then equal
			continue;
		
		ID = targetID;
		
		break;
	}
	
	if ( ID == 0 )
	{
#ifdef _DEBUG
		UTIL_LogDebug("AMei\n");
#endif
		
		RETURN_META(MRES_IGNORED);	// player not found
	}
	
	if ( player_data[ID].gestation_Emulation == GESTATE_RUNNING )
	{
		// allow one log message, but prevent log spam
		if ( player_data[ID].gestating_emu_msg == false )
			player_data[ID].gestating_emu_msg = true;
		else
		{
#ifdef _DEBUG
			UTIL_LogDebug("AMee\n");
#endif
			
			RETURN_META(MRES_SUPERCEDE);
		}
	}else if ( player_data[ID].died_during_gestate == true )
	{
		if ( player_data[ID].died_during_gestate_change_Msg == false )
		{
			player_data[ID].died_during_gestate_change_Msg = true;
			
#ifdef _DEBUG
			UTIL_LogDebug("AMed\n");
#endif
			
			RETURN_META(MRES_SUPERCEDE);
		}
		
		player_data[ID].died_during_gestate_change_Msg = false;
		
		counter = 0;
		c = buffer_start;
		for ( int i = 0; i < (int)strlen(buffer); ++i )
			++c;
		
		while ( c > buffer_start )
		{
			--c;
			if ( *c != '"' )
				continue;
			
			++counter;
			if ( counter == 2 )
				break;
		}
		
		strcpy(buffer + (c - buffer_start) + 1, "\"skulk\"\0");
		
		// block old alert and send a new fixed one ( change role to skulk, NOT gorge )
		ALERT(at_logged, "%s\n", buffer);
		
#ifdef _DEBUG
		UTIL_LogDebug("AMer\n");
#endif
		
		RETURN_META(MRES_SUPERCEDE);
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("AMe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnMessageBegin_Post( int msg_dest , int msg_type , const float *pOrigin , edict_t *ed )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("MBPs");
#endif
	
	if ( msg_type == SetTech_ID
		|| msg_type == ScoreInfo_ID
		|| msg_type == StatusValue_ID
		|| msg_type == TeamInfo_ID
		|| msg_type == HUDText2_ID )
	{
		Msg_type_Post = msg_type;
		Msg_receiver_Post = ENTINDEX(ed);
		Msg_arg_num_Post = 1;
		Msg_stored_data_Post = 0;
		Msg_correct_data_Post = false;
	}
	
#ifdef _DEBUG
	UTIL_LogDebug("MBPe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnWriteByte_Post( int value )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("WBPs");
#endif
	
	if ( Msg_type_Post == SetTech_ID )
	{
		EVENT_SetTech_POST(Msg_receiver_Post, value, Msg_arg_num_Post, Msg_stored_data_Post);
	}else if ( Msg_type_Post == TeamInfo_ID )
	{
		Msg_receiver_Post = value;		// we need to set it here cause in MessageBegin ID is = 0
	}else if ( Msg_type_Post == StatusValue_ID )
	{
		Msg_correct_data_Post = EVENT_StatusValue_Byte_POST(value);
	}else if ( Msg_type_Post == ScoreInfo_ID )
	{
		if ( Msg_arg_num_Post == 1 )
			Msg_receiver_Post = value;
		
		player_data[Msg_receiver_Post].scoreinfo_data[Msg_arg_num_Post] = value;
	}
	
	++Msg_arg_num_Post;
	
#ifdef _DEBUG
	UTIL_LogDebug("WBPe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnWriteLong_Post( int value )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
	++Msg_arg_num_Post;
	
	RETURN_META(MRES_IGNORED);
}

void pfnWriteShort_Post( int value )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("WSHPs");
#endif
	
	if ( Msg_type_Post == StatusValue_ID )
	{
		if ( Msg_correct_data_Post )
			EVENT_StatusValue_Short_POST(Msg_receiver_Post, value);
	}else if ( Msg_type_Post == ScoreInfo_ID )
	{
		player_data[Msg_receiver_Post].scoreinfo_data[Msg_arg_num_Post] = value;
		
		EVENT_ScoreInfo(Msg_receiver_Post, Msg_arg_num_Post);
	}
	
	++Msg_arg_num_Post;
	
#ifdef _DEBUG
	UTIL_LogDebug("WSHPe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnWriteString_Post( const char *string )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("WSTPs");
#endif
	
	if ( Msg_type_Post == HUDText2_ID )
	{
		Msg_correct_data_Post = EVENT_HUDText2_POST(string);
	}else if ( Msg_type_Post == TeamInfo_ID )
	{
		Msg_correct_data_Post = EVENT_TeamInfo_POST(Msg_receiver_Post, string);
	}
	
	++Msg_arg_num_Post;
	
#ifdef _DEBUG
	UTIL_LogDebug("WSTPe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void pfnWriteCoord_Post( float flValue )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
	++Msg_arg_num_Post;
	
	RETURN_META(MRES_IGNORED);
}

void pfnMessageEnd_Post( void )
{
	if ( gIgnore_Self_Send_Msg )
		RETURN_META(MRES_IGNORED);
	
#ifdef _DEBUG
	UTIL_LogDebug("MEPs");
#endif
	
	if ( Msg_type_Post == HUDText2_ID )
	{
		if ( Msg_correct_data_Post )
			EVENT_HUDText2_END_POST(Msg_receiver_Post);
	}else if ( Msg_type_Post == TeamInfo_ID )
	{
		if ( Msg_correct_data_Post )
			EVENT_TeamInfo_END_POST(Msg_receiver_Post);
	}else if ( Msg_type_Post == StatusValue_ID )
	{
		EVENT_StatusValue_END_POST(Msg_receiver_Post);
	}else if ( Msg_type_Post == ScoreInfo_ID )
	{
		UTIL_sendScoreInfo(Msg_receiver_Post);
	}
	
	Msg_type_Post = 0;
	Msg_receiver_Post = 0;
	
#ifdef _DEBUG
	UTIL_LogDebug("MEPe\n");
#endif
	
	RETURN_META(MRES_IGNORED);
}

void Cleanup_Init( )
{
	g_pFunctionTable->pfnSpawn = NULL;
}

void Set_Hooks( )
{
	
	g_pFunctionTable_Post->pfnServerActivate = ServerActivate_Post;
	//g_pFunctionTable_Post->pfnServerDeactivate = ServerDeactivate_Post;		// should not have been cleaned
	
	g_pFunctionTable_Post->pfnClientConnect = ClientConnect_Post;
	g_pFunctionTable->pfnClientDisconnect = ClientDisconnect;
	g_pFunctionTable_Post->pfnClientPutInServer = ClientPutInServer_Post;
	g_pFunctionTable_Post->pfnClientUserInfoChanged = ClientUserInfoChanged_Post;
	
	Set_Hooks_basic();
}

void Set_Hooks_basic( )
{
	g_pFunctionTable->pfnClientCommand = ClientCommand;
	g_pFunctionTable->pfnPlayerPreThink = ClientPreThink;
	g_pFunctionTable_Post->pfnPlayerPreThink = ClientPreThink_Post;
	g_pFunctionTable->pfnCmdStart = CmdStart;
	
	g_pFunctionTable->pfnKeyValue = KeyValue;
	g_pengfuncsTable->pfnAlertMessage = pfnAlertMessage;
	g_pengfuncsTable->pfnPlaybackEvent = pfnPlaybackEvent;
	g_pFunctionTable->pfnStartFrame = ServerFrame;
	g_pFunctionTable->pfnSpawn = Spawn;
	
	g_pengfuncsTable->pfnMessageBegin = pfnMessageBegin;
	Set_Hooks_Message();
		
	g_pengfuncsTable_Post->pfnMessageBegin = pfnMessageBegin_Post;
	Set_Hooks_Message_Post();
}

void Cleanup_Hooks( )
{
	g_pFunctionTable_Post->pfnServerActivate = NULL;
	//g_pFunctionTable_Post->pfnServerDeactivate = NULL;	// is needed to set hooks again
	
	g_pFunctionTable_Post->pfnClientConnect = NULL;
	g_pFunctionTable->pfnClientDisconnect = NULL;
	g_pFunctionTable_Post->pfnClientPutInServer = NULL;
	g_pFunctionTable_Post->pfnClientUserInfoChanged = NULL;
	
	Cleanup_Hooks_basic();
}

void Cleanup_Hooks_basic( )
{
	g_pFunctionTable->pfnClientCommand = NULL;
	g_pFunctionTable->pfnPlayerPreThink = NULL;
	g_pFunctionTable_Post->pfnPlayerPreThink = NULL;
	g_pFunctionTable->pfnCmdStart = NULL;
	
	g_pFunctionTable->pfnKeyValue = NULL;
	g_pengfuncsTable->pfnAlertMessage = NULL;
	g_pengfuncsTable->pfnPlaybackEvent = NULL;
	g_pFunctionTable->pfnStartFrame = NULL;
	g_pFunctionTable->pfnSpawn = NULL;
	
	g_pengfuncsTable->pfnMessageBegin = NULL;
	Clear_Hooks_Message();
		
	g_pengfuncsTable_Post->pfnMessageBegin = NULL;
	Clear_Hooks_Message_Post();
}

void Set_Hooks_Message( )
{
	g_pengfuncsTable->pfnWriteByte = pfnWriteByte;
	g_pengfuncsTable->pfnWriteCoord = pfnWriteCoord;
	g_pengfuncsTable->pfnWriteLong = pfnWriteLong;
	g_pengfuncsTable->pfnWriteShort = pfnWriteShort;
	g_pengfuncsTable->pfnWriteString = pfnWriteString;
	g_pengfuncsTable->pfnMessageEnd = pfnMessageEnd;
}

void Set_Hooks_Message_Post( )
{
	g_pengfuncsTable_Post->pfnWriteByte = pfnWriteByte_Post;
	g_pengfuncsTable_Post->pfnWriteCoord = pfnWriteCoord_Post;
	g_pengfuncsTable_Post->pfnWriteLong = pfnWriteLong_Post;
	g_pengfuncsTable_Post->pfnWriteShort = pfnWriteShort_Post;
	g_pengfuncsTable_Post->pfnWriteString = pfnWriteString_Post;
	g_pengfuncsTable_Post->pfnMessageEnd = pfnMessageEnd_Post;
}

void Clear_Hooks_Message( )
{
	g_pengfuncsTable->pfnWriteByte = NULL;
	g_pengfuncsTable->pfnWriteCoord = NULL;
	g_pengfuncsTable->pfnWriteLong = NULL;
	g_pengfuncsTable->pfnWriteShort = NULL;
	g_pengfuncsTable->pfnWriteString = NULL;
	g_pengfuncsTable->pfnMessageEnd = NULL;
}

void Clear_Hooks_Message_Post( )
{
	g_pengfuncsTable_Post->pfnWriteByte = NULL;
	g_pengfuncsTable_Post->pfnWriteCoord = NULL;
	g_pengfuncsTable_Post->pfnWriteLong = NULL;
	g_pengfuncsTable_Post->pfnWriteShort = NULL;
	g_pengfuncsTable_Post->pfnWriteString = NULL;
	g_pengfuncsTable_Post->pfnMessageEnd = NULL;
}

