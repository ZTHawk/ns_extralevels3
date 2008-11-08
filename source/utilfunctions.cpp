//this file exists so that you can paste functions that you need from util.cpp in the actual SDK
//as well as your new custom helper functions you declared in utilfunctions.h

//REMEMBER: to keep your DLL size down, remove functions you don't need from this file


#include "plugin.h"
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
/*#include <sdk_util.h>	//pre-defined function definitions saves us a lot of work
#include <cbase.h>

//decided to just fukkit and add all the headers that show up in the hlsdk util.cpp
#include <gamerules.h>
#include <saverestore.h>
#include <time.h>
#include <shake.h>
#include <decals.h>
#include <player.h>
//#include <weapons.h>
*/
#include "string.h"

vector<char *> banList;

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf( char *fmt , ... )
{
	va_list argptr;
	static char string[1024];
	
	va_start(argptr, fmt);
	vsnprintf(string, sizeof(string), fmt, argptr);
	va_end(argptr);

	// Print to server console
	ALERT(at_logged, "%s", string);
}

// same as above but to server console
void UTIL_ServerPrint( const char *fmt , ... )
{
	va_list argptr;
	static char pszMessage[1024];
	
	// concatenate all the arguments in one string
	va_start(argptr, fmt);
	vsnprintf(pszMessage, sizeof(pszMessage), fmt, argptr);
	va_end(argptr);
	
	SERVER_PRINT(pszMessage);
}

#ifdef _DEBUG
bool debug_running = true;
int log_counter = 0;
const static char *debug_file = "ns/addons/extralevels3/extralevels3_debug.txt";
void UTIL_LogDebug( const char *text )
{
	if ( debug_running == false )
		return;
	
	FILE *file = fopen(debug_file, "a");
	if ( file == NULL )
		return;
	
	fprintf(file, "%s", text);
	fclose(file);
	
	++log_counter;
}

void UTIL_ClearLog( )
{
	if ( remove(debug_file) != 0 )
		UTIL_LogDebug("Error deleting Logfile");
	
	log_counter = 0;
}

int UTIL_getLogCount( )
{
	return log_counter;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////// PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER ////////////
////////// PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER ////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

int UTIL_getWeaponReserve( edict_t *pEntity , byte WeaponID )
{
	switch ( WeaponID )
	{
		case WEAPON_PISTOL:
			return get_private(pEntity, MAKE_OFFSET(AMMO_PISTOL));
		case WEAPON_LMG:
			return get_private(pEntity, MAKE_OFFSET(AMMO_LMG));
		case WEAPON_SHOTGUN:
			return get_private(pEntity, MAKE_OFFSET(AMMO_SHOTGUN));
		case WEAPON_HMG:
			return get_private(pEntity, MAKE_OFFSET(AMMO_HMG));
		case WEAPON_GRENADE_GUN:
			return get_private(pEntity, MAKE_OFFSET(AMMO_GL));
		case WEAPON_GRENADE:
			return get_private(pEntity, MAKE_OFFSET(AMMO_HG));
		default:
			return 0;
	}
	
	return 0;
}

void UTIL_setWeaponReserve( edict_t *pEntity , byte WeaponID , int ReserveSize )
{
	switch ( WeaponID )
	{
		case WEAPON_PISTOL:
		{
			set_private(pEntity, MAKE_OFFSET(AMMO_PISTOL), ReserveSize);
			break;
		}
		case WEAPON_LMG:
		{
			set_private(pEntity, MAKE_OFFSET(AMMO_LMG), ReserveSize);
			break;
		}
		case WEAPON_SHOTGUN:
		{
			set_private(pEntity, MAKE_OFFSET(AMMO_SHOTGUN), ReserveSize);
			break;
		}
		case WEAPON_HMG:
		{
			set_private(pEntity, MAKE_OFFSET(AMMO_HMG), ReserveSize);
			break;
		}
		case WEAPON_GRENADE_GUN:
		{
			set_private(pEntity, MAKE_OFFSET(AMMO_GL), ReserveSize);
			break;
		}
		case WEAPON_GRENADE:
		{
			set_private(pEntity, MAKE_OFFSET(AMMO_HG), ReserveSize);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////// FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE /////////
////////// FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE FILE /////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void UTIL_getUpgradeDataFromFile( Config_data upgrade_data[] , int array_size )
{
	FILE *file = fopen(config_file, "r");
	if ( file == NULL )
		return;
	
	char buffer[128];
	int config_num = 0;
	while ( fgets(buffer, 127, file) != NULL )
	{
		if ( buffer[0] == '\n'					// empty line
			|| ( buffer[0] == '/' && buffer[1] == '/' ) )	// comment
			continue;
		
		char *c = strstr(buffer, upgrade_data[config_num].config_name);
		if ( c == NULL )
			continue;
		
		char *buffer_start = (char *)buffer;
		int found_at_pos = ( c - buffer_start ) + strlen(upgrade_data[config_num].config_name);
		
		const char *dummy = &buffer[found_at_pos];
		switch ( upgrade_data[config_num].data_type )
		{
			case TYPE_INT:
			{
				*((int *)upgrade_data[config_num].address) = atoi(dummy);
				break;
			}
			case TYPE_FLOAT:
			{
				*((float *)upgrade_data[config_num].address) = atof(dummy);
				break;
			}
			case TYPE_BOOL:
			{
				*((bool *)upgrade_data[config_num].address) = ( atoi(dummy) > 0 ) ? true : false;
				break;
			}
			case TYPE_BYTE:
			{
				*((byte *)upgrade_data[config_num].address) = atoi(dummy);
			}
		}
		
		++config_num;
		if ( config_num == array_size )
			break;
	}
	fclose(file);
}

void UTIL_getBanData( )
{
	FILE *file = fopen(ban_file, "r");
	
	if ( file == NULL )
		return;
	
	char buffer[64];
	int ban_num = 0;
	while ( fgets(buffer, 63, file) != NULL )
	{
		if ( (buffer[0] == '\n')					// empty line
			|| ( (buffer[0] == '/') && (buffer[1] == '/') ) )	// comment
			continue;
		
		int pos = strspn(buffer, "STEAM_:0123456789");
		
		if ( pos < 7 )		// make sure at least containing "STEAM_"
			continue;
		
		char *tmp = new char[64];
		strncpy(tmp, buffer, pos);
		tmp[pos] = '\0';
		banList.push_back(tmp);
		
		++ban_num;
	}
	fclose(file);
}

void UTIL_getConfigFilenames( )
{
	char *global_path = (char*)GET_PLUGIN_PATH(PLID);
	char *addons_path = strstr(global_path, "addons");
	
	char cfg_path_base[128];
	memset(cfg_path_base, 0, 128);
	char *last_slash_pos = strrchr(global_path, '/');
	
	strncpy(cfg_path_base, addons_path, last_slash_pos - addons_path);
	
	// check if dll has special folder
	char *c = strstr(cfg_path_base, "dll");
	if ( c != NULL )
		cfg_path_base[c - cfg_path_base - 1] = '\0';
	
	FILE *file_cfg = fopen(config_file, "r");
	if ( file_cfg == NULL )
	{
		char cfg_file_main[128];
		sprintf(cfg_file_main, "ns/%s/configs/extralevels3.cfg", cfg_path_base);
		
		char *temp = new char[strlen(cfg_file_main) + 1];
		strcpy(temp, cfg_file_main);
		
		delete config_file;
		config_file = temp;
		config_file[strlen(cfg_file_main)] = 0;
	}
	
	FILE *file_ban = fopen(ban_file, "r");
	if ( file_ban == NULL )
	{
		char cfg_file_bandata[128];
		sprintf(cfg_file_bandata, "ns/%s/configs/el3_ban.cfg", cfg_path_base);
		
		char *temp = new char[strlen(cfg_file_bandata) + 1];
		strcpy(temp, cfg_file_bandata);
		
		delete ban_file;
		ban_file = temp;
		ban_file[strlen(cfg_file_bandata)] = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////// MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC /////////
////////// MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC /////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTIL_isCloaked( edict_t *pEntity )
{
	return ( get_private(pEntity, MAKE_OFFSET(CLOAK)) < 0.5
		|| player_etherealshift[ENTINDEX(pEntity)].Shifting == true );
}




unsigned short FixedUnsigned16( float value, float scale )
{
	int output;

	output = (int)(value * scale);
	if ( output < 0 )
		output = 0;
	if ( output > 0xFFFF )
		output = 0xFFFF;

	return (unsigned short)output;
}

short FixedSigned16( float value, float scale )
{
	int output;

	output = (int)(value * scale);

	if ( output > 32767 )
		output = 32767;

	if ( output < -32768 )
		output = -32768;

	return (short)output;
}





