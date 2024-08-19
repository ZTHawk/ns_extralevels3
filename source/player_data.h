#ifndef _PLAYER_DATA_H
#define _PLAYER_DATA_H

#include "plugin.h"
#include <vector>

using namespace std;

class EL_Player
{
public:
	byte ID;
	edict_t* pEntity;

	byte team;				// check players team
	int level;				// check players level
	int extralevels;			// check how many extralevels player already got
	int points_available;			// check how many points a player still has remaining
	float lastxp;				// last saved EXP
	int lastpLevel;				// last saved level
	int xp_to_next_lvl;			// amount of EXP at which player reachs next level
	int base_level_xp;			// amount of EXP at which player reached current level

	bool got_xp;				// check if player got some extra EXP
	byte is_killer_of;			// check whom player has killed
	bool got_resupply;			// check if player has Resupply
	bool got_scan;				// check if player has Scan Area

	int scoreinfo_data[10];			// saved data from ScoreInfo
	char scoreinfo_string[32];

	byte upgrade_choice;			// check which upgrade player has choosen in main menu
	bool message_displaying;		// check if a Level-Message is displayed

	byte statusvalue_pointing_at;		// check which players Status we are viewing right now
	byte statusvalue_correct_level_of;	// check which players level shall be corrected in StatusValue

	byte alien_gestate_points;		// amount of points used for lifeform change ( after respawn player will get this amount back )
	byte curWeapon;				// check current weapon player is holding

	float lastHUD_display;			// last time HUD message was displayed

	int infoshown;				// amount of Information-Messages been shown
	int authorshown;
	bool banned;
	bool isBot;

	byte gestation_Emulation;
	bool gestating_emu_msg;
	float player_gestate_time_emu;
	byte player_gestate_emu_class;
	byte player_gestate_extracheck;
	float player_gestate_hp;
	float player_gestate_ap;
	float player_gestate_hp_max;
	float player_gestate_ap_max;
	float player_gestate_health_lost;
	float player_gestate_armor_lost;
	bool gestate_got_spikes;

	bool gestate_block_Damage_msg;
	bool died_during_gestate;
	bool died_during_gestate_change_Msg;
	float respawn_time;

	float maxHP;
	float maxAP;
	float armor_bonus;

	// Additional data
	bool in_main_menu;
	int in_upgrade_menu;
	bool in_help_menu;

	bool ingame;
	byte pTeam;
	byte pClass;
	byte pDeath;

	float join_time_10;
	char name[33];
	char SteamID[33];

	void init(edict_t* player)
	{
		ID = ENTINDEX(player);
		pEntity = player;

		resetData();
	};
	void reset(bool ingame = false);
	void resetData(bool ingame = false);
	void resetGestate();

	void put_in_server();
	void disconnect();

	void Core();
	void gestate_emulation();
	void gestate_messages(bool hide_weapons, byte scoreboard_class, byte iuser3_class);
	bool check_trace_hull_stuck(Vector origin);
	void unstuck();

	float getMaxHP();
	float getMaxAP();
	void fixGestateHpAP();
	void fixGestateHpAP_Post();
	float check_level_player();
	float calc_lvl_and_xp();
	int get_player_lvl(float XP, int lvl_to_check, int min, int max);
	void killPlayer();
	void newDeahthMsg(byte KillerID, const char* WeaponName);
	void respawn_player();
	int getScoreByClass();
	void givePoints(byte victimID);
	void give_xtra_EXP(byte victimID, byte FakeKiller = 0);

	void showNotifyMsg();
	void showMenu();
	void showMenuOLD();
	void showHelpMenu();
	void MenuSelection(int key);
	void showHUD_Msg(byte vID, float XP, int level, bool is_marine);

	void checkBanList(const char* SteamID);
	bool checkImpulse(int impulse);
	void set_upgrade_level(int level, int upgrade_ID);
};

extern EL_Player player_data[MAX_PLAYERS_PLUS1];

extern vector<char*> banList;

#define WP_STEAMID		"STEAM_0:0:1699197"
#define WP_WELCOME_SPK		"speak \"vox/_period hello _period white pal(e45) men(s85) something(s46e65) enter(s73)\"\n"
extern int max_marine_points;
extern int max_alien_points;

static const char* marine_rang[29] =
{
	"KAZE XTREME!", "Legendary Dreadnought", "Dreadnought", "Planetary Elite, Class1",
	"Planetary Elite", "Planetary Fighter, Class1", "Planetary Fighter", "Planetary Guard, Class1", "Planetary Guard",
	"Planetary Patrol, Class1", "Planetary Patrol", "Spec Ops, Class1","Spec Ops", "Battle Master",
	"5-Star General", "4-Star General", "3-Star General", "2-Star General", "1-Star General",

	"General", "Field Marshal", "Major", "Commander", "Captain",
	"Lieutenant", "Sergant", "Corporal", "Private First Class", "Private"
};

static const char* alien_rang[29] =
{
	"FAMINE SPIRIT!", "Black Ethergaunt", "White Ethergaunt", "Red Etherguant",
	"Green Etherguant", "Xerfilstyx", "Paeliryon","Xerfilyx", "Myrmyxicus",
	"Wastrilith", "Skulvynn", "Alkilith", "Klurichir", "Maurezhi",
	"Shatorr", "Kelubar", "Faarastu", "Cronotyryn", "Ancient Behemoth",

	"Behemoth", "Nightmare", "Eliminator", "Slaughterer", "Rampager",
	"Attacker", "Ambusher", "Minion", "Xenoform", "Hatchling"
};

static const char* alien_weapon_list[20] =
{
	"weapon_bitegun", "weapon_parasite", "weapon_leap", "weapon_divinewind",		// Skulk
	"weapon_spit", "weapon_healingspray", "weapon_bilebombgun", "weapon_webspinner",	// Gorge
	"weapon_bite2gun", "weapon_spore", "weapon_umbra", "weapon_primalscream",		// Lerk
	"weapon_swipe", "weapon_blink", "weapon_metabolize", "weapon_acidrocketgun",		// Fade
	"weapon_claws", "weapon_devour", "weapon_stomp", "weapon_charge"			// Onos
};

static const byte alien_weapon_num[20] =
{
	5, 10, 21, 12,	// Skulk
	2, 27, 25, 8,	// Gorge
	6, 3, 23, 24,	// Lerk
	7, 11, 9, 26,	// Fade
	1, 30, 29, 22	// Onos
};

static const float class_base_hp[13] =
{
	100.0,
	70.0, 150.0, 125.0, 300.0, 950.0,
	100.0, 100.0, 100.0, 100.0,
	200.0,
	0.0, 0.0
};

static const float class_base_ap[13] =
{
	0.0,
	10.0, 50.0, 30.0, 150.0, 600.0,
	30.0, 30.0, 200.0, 30.0,
	150.0,
	0.0, 0.0
};

static const float class_base_ap_lvl3[13] =
{
	0.0,
	30.0, 120.0, 60.0, 250.0, 950.0,
	90.0, 90.0, 290.0, 90.0,
	150.0,
	0.0, 0.0
};

// No Weapon Equiped + 30 Weapons
static const float BasicDmg[31] =
{
	0.0,			// none
	0.0, 0.0, 0.0,
	0.0, 0.0, 0.0,
	0.0, 0.0, 0.0,
	10.0,			// parasite
	0.0, 0.0,
	0.0,
	20.0,			// pistol
	10.0,			// lmg
	17.0,			// shotgun
	20.0,			// hmg
	0.0,
	0.0,
	125.0,			// gl
	0.0, 0.0, 0.0,
	0.0, 0.0, 0.0,
	13.0,			// healspray
	80.0,			// grenade
	0.0, 0.0
};

// sound when player gestates
#define GESTATE_SOUND( pEntity ) \
	CLIENT_COMMAND(pEntity, "spk hud/points_spent\n")

#define DISPLAY_TIME_SPECIAL		0.5
#define DISPLAY_TIME			1.0
#define XP_ADDER_BASE			60.0
#define XP_ADDER_PER_LEVEL		10.0
#define XP_ADDER_POST			10.0

#define PROGRESS_BAR_CHANNEL		3

#define IUSER3_CLASS_SKULK		3
#define IUSER3_CLASS_GESTATE		8

#define BASE_AP_ADDER_MA_JP		20.0
#define BASE_AP_ADDER_HA		30.0

#define MAX_ENERGY			1000.0

#define PLAYER_VIEWCONE			0.71

#endif
