#ifndef _UPGRADE_SENSEOFANCIENTS_H_
#define _UPGRADE_SENSEOFANCIENTS_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"
#include <vector>
using namespace std;

static const char* SOA_config_names[] =
{
	"SENSEOFANCIENTS",
	"SOA_COST",
	"SOA_MAX",
	"SOA_LEVEL",
	"SOA_PARASITE_INIT",
	"SOA_PARASITE_ADD",
	"SOA_PARASITE_DMG",
	"SOA_HEALSPRAY_DMG",
	"SOA_GASDAMAGE",
	"SOA_BLINK_ENERGY_BONUS",
	"SOA_DEVOUR_ADDER",
	"SOA_DEVOURTIME_INIT",
	"SOA_DEVOURTIME_BONUS",
	"SOA_GESTATE_ARMOR_ADD"
};

typedef struct SporeData_s
{
	int SporeLevel;
	int Status;
	Vector Origin;
} SporeData_t;

class Upgrade_Senseofancients : public base_upgrade_data
{
public:
	float ParasiteChanceInit;
	float ParasiteChanceBonus;
	float ParasiteDMGPercentage;
	float HealsprayPercentage;
	float SporeDMG;
	float BlinkEnergyPercentage;
	int DevourAdder;
	float DevourTimeInit;
	float DevourTimeBonus;
	float GestateArmorBonus;

	vector<SporeData_t> SporeData;
	int Spore_Num;
	float nextSporeTime;

	void init();
	bool add_to_menu(byte ID, int num, int& Keys, char* menu);
	void show_upgrade_menu(edict_t* player);

	void SporeEmulationTimer();
};

class EL_Senseofancients : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro

public:
	float nextParasiteTime;			// check when a parasited player due to SoA will do next parasite in range
	byte ParasiteMode;			// check if player got parasited by skulk or due to SoA and current status of SoA parasite
	byte Infected_by;			// check which player infected you with SoA parasite
	byte MyParasiter;

	bool In_Spore;				// check if player is inside spore

	bool JustBlinked;			// check if Fade just did a Blink
	float got_BloodLust;			// check if and how much BloodLust a player got
	int DevourTimeMultiplier;		// multiplier for SoA Onos Multi Devour
	int DevourPlayersNum;			// amount of players Onos is digesting right now

	Vector Origin_before_Redeem;

	void Think_Post();
	void respawned();
	void reset_basic();

	void check_Parasite();
	void setWeaponData_Dmg();
	void check_HealingSpray();
	void add_Spore(const edict_t* sporeEntity);
	void Blink_Energy();
	void free_digested_Players();
	void Player_Redeemed(float* Origin);
	void setGestateAP();

	void ReadyRoom();
	bool ImpulseCheckBlock();

	bool setHUDText(byte vID, bool is_marine, hudtextparms_t& hud_params, char* CoreT_GL_reload_Shift_text);

private:
	byte ParasiteChance;

	float Fade_Energy;			// last stored energy
	float BlinkEnergyReducer;

	float DigestTime;			// check when a digesting player is ready for next health decrease
	float NextDevour;			// check when Onos is able to devour next player
	float EvolveMsgBlock_time;		// check when player gets a popup telling not being able to gestate
	bool JustDevoured;			// check if player has just devoured someone
	int DevourMaxAmount;			// amount of players Onos can devour at most
	byte MyDigester;			// get players digester
	byte CurrentlyDigesting;		// check which player is the most recent disgesting player
	bool JustRedeemed;			// check if player has redeemed
	bool JustFreed;				// check if player just got released from digestion ( needed to prevent instant deaths )
	float DevourTime;			// amount of time player needs to wait between two player when doing SoA Multi Devour
	float SpawnTime;

	void reset_parasite_vars();
	void reset_devour_vars();

	void Parasite_Players();
	void Digest_Player();
	void check_for_next_Devour();
	void find_Digester();
	byte get_MyOnos();
	void set_Player_Weaponmodel();

	void add_hp_ap(edict_t* targetEntity, float health, float armor);
	void add_hp_ap_structure(edict_t* targetEntity, float health);
};

extern Upgrade_Senseofancients data_senseofancients;
extern EL_Senseofancients player_senseofancients[MAX_PLAYERS_PLUS1];


#define MAX_PARASITE_CHANCE		40


// Sense of Ancients Skulk parasite chances
static const byte rand_para_chance[50] =
{
	9, 13, 27, 32, 46, 59, 65, 74, 88, 97,
	3, 12, 29, 36, 41, 55, 63, 76, 82, 93,
	4, 17, 23, 38, 44, 58, 60, 78, 84, 91,
	5, 14, 24, 35, 47, 52, 69, 71, 89, 95,
	7, 16, 25, 31, 42, 54, 66, 79, 81, 99
};

enum Model_Index
{
	MODEL_KNIFE = 0,
	MODEL_PISTOL,
	MODEL_LMG,
	MODEL_SHOTGUN,
	MODEL_HMG,
	MODEL_GL,

	SOA_MODEL_MAX
};

static const char* View_Models[SOA_MODEL_MAX] =
{
	"models/v_kn.mdl", "models/v_hg.mdl", "models/v_mg.mdl",
	"models/v_sg.mdl", "models/v_hmg.mdl", "models/v_gg.mdl",
};

static const char* View_Models_Heavy[SOA_MODEL_MAX] =
{
	"models/v_kn_hv.mdl", "models/v_hg_hv.mdl", "models/v_mg_hv.mdl",
	"models/v_sg_hv.mdl", "models/v_hmg_hv.mdl", "models/v_gg_hv.mdl"
};

static const char* Weapon_Models[SOA_MODEL_MAX] =
{
	"models/p_kn.mdl", "models/p_hg.mdl", "models/p_mg.mdl",
	"models/p_sg.mdl", "models/p_hmg.mdl", "models/p_gg.mdl"
};

static const char No_Gestate_Digest_Msg[POPUP_MSG_LEN] = "You can't gestate while digesting a player.";


#define PARASITE_RANGE			250.0
#define SPORE_RANGE			250.0

#define PARASITE_NONE			0
#define PARASITE_INIT			1
#define PARASITE_BY_SKULK_1		2
#define PARASITE_BY_SKULK_2		3
#define PARASITE_BY_SKULK_3		4
#define PARASITE_BY_MARINE_1		5
#define PARASITE_BY_MARINE_2		6
#define PARASITE_BY_MARINE_3		7
#define PARASITE_BY_MARINE_4		8
#define PARASITE_DONE			9

#define DEVOUR_HP_DEGEN			15.25
#define DEVOUR_HP_REGEN			15.0

#define PARASITE_TIME			1.0
#define DIGEST_TIME			1.0
#define SPORE_EMU_TIME			1.0
#define EVOLVE_TIME			5.0

#define SKULK_HEALSPRAY_HP		15.0
#define SKULK_HEALSPRAY_AP		15.8
#define GORGE_HEALSPRAY_HP		19.0
#define GORGE_HEALSPRAY_AP		19.0
#define LERK_HEALSPRAY_HP		18.0
#define LERK_HEALSPRAY_AP		18.0
#define FADE_HEALSPRAY_HP		25.0
#define FADE_HEALSPRAY_AP		25.0
#define ONOS_HEALSPRAY_HP		51.0
#define ONOS_HEALSPRAY_AP		51.0
#define GESTATE_HEALSPRAY_HP		13.0
#define GESTATE_HEALSPRAY_AP		21.0
#define SLEF_HEALSPRAY_HP		9.0
#define SELF_HEALSPRAY_AP		9.5
#define STRUCTURE_HEALSPRAY_HP		65.0

#define ONOS_HEALSPRAY_RANGE		191.22
#define OTHER_HEALSPRAY_RANGE		177.51
#define HIVE_HEALSPRAY_RANGE		339.61
#define CHAMBER_HEALSPRAY_RANGE		196.54
#define ALIEN_RT_HEALSPRAY_RANGE	251.62

// default config settings
#define SENSEOFANCIENTS			true
#define SOA_COST			1
#define SOA_MAX				5
#define SOA_LEVEL			10
#define SOA_PARASITE_INIT		5.0
#define SOA_PARASITE_ADD		5.0
#define SOA_PARASITE_DMG		10.0
#define SOA_HEALSPRAY_DMG		30.0
#define SOA_GASDAMAGE			3.0
#define SOA_BLINK_ENERGY_BONUS		10.0
#define SOA_DEVOUR_ADDER		5
#define SOA_DEVOURTIME_INIT		1.5
#define SOA_DEVOURTIME_BONUS		0.2
#define SOA_GESTATE_ARMOR_ADD		15.0

#endif

