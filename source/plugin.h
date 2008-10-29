#ifndef _PLUGIN_H_
#define _PLUGIN_H_

// disable "XXX was declared deprecated" warnings
#pragma warning(disable:4996)

//#define linux
//#define __linux__

//=================================================================
//Plugin Information
//=================================================================
#define PLUGIN_DATE		"October 01, 2008"
#define PLUGIN_NAME		"ExtraLevels 3 MM"
#define PLUGIN_AUTHOR		"White Panther"
#define PLUGIN_EMAIL		""
#define PLUGIN_URL		""
#define MY_LOGTAG		"EXTRALEVELS_3_MM"
#define PLUGIN_CVAR		"mm_extralevels3"
#define PLUGIN_VERSION		"0.7.8c2"
#define PLUGIN_LOADABLE		PT_CHANGELEVEL
#define PLUGIN_UNLOADABLE	PT_ANYPAUSE


#define PLUGIN_VERS_DWORD	0, 7, 8, 12
#define PLUGIN_COMMENTS		"Enjoy"
#define PLUGIN_DESC		"More than 10 levels + new upgrades"
#define PLUGIN_FILENAME		PLUGIN_LOGTAG ".DLL"
#define PLUGIN_INTERNAL		PLUGIN_LOGTAG
#define PLUGIN_COPYRIGHT	"GNU Public License"

#ifndef VERS_PLUGIN_H		//so that we don't confuse the resource compiler
#include <extdll.h>		//always

#include <string.h>		//to prevent linux errors while processing meta_api.h
#include <meta_api.h>

#include <string>
#include <map>

using std::map;
using std::string;
using std::pair;

//=================================================================
//=================================================================

// uncomment line below to enable debug ( high CPU usage )
// #define _DEBUG

// comment line below if using NS version 3.2.1 or better
#define _PRE_NS_321

#define EL3_CVARNAME_NOTIFYME		"el3_notifyme"
#define EL3_CVAR_NOTIFYME_VALUE		"5"
#define EL3_CVARNAME_INSTRUCT		"el3_instruct"
#define EL3_CVAR_INSTRUCT_VALUE		"5"
#define EL3_CVARNAME_HUDDISPLAY		"el3_huddisplay"
#define EL3_CVAR_HUDDISPLAY_VALUE	"1"
#define EL3_CVARNAME_MAXLEVEL		"el3_maxlevel"
#define EL3_CVAR_MAXLEVEL_VALUE		"50"

#define NO_TEAM				0
#define MARINE				1
#define ALIEN				2
#define HUD_CHANNEL			3

#define BASE_MAX_LEVEL			10
#define EXP_RANGE			522.0
#define BASE_MAX_MARINE_POINTS		17
#define BASE_MAX_ALIEN_POINTS		20

#define IMPULSE_READYROOM		5
#define	IMPULSE_RESUPPLY		31
#define IMPULSE_GRENADE			37
#define	IMPULSE_SCAN			53
#define IMPULSE_SHOTGUN			64
#define IMPULSE_HMG			65
#define IMPULSE_GL			66
#define IMPULSE_FLASLIGHT		100
#define IMPULSE_CARAPACE		101
#define IMPULSE_REGENERATION		102
#define IMPULSE_REDEMPTION		103
#define IMPULSE_CELERITY		107
#define IMPULSE_ADRENALINE		108
#define IMPULSE_SILENCE			109
#define IMPULSE_CLOAKING		110
#define IMPULSE_FOCUS			111
#define IMPULSE_SENSEOFFEAR		112
#define IMPULSE_HIVE3_ABILITY		118
#define IMPULSE_HIVE4_ABILITY		126

#define GESTATE_NONE			0
#define GESTATE_START			1
#define GESTATE_START_POST		2
#define GESTATE_RUNNING			3
#define GESTATE_END			4
#define GESTATE_END_POST		5

#define HIVE_3_WEAPONS			4
#define HIVE_2_WEAPONS			3
#define HIVE_1_WEAPONS			2
#define HIVE_0_WEAPONS			1

#define CVAR_CHECK_DELAY		1.0

#define MENU_KEY_1			(1<<0)
#define MENU_KEY_2			(1<<1)
#define MENU_KEY_3			(1<<2)
#define MENU_KEY_4			(1<<3)
#define MENU_KEY_5			(1<<4)
#define MENU_KEY_6			(1<<5)
#define MENU_KEY_7			(1<<6)
#define MENU_KEY_8			(1<<7)
#define MENU_KEY_9			(1<<8)
#define MENU_KEY_0			(1<<9)

#define TRACEHULL_STUCK			true

#define UPGRADE_NAME_LEN		20
#define UPGRADE_DESCRIPTION_LEN		1024
#define MENU_OPTION_LEN			80
#define MENU_CONTENT_LEN		512
#define POPUP_MSG_LEN			190
#define NOTIFY_MSG_LEN			128
#define HUD_MSG_LEN			256
#define HUD_MSG_EXTRA_LEN		128

#define MARINE_HUD_COLOR_R		0
#define MARINE_HUD_COLOR_G		75
#define MARINE_HUD_COLOR_B		100
#define ALIEN_HUD_COLOR_R		160
#define ALIEN_HUD_COLOR_G		100
#define ALIEN_HUD_COLOR_B		0

#define MAX_PLAYERS			33
#define MAX_PLAYERS_PLUS1		33	// set this to maximum players + 1 so arrays can be accessed via ID

// default config settings
#define CUSTOM_LEVELS			false
#define BASE_XP_TO_NEXT_LEVEL		500.0
#define NEXT_LEVEL_XP_MODIFIER		50.0

#define RESET_INGAME			true

//=================================================================
//helper functions
//=================================================================
void plugin_init( );
void plugin_quit( );

void initCVARS( );
void initCVARS_values( );
void precacheSounds( );
void init_xplevel_data( );
void pre_calc_level_data( );

extern char *config_file;
extern char *ban_file;

extern bool Custom_Levels;
extern float Base_XP_Custom;
extern float Base_XP_Custom_modifier;
extern int *Base_XP_at_Level;
extern int *Next_Level_XP_modifier;

extern int max_level;

static const char *Custom_config_names[] =
{
	"CUSTOM_LEVELS",
	"BASE_XP_TO_NEXT_LEVEL",
	"NEXT_LEVEL_XP_MODIFIER"
};

extern edict_t *Hive_ID;
extern edict_t *Hive_ID2;

extern bool gBlockMsgPlayer;
extern bool gBlockLog;

extern enginefuncs_t *g_pEngTable;
extern enginefuncs_t *g_pengfuncsTable;
extern enginefuncs_t *g_pengfuncsTable_Post;
extern DLL_FUNCTIONS *g_pFunctionTable;
extern DLL_FUNCTIONS *g_pFunctionTable_Post;

extern bool gIgnore_Self_Send_Msg;

#define CVAR_LEVELNAMES_NUM			19
static const char *level_cvar_list[CVAR_LEVELNAMES_NUM] =
{
	"el3_XlevelS", "el3_XlevelR", "el3_XlevelQ", "el3_XlevelP", "el3_XlevelO",
	"el3_XlevelN", "el3_XlevelM", "el3_XlevelL", "el3_XlevelK", "el3_XlevelJ",
	"el3_XlevelI", "el3_XlevelH", "el3_XlevelG", "el3_XlevelF", "el3_XlevelE",
	"el3_XlevelD", "el3_XlevelC", "el3_XlevelB", "el3_XlevelA"
};

extern cvar_t *CVAR_notify;
extern cvar_t *CVAR_instruct;
extern cvar_t *CVAR_huddisplay;
extern cvar_t *CVAR_maxlevel;
extern cvar_t *CVAR_upgrade_levels[19];


enum sounds
{
	sound_AlienLevelUP,
	sound_MarineLevelUP,
	
	MAX_SOUNDS
};

static const char *sound_files[MAX_SOUNDS] =
{
	"misc/a-levelup.wav",	// levelup sound aliens
	"misc/levelup.wav"	// levelup sound marines
};

class hl_string_base
{
	private:
	map<string, int> hl_string_map;
	
	public:
	int find( const char *item )
	{
		int id = 0;
		map<string, int>::iterator iter = hl_string_map.find(item);
		if ( iter == hl_string_map.end() )
		{
			id = ALLOC_STRING(item);
			hl_string_map.insert(pair<string, int>(item, id));
		}else
			id = iter->second;
		
		return id;
	};
	
	void insert( const char *item , int value )
	{
		hl_string_map.insert(pair<string, int>(item, value));
	};
};

extern hl_string_base hl_strings;

#endif // #ifndef VERS_PLUGIN_H
#endif // #ifndef _PLUGIN_H_

