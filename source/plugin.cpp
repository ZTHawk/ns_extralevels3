//This file contains utility functions that don't serve any specific purpose but are invaluable nevertheless

#include "plugin.h"
#include "utilfunctions.h"
#include <sdk_util.h>	//pre-defined function definitions saves us a lot of work

cvar_t *CVAR_notify;
cvar_t *CVAR_instruct;
cvar_t *CVAR_huddisplay;
cvar_t *CVAR_maxlevel;
cvar_t *CVAR_upgrade_levels[CVAR_LEVELNAMES_NUM];

edict_t *Hive_ID = NULL;
edict_t *Hive_ID2 = NULL;
bool isMvA = true;
bool isMvM = false;
bool isAvA = false;

bool Custom_Levels = false;
float Base_XP_Custom = 0;
float Base_XP_Custom_modifier = 0;
int *Base_XP_at_Level = NULL;
int *Next_Level_XP_modifier = NULL;
int max_level = 0;

char *config_file = "ns/addons/extralevels3/configs/extralevels3.cfg";
char *ban_file = "ns/addons/extralevels3/configs/el3_ban.cfg";

hl_string_base hl_strings;

//=================================================================
//the grand old initialization function
//=================================================================
void plugin_init( )
{
	UTIL_LogPrintf( "[" PLUGIN_NAME "] ================LOADING===============\n");
	//UTIL_LogPrintf( "[" PLUGIN_NAME "] By " PLUGIN_AUTHOR " (" PLUGIN_EMAIL ")\n");
	UTIL_LogPrintf( "[" PLUGIN_NAME "] By " PLUGIN_AUTHOR " - Version: " PLUGIN_VERSION " - Date: " PLUGIN_DATE " \n");
	//UTIL_LogPrintf( "[" PLUGIN_NAME "] ================LOADING===============\n");
	
	// Register Cvars
	initCVARS();
}

//=================================================================
//the grand old quit function
//=================================================================
void plugin_quit( )
{
	UTIL_LogPrintf( "[" PLUGIN_NAME "] ===============UNLOADING==============\n");
	//UTIL_LogPrintf( "[" PLUGIN_NAME "] By " PLUGIN_AUTHOR " (" PLUGIN_EMAIL ")\n");
	UTIL_LogPrintf( "[" PLUGIN_NAME "] By " PLUGIN_AUTHOR " - Version: " PLUGIN_VERSION " - Date: " PLUGIN_DATE " \n");
	//UTIL_LogPrintf( "[" PLUGIN_NAME "] ===============UNLOADING==============\n");
	
	for ( int i = 0; i < CVAR_LEVELNAMES_NUM; ++i )
		delete CVAR_upgrade_levels[i]->string;
	delete config_file;
	delete ban_file;
	
	for ( unsigned int i = 0; i < banList.size(); ++i )
		delete(banList[i]);
	banList.clear();
}

void initCVARS( )
{
	static cvar_t el3_notifyme_cvar = {EL3_CVARNAME_NOTIFYME, EL3_CVAR_NOTIFYME_VALUE, 0, atof(EL3_CVAR_NOTIFYME_VALUE), NULL};		// Set to the number of times you want the "ExtraLevels 3" message to be displayed on spawn.
	CVAR_REGISTER(&el3_notifyme_cvar);
	CVAR_notify = CVAR_GET_POINTER(EL3_CVARNAME_NOTIFYME);
	
	static cvar_t el3_instruct_cvar = {EL3_CVARNAME_INSTRUCT, EL3_CVAR_INSTRUCT_VALUE, 0, atof(EL3_CVAR_INSTRUCT_VALUE), NULL};		// Set to the number of times you want the "type /xhelp for more info" message to be displayed on spawn.	
	CVAR_REGISTER(&el3_instruct_cvar);
	CVAR_instruct = CVAR_GET_POINTER(EL3_CVARNAME_INSTRUCT);
	
	static cvar_t el3_huddisplay_cvar = {EL3_CVARNAME_HUDDISPLAY, EL3_CVAR_HUDDISPLAY_VALUE, 0, atof(EL3_CVAR_HUDDISPLAY_VALUE), NULL};	// Set to 1 to get the normal levels display, set to 2 to get the alternate levels display. If you change from "2" to "1" in the same round, you may expirience some problems with the hud levels display for the round!
	CVAR_REGISTER(&el3_huddisplay_cvar);
	CVAR_huddisplay = CVAR_GET_POINTER(EL3_CVARNAME_HUDDISPLAY);
	
	static cvar_t el3_maxlevel_cvar = {EL3_CVARNAME_MAXLEVEL, EL3_CVAR_MAXLEVEL_VALUE, 0, atof(EL3_CVAR_MAXLEVEL_VALUE), NULL};
	CVAR_REGISTER(&el3_maxlevel_cvar);
	CVAR_maxlevel = CVAR_GET_POINTER(EL3_CVARNAME_MAXLEVEL);
	
	//Automatically register a cvar that makes the plugin version publically available
	static cvar_t plugin_version_register = { PLUGIN_CVAR, PLUGIN_VERSION, FCVAR_SERVER | FCVAR_SPONLY | FCVAR_PRINTABLEONLY, 0, NULL };
	CVAR_REGISTER(&plugin_version_register);
	CVAR_SET_STRING(PLUGIN_CVAR, PLUGIN_VERSION);
	
	for ( int i = 0; i < CVAR_LEVELNAMES_NUM; ++i )
	{
		cvar_t* level_list_cvar = new cvar_t;
		level_list_cvar->name = (char *)level_cvar_list[(CVAR_LEVELNAMES_NUM - 1) - i];
		level_list_cvar->string = "0";
		level_list_cvar->flags = 0;
		level_list_cvar->value = 0.0;
		
		static cvar_t tmp;
		tmp = *(level_list_cvar);
		CVAR_REGISTER(&tmp);
		CVAR_upgrade_levels[(CVAR_LEVELNAMES_NUM - 1) - i] = CVAR_GET_POINTER(level_cvar_list[(CVAR_LEVELNAMES_NUM - 1) - i]);
	}
	
	initCVARS_values();
}

void initCVARS_values( )
{
	float level_step = ( CVAR_maxlevel->value - (float)BASE_MAX_LEVEL ) / ( CVAR_LEVELNAMES_NUM + 1 );
	int level_increaser = (int)floor(level_step);
	float levels_remaining = 0;
	int level_difference;
	int level_to_set = 10;
	char str_num[12];
	for ( int i = 0; i < CVAR_LEVELNAMES_NUM; ++i )
	{
		level_to_set += level_increaser;
		levels_remaining += level_step;
		level_difference = (int)floor(levels_remaining - (float)level_increaser);
		if ( level_difference >= 1 )
		{
			level_to_set += level_difference;
			levels_remaining -= (float)level_difference;
		}else
			levels_remaining -= (float)level_increaser;
		
		sprintf(str_num, "%d", level_to_set);
		
		delete CVAR_upgrade_levels[(CVAR_LEVELNAMES_NUM - 1) - i]->string;
		CVAR_upgrade_levels[(CVAR_LEVELNAMES_NUM - 1) - i]->string = new char[strlen(str_num)];
		strcpy(CVAR_upgrade_levels[(CVAR_LEVELNAMES_NUM - 1) - i]->string, str_num);
		CVAR_upgrade_levels[(CVAR_LEVELNAMES_NUM - 1) - i]->value = (float)level_to_set;
	}
}

void precacheSounds( )
{
	for ( int i = 0; i < MAX_SOUNDS; ++i )
		PRECACHE_SOUND((char *)sound_files[i]);
}

void init_xplevel_data( )
{
	Config_data customelevels_data[] =
	{
		{&(Custom_Levels = CUSTOM_LEVELS), TYPE_BOOL, (char *)Custom_config_names[0]},
		{&(Base_XP_Custom = BASE_XP_TO_NEXT_LEVEL), TYPE_FLOAT, (char *)Custom_config_names[1]},
		{&(Base_XP_Custom_modifier = NEXT_LEVEL_XP_MODIFIER), TYPE_FLOAT, (char *)Custom_config_names[2]},
	};
	
	UTIL_getUpgradeDataFromFile(customelevels_data, ARRAYSIZE(customelevels_data));
	
	pre_calc_level_data();
}

void pre_calc_level_data( )
{
	max_level = (int)CVAR_maxlevel->value;
	
	delete[] Base_XP_at_Level;
	delete[] Next_Level_XP_modifier;
	
	Base_XP_at_Level = new int [max_level + 1];
	Next_Level_XP_modifier = new int [max_level + 1];
	for ( int i = 0; i <= max_level; ++i )
	{
		if ( i == 0 )
		{
			// level ZERO does not exist, so use this part to store max possible array length
			Base_XP_at_Level[i] = max_level;
			Next_Level_XP_modifier[i] = 0;
		}else if ( i == 1 )
		{
			Base_XP_at_Level[i] = 0;		// even with 0 XP you are level 1
			// all EXP are calculated with +1, so do a fix for first level
			Next_Level_XP_modifier[i] = 101;	// default value
		}else if ( i < 10 || Custom_Levels == false )
		{
			// no Custom levels shall be used, so use default system
			Base_XP_at_Level[i] = ( i + 2 ) * ( i - 1) * 25 + 1;
			Next_Level_XP_modifier[i] = ( i + 1 )* 50;
		}else if ( i == 10 )
		{
			Base_XP_at_Level[i] = ( i + 2 ) * ( i - 1) * 25 + 1;
			Next_Level_XP_modifier[i] = (int)(Base_XP_Custom + Base_XP_Custom_modifier);
		}else
		{
			Base_XP_at_Level[i] = 2701
						+ ( (int)( (float)(2 * ( i - 10 )) * Base_XP_Custom
							+ Base_XP_Custom_modifier * (float)( ( i - 10 ) * ( i - 10 ) + ( i - 10 ) ) ) )
						/ 2;
			Next_Level_XP_modifier[i] = (int)(Base_XP_Custom + (float)( i - 9 ) * Base_XP_Custom_modifier);
		}
	}
}

