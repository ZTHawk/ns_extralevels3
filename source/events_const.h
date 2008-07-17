#ifndef _EVENTS_CONST_H_
#define _EVENTS_CONST_H_


#define CURWEAPON_NO_WEAPON		0
#define CURWEAPON_TO_WEAPON		6
#define SETTECH_MSG_ARG_ON		4
#define DAMAGE_MSG_ARG_DMG		2
#define DEATH_MSG_ARG_KILLER_ID		1
#define DEATH_MSG_ARG_VICTIM_ID		2

#define STATUS_VALUE_PLAYER_ID		1
#define STATUS_VALUE_PLAYER_HP		2
#define STATUS_VALUE_PLAYER_AP		3
#define STATUS_VALUE_PLAYER_LEVEL	4

#define SCORE_INFO_ID			1
#define SCORE_INFO_SCORE		2
#define SCORE_INFO_KILLS		3
#define SCORE_INFO_DEATHS		4
#define SCORE_INFO_LEVEL		5
#define SCORE_INFO_CLASS		6
#define SCORE_INFO_ICON			7
#define SCORE_INFO_TEAM			8
#define SCORE_INFO_HEALTH		9

#define PARTICLE_LIST_LEN		64

extern int TextMsg_ID;
extern int HUDText2_ID;
extern int AmmoPickup_ID;
extern int SetTech_ID;
extern int ScoreInfo_ID;
extern int HideWeapon_ID;
extern int Progress_ID;
extern int WeapPickup_ID;
extern int StatusValue_ID;
extern int ScreenFade_ID;

extern int DeathMsg_ID;
extern int CurWeapon_ID;
extern int Particles_ID;
extern int Damage_ID;
extern int TeamInfo_ID;
extern int ShowMenu_ID;

extern char ParticleNames[PARTICLE_LIST_LEN][33];
extern int ParticleCount;
extern int Xenocide_Particle_ID;
extern int BileBomb_Particle_ID;
extern int AcidHit_Particle_ID;

extern unsigned short Spore_Event_ID;
static const char *Spore_Event_Name = "events/SporeCloud.sc";
extern unsigned short Cloak_Event_ID;
static const char *Cloak_Event_Name = "events/StartCloak.sc";
extern unsigned short Umbra_Event_ID;
static const char *Umbra_Event_Name = "events/UmbraCloud.sc";
extern unsigned short Particle_Event_ID;
static const char *Particle_Event_Name = "events/Particle.sc";
extern unsigned short HealingSpray_Event_ID;
static const char *HealingSpray_Event_Name = "events/HealingSpray.sc";

extern int FuseLight;

#endif


