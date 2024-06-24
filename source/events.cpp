
#include "plugin.h"
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
#include "player_data.h"
#include "exp_controller.h"
#include "events.h"

int TextMsg_ID = -1;
int HUDText2_ID = -1;
int AmmoPickup_ID = -1;
int SetTech_ID = -1;
int ScoreInfo_ID = -1;
int HideWeapon_ID = -1;
int Progress_ID = -1;
int WeapPickup_ID = -1;
int StatusValue_ID = -1;
int ScreenFade_ID = -1;

int DeathMsg_ID = -1;
int CurWeapon_ID = -1;
int Particles_ID = -1;
int Damage_ID = -1;
int TeamInfo_ID = -1;
int ShowMenu_ID = -1;

unsigned short Spore_Event_ID = 0;
unsigned short Cloak_Event_ID = 0;
unsigned short Umbra_Event_ID = 0;
unsigned short Particle_Event_ID = 0;
unsigned short HealingSpray_Event_ID = 0;

char ParticleNames[PARTICLE_LIST_LEN][33];
int ParticleCount = 0;
int Xenocide_Particle_ID = -1;
int BileBomb_Particle_ID = -1;
int AcidHit_Particle_ID = -1;




