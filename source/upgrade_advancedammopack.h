#ifndef _UPGRADE_ADVANCEDAMMOPACK_H_
#define _UPGRADE_ADVANCEDAMMOPACK_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

static const char* AA_config_names[] =
{
	"ADVAMMOPACK",
	"AA_COST",
	"AA_MAX",
	"AA_LEVEL",
	"AA_PISTOL",
	"AA_LMG",
	"AA_SHOTGUN",
	"AA_HMG",
	"AA_GL"
};

class Upgrade_AdvancedAmmopack : public base_upgrade_data
{
public:
	float Ammo_per_WeaponID[33];		// each WeaponID own entry

	void init();
	bool add_to_menu(byte ID, int num, int& Keys, char* menu);
	void show_upgrade_menu(edict_t* player);
};

class EL_AdvancedAmmopack : public base_upgrade_pl_data
{
	UPGRADE_DATA_PLAYER;	// Makro

public:
	float endReloadTime;			// time when reload will be done

	void respawned();
	void reset_basic();

	void fixAnimation();
	void setWeaponData_Ammo(byte mode = 0);
	void setAmmoPickupMsg(byte AmmoType);
	bool setHUDText(byte vID, bool is_marine, hudtextparms_t& hud_params, char* CoreT_GL_reload_Shift_text);

private:
	int bonusAmmo[33];			// amount of ammo each weapon gets from AA ( each WeaponID own entry )

	int entPistolID;
	int entPrimaryGunID;
	byte PrimaryGunID;
	int entPrimaryGunOldID;

	float GL_ReloadTime_reducer;		// amount of time the reload time will be reduced
	int freeAmmo_at_lvl[33];		// check at which level each weapon got ammo when purchased AA ( each WeaponID own entry )

	byte curReloadingWeapon;		// check which weapon player is reloading
	bool ReloadingStarted;			// check if player started to reload

	int Current_Ammo;			// current Ammo
	int Reserve_Ammo;			// current Ammo in Reserve

	int Ammo_before_reload;			// store amount of ammo before reloading with Advanced Ammo
	int Reserve_before_reload;		// store amount of reserve ammo before reloading with Advanced Ammo

	int Shotgun_bulltes_stolen;		// amount of bullets that has been removed from shotgun's reserve
	int Shotgun_bulltes_xtra_stolen;
	int Shotgun_bulltes_to_steal;		// amount of bullets that needs to be removed from shotgun's reserve before reload ends

	int bullets_stolen;

	int Reserve_ammo_corrector;		// 
	int curWeaponMaxAmmoBasic;
	float SpawnTime;

	void resetGunPointer();
	void resetExtra();
	void findWeaponData(byte mode = 0);
	void addWeaponClip(edict_t* entWeapon, int ammo_to_add);
	void addWeaponReserve(byte WeaponID, int ammo_to_add);

	void checkReloadCorrectors(byte WeaponID, int* Ammo_to_add);
	void checkReloadStarting(byte WeaponID, edict_t* entWeapon);
	void checkReloadingFullAmmo(byte WeaponID, edict_t* entWeapon);
	void ReloadInitiate(byte WeaponID, int Ammo_to_add, bool isWeaponReloading);
	void ReloadShotgun(edict_t* entWeapon);
	void ReloadOtherWeapons(byte WeaponID, edict_t* entWeapon, int Ammo_to_add, bool isWeaponReloading);
};

extern Upgrade_AdvancedAmmopack data_advancedammopack;
extern EL_AdvancedAmmopack player_advancedammopack[MAX_PLAYERS_PLUS1];

// No Weapon Equiped + 30 Weapons
static const int BasicAmmo[31] =
{
	0,		// none
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0,
	10,	// pistol
	50,	// lmg
	8,	// shotgun
	125,	// hmg
	0,
	0,
	4,	// gl
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0
};

#define RELOAD_TIME_PISTOL			3.0
#define RELOAD_TIME_LMG				3.5
#define RELOAD_TIME_SHOTGUN_INIT		1.2
#define RELOAD_TIME_SHOTGUN			0.5
#define RELOAD_TIME_SHOTGUN_END			1.2
#define RELOAD_TIME_HMG				7.0
#define RELOAD_TIME_GL_INIT			2.2
#define RELOAD_TIME_GL				1.1

#define ANIMATION_RELOAD_DONE_SHOTGUN		4
#define ANIMATION_SWITCHED_TO_GL		13

#define AA_BOUGHT_NEW_WEAPON			1

#define MAX_VISIBLE_AMMO			250

// default config settings
#define ADVAMMOPACK				true
#define AA_COST					1
#define AA_MAX					5
#define AA_LEVEL				0
#define AA_PISTOL				8.0
#define AA_LMG					10.0
#define AA_SHOTGUN				4.0
#define AA_HMG					20.0
#define AA_GL					1.0

#endif

