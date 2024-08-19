//======================================================================
// Forward declarations for functions that deal directly with the engine
//======================================================================

#ifndef _UTILFUNCTIONS_H_
#define _UTILFUNCTIONS_H_

#define ABSOLUTE_VALUE_EASY(x) (((x) < 0) ? (-(x)) : (x)) //very useful for gpGlobals->time comparisons

#include "plugin.h"
#include "ns_const.h"
#include "player_data.h"
#include "events_const.h"

enum CLIENT_CMD_type
{
	PRINT_NOTIFY = 1,
	PRINT_CONSOLE,
	PRINT_CHAT,
	PRINT_CENTER,
};

enum TRACE_HULL_type
{
	HULL_POINT = 0,
	HULL_HUMAN,
	HULL_LARGE,
	HULL_HEAD
};

enum Config_data_type
{
	TYPE_INT = 1,
	TYPE_FLOAT,
	TYPE_BOOL,
	TYPE_BYTE
};

struct Config_data
{
	void* address;
	int data_type;
	char* config_name;
};


//just declare extra helper functions you need here

void UTIL_ServerPrint(const char* fmt, ...);
void UTIL_ClientPrint(edict_t* pEntity, int Msg_Type, char* Msg);



void UTIL_clearMenu(edict_t* pEdict);
void UTIL_ShowMenu(edict_t* pEdict, int slots, int displaytime, char* pText);
bool UTIL_isAlive(edict_t* pEntity);
bool UTIL_isCloaked(edict_t* pEntity);
void UTIL_giveItem(edict_t* pEntity, const char* Item);
void UTIL_sendScoreInfo(int ID);

int UTIL_getPoints(edict_t* pEntity);
void UTIL_setPoints(edict_t* pEntity, int Points);
void UTIL_addPoints(edict_t* pEntity, int Points);

int UTIL_getWeaponClip(edict_t* entWeapon);
void UTIL_setWeaponClip(edict_t* entWeapon, int ClipSize);
void UTIL_addWeaponClip(edict_t* entWeapon, int ClipSize);

int UTIL_getWeaponReserve(edict_t* pEntity, byte WeaponID);
void UTIL_setWeaponReserve(edict_t* pEntity, byte WeaponID, int ReserveSize);
void UTIL_addWeaponReserve(edict_t* pEntity, byte WeaponID, int ReserveSize);

float UTIL_getWeaponDamage(edict_t* entWeapon);
void UTIL_setWeaponDamage(edict_t* entWeapon, float damage);
void UTIL_addWeaponDamage(edict_t* entWeapon, float damage);

int UTIL_getScore(edict_t* pEntity);
void UTIL_setScore(edict_t* pEntity, int Score);
void UTIL_addScore(edict_t* pEntity, int Score);

float UTIL_getEXP(edict_t* pEntity);
void UTIL_setEXP(edict_t* pEntity, float XP);
void UTIL_addEXP(edict_t* pEntity, float XP);

int UTIL_getDeaths(edict_t* pEntity);
void UTIL_setDeaths(edict_t* pEntity, int Deaths);
void UTIL_addDeaths(edict_t* pEntity, int Deaths);

bool UTIL_getMask(edict_t* pEntity, int Mask);
void UTIL_setMask(edict_t* pEntity, int Mask, bool on_off);

int UTIL_getHiveAbility(edict_t* pEntity, int HiveAbilityNum);
int UTIL_getClass(edict_t* pEntity);

bool UTIL_hasWeapon(edict_t* pEntity, byte WeaponID);

int UTIL_getArmorUpgrade(edict_t* pEntity);


void UTIL_showPopup(edict_t* pEntity, char* Msg, int ObeyAutohelp = 0);
void UTIL_setHUD(hudtextparms_t& hud_params, byte R, byte G, byte B, float X, float Y, int Effects, float FXTime, float HoldTime, float FadeInTime, float FadeOutTime, int Channel);
void UTIL_HudMessage(edict_t* pEntity, const hudtextparms_t& textparms, const char* pMessage);
void UTIL_resetHUD(edict_t* pEntity);

void UTIL_getUpgradeDataFromFile(Config_data upgrade_data[], int array_size);
void UTIL_getBanData();
void UTIL_getConfigFilenames();

int get_private(edict_t* pEntity, int offset);
float get_private_f(edict_t* pEntity, int offset);
void set_private(edict_t* pEntity, int offset, int value);
void set_private_f(edict_t* pEntity, int offset, float value);
void add_private(edict_t* pEntity, int offset, int value);
void add_private_f(edict_t* pEntity, int offset, float value);

#ifdef _DEBUG
extern bool debug_running;
void UTIL_LogDebug(const char* text);
void UTIL_ClearLog();
int UTIL_getLogCount();
#endif





inline void SAFE_USER_MSG(int& gmsgvar, const char* szMsgName, const int& iSize)
{
	if ( gmsgvar <= 0 )
	{
		//attempt to get the msg from the game that's running
		gmsgvar = GET_USER_MSG_ID(&Plugin_info, szMsgName, NULL);

		//and if that fails, register it ourselves
		if ( gmsgvar == 0 )
			gmsgvar = REG_USER_MSG(szMsgName, iSize);
	}
}

inline void UTIL_ClientPrint(edict_t* pEntity, int Msg_Type, char* Msg)
{
	// Client only accepts max 189 chars before overflow
	if ( strlen(Msg) > 189 )
		Msg[190] = 0;

	MESSAGE_BEGIN(MSG_ONE, TextMsg_ID, NULL, pEntity);
	WRITE_BYTE(Msg_Type);
	WRITE_STRING(Msg);
	MESSAGE_END();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////// PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER ////////////
////////// PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER PLAYER ////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline int UTIL_getPoints(edict_t* pEntity)
{
	return get_private(pEntity, MAKE_OFFSET(POINTS));
}

inline void UTIL_setPoints(edict_t* pEntity, int Points)
{
	set_private(pEntity, MAKE_OFFSET(POINTS), Points);
}

inline void UTIL_addPoints(edict_t* pEntity, int Points)
{
	add_private(pEntity, MAKE_OFFSET(POINTS), Points);
}

inline int UTIL_getWeaponClip(edict_t* entWeapon)
{
	return get_private(entWeapon, MAKE_OFFSET(WEAPCLIP));
}

inline void UTIL_setWeaponClip(edict_t* entWeapon, int ClipSize)
{
	set_private(entWeapon, MAKE_OFFSET(WEAPCLIP), ClipSize);
}

inline void UTIL_addWeaponClip(edict_t* entWeapon, int ClipSize)
{
	add_private(entWeapon, MAKE_OFFSET(WEAPCLIP), ClipSize);
}

inline float UTIL_getWeaponDamage(edict_t* entWeapon)
{
	if ( entWeapon->pvPrivateData == NULL )
		return 0.0;

	return get_private_f(entWeapon, MAKE_OFFSET(WEAPDMG));
}

inline void UTIL_setWeaponDamage(edict_t* entWeapon, float damage)
{
	return set_private_f(entWeapon, MAKE_OFFSET(WEAPDMG), damage);
}

inline void UTIL_addWeaponDamage(edict_t* entWeapon, float damage)
{
	return add_private_f(entWeapon, MAKE_OFFSET(WEAPDMG), damage);
}

inline int UTIL_getScore(edict_t* pEntity)
{
	return get_private(pEntity, MAKE_OFFSET(SCORE));
}

inline void UTIL_setScore(edict_t* pEntity, int Score)
{
	set_private(pEntity, MAKE_OFFSET(SCORE), Score);
}

inline void UTIL_addScore(edict_t* pEntity, int Score)
{
	add_private(pEntity, MAKE_OFFSET(SCORE), Score);
}

inline float UTIL_getEXP(edict_t* pEntity)
{
	return get_private_f(pEntity, MAKE_OFFSET(EXP));
}

inline void UTIL_setEXP(edict_t* pEntity, float XP)
{
	set_private_f(pEntity, MAKE_OFFSET(EXP), XP);
}

inline void UTIL_addEXP(edict_t* pEntity, float XP)
{
	add_private_f(pEntity, MAKE_OFFSET(EXP), XP);
}

inline int UTIL_getDeaths(edict_t* pEntity)
{
	return get_private(pEntity, MAKE_OFFSET(DEATHS));
}

inline void UTIL_setDeaths(edict_t* pEntity, int Deaths)
{
	set_private(pEntity, MAKE_OFFSET(DEATHS), Deaths);
}

inline void UTIL_addDeaths(edict_t* pEntity, int Deaths)
{
	add_private(pEntity, MAKE_OFFSET(DEATHS), Deaths);
}

inline int UTIL_getHiveAbility(edict_t* pEntity, int HiveAbilityNum)
{
	int result = get_private(pEntity, MAKE_OFFSET(HIVEABILITY));

	return (HiveAbilityNum > 0) ? (result >= HiveAbilityNum - 1) : result;
}

inline int UTIL_getClass(edict_t* pEntity)
{
	if ( pEntity->v.deadflag )
		return NS_CLASS_DEAD;
	if ( !pEntity->v.team )
		return NS_CLASS_NOTEAM;
	switch ( pEntity->v.iuser3 )
	{
		case 1:
			// Light armor marine..
			if ( pEntity->v.iuser4 & NS_MASK_HEAVYARMOR )
				return NS_CLASS_HEAVY;
			if ( pEntity->v.iuser4 & NS_MASK_JETPACK )
				return NS_CLASS_JETPACK;
			return NS_CLASS_MARINE;
		case 2:
			return NS_CLASS_COMMANDER;
		case 3:
			return NS_CLASS_SKULK;
		case 4:
			return NS_CLASS_GORGE;
		case 5:
			return NS_CLASS_LERK;
		case 6:
			return NS_CLASS_FADE;
		case 7:
			return NS_CLASS_ONOS;
		case 8:
			return NS_CLASS_GESTATE;
	}
	return NS_CLASS_UNKNOWN;
}

inline bool UTIL_getMask(edict_t* pEntity, int Mask)
{
	return (pEntity->v.iuser4 & Mask) ? true : false;
}

inline void UTIL_setMask(edict_t* pEntity, int Mask, bool on_off)
{
	if ( on_off )
	{
		pEntity->v.iuser4 |= Mask;
	} else if ( pEntity->v.iuser4 & Mask )
	{
		pEntity->v.iuser4 &= ~Mask;
	}
}

inline bool UTIL_hasWeapon(edict_t* pEntity, byte WeaponID)
{
	return (pEntity->v.weapons & (1 << WeaponID)) ? true : false;
}

inline int UTIL_getArmorUpgrade(edict_t* pEntity)
{
	return (pEntity->v.iuser4 & NS_MASK_ARMOR3) ? 3
		: ((pEntity->v.iuser4 & NS_MASK_ARMOR2) ? 2
			: ((pEntity->v.iuser4 & NS_MASK_ARMOR1) ? 1
				: 0));
}

inline void UTIL_giveItem(edict_t* pEntity, const char* Item)
{
	//edict_t *object = CREATE_NAMED_ENTITY(ALLOC_STRING(Item));	//create
	int hl_strings_item_id = hl_strings.find(Item);
	edict_t* object = CREATE_NAMED_ENTITY(hl_strings_item_id);	//create
	if ( !object )
		return;

	SET_ORIGIN(object, pEntity->v.origin);				// move to player
	gpGamedllFuncs->dllapi_table->pfnSpawn(object);			// emulate spawn
	object->v.flags |= FL_ONGROUND;					// make it think it's touched the ground
	gpGamedllFuncs->dllapi_table->pfnThink(object);			// let the object think
	gpGamedllFuncs->dllapi_table->pfnTouch(object, pEntity);	// let player touch ( pickup ) item
}

inline bool UTIL_isAlive(edict_t* pEntity)
{
	return (pEntity->v.deadflag == DEAD_NO
		&& pEntity->v.health > 0);
}

inline bool UTIL_is_in_viewcone(edict_t* pEntity, edict_t* targetEntity)
{
	MAKE_VECTORS(pEntity->v.v_angle);

	Vector vecLOS = targetEntity->v.origin - pEntity->v.origin;
	vecLOS = vecLOS.Normalize();

	float dot_procuct = DotProduct(vecLOS, gpGlobals->v_forward);

	if ( dot_procuct > PLAYER_VIEWCONE )
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////// MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC /////////
////////// MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC MISC /////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

/*inline bool UTIL_isCloaked( edict_t *pEntity )
{
	return ( get_private(pEntity, MAKE_OFFSET(CLOAK)) < 0.5
		|| player_etherealshift[ENTINDEX(pEntity)].Shifting == true );
}*/

inline void UTIL_showPopup(edict_t* pEntity, char* Msg, int ObeyAutohelp)
{
	MESSAGE_BEGIN(MSG_ONE, HUDText2_ID, NULL, pEntity);
	WRITE_STRING(Msg);
	WRITE_BYTE(ObeyAutohelp);
	MESSAGE_END();
}

inline void UTIL_setHUD(hudtextparms_t& hud_params, byte R, byte G, byte B, float X, float Y, int Effects, float FXTime, float HoldTime, float FadeInTime, float FadeOutTime, int Channel)
{
	hud_params.a1 = 0;
	hud_params.a2 = 0;
	hud_params.r2 = 255;
	hud_params.g2 = 255;
	hud_params.b2 = 250;
	hud_params.r1 = R;
	hud_params.g1 = G;
	hud_params.b1 = B;
	hud_params.x = X;
	hud_params.y = Y;
	hud_params.effect = Effects;
	hud_params.fxTime = FXTime;
	hud_params.holdTime = HoldTime;
	hud_params.fadeinTime = FadeInTime;
	hud_params.fadeoutTime = FadeOutTime;
	hud_params.channel = Channel;
}

inline void UTIL_HudMessage(edict_t* pEntity, const hudtextparms_t& textparms, const char* pMessage)
{
	MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity);
	WRITE_BYTE(TE_TEXTMESSAGE);
	WRITE_BYTE(textparms.channel & 0xFF);

	WRITE_SHORT(FixedSigned16(textparms.x, 1 << 13));
	WRITE_SHORT(FixedSigned16(textparms.y, 1 << 13));
	WRITE_BYTE(textparms.effect);

	WRITE_BYTE(textparms.r1);
	WRITE_BYTE(textparms.g1);
	WRITE_BYTE(textparms.b1);
	WRITE_BYTE(textparms.a1);

	WRITE_BYTE(textparms.r2);
	WRITE_BYTE(textparms.g2);
	WRITE_BYTE(textparms.b2);
	WRITE_BYTE(textparms.a2);

	WRITE_SHORT(FixedUnsigned16(textparms.fadeinTime, 1 << 8));
	WRITE_SHORT(FixedUnsigned16(textparms.fadeoutTime, 1 << 8));
	WRITE_SHORT(FixedUnsigned16(textparms.holdTime, 1 << 8));

	if ( textparms.effect == 2 )
		WRITE_SHORT(FixedUnsigned16(textparms.fxTime, 1 << 8));

	if ( strlen(pMessage) < 512 )
	{
		WRITE_STRING(pMessage);
	} else
	{
		char tmp[512];
		strncpy(tmp, pMessage, 511);
		tmp[511] = 0;
		WRITE_STRING(tmp);
	}
	MESSAGE_END();
}

inline void UTIL_resetHUD(edict_t* pEntity)
{
	hudtextparms_t dummy;
	dummy.channel = HUD_CHANNEL;
	UTIL_HudMessage(pEntity, dummy, "");
}

inline void UTIL_clearMenu(edict_t* pEdict)
{
	UTIL_ShowMenu(pEdict, 1, 1, " \n");
}

inline void UTIL_ShowMenu(edict_t* pEdict, int slots, int displaytime, char* pText)
{
	char* dummy_text = pText;
	char temp_char = 0;
	int temp_len;
	int string_len = strlen(pText);

	while ( *dummy_text )
	{
		temp_len = string_len;
		if ( temp_len > 175 )
			temp_len = 175;
		string_len -= temp_len;
		temp_char = *(dummy_text += temp_len);
		*dummy_text = 0;

		MESSAGE_BEGIN(MSG_ONE, ShowMenu_ID, NULL, pEdict);
		WRITE_SHORT(slots);
		WRITE_CHAR(displaytime);
		WRITE_BYTE(temp_char ? TRUE : FALSE);
		WRITE_STRING(pText);
		MESSAGE_END();

		*dummy_text = temp_char;
		pText = dummy_text;
	}
}

inline void UTIL_sendScoreInfo(int ID)
{
	/*MESSAGE_BEGIN(MSG_ALL, ScoreInfo_ID);
	WRITE_BYTE(player_data[ID].scoreinfo_data[SCORE_INFO_ID]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_SCORE]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_KILLS]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_DEATHS]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_LEVEL]);
	WRITE_BYTE(player_data[ID].scoreinfo_data[SCORE_INFO_CLASS]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_ICON]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_TEAM]);
	WRITE_SHORT(player_data[ID].scoreinfo_data[SCORE_INFO_HEALTH]);
	WRITE_STRING(player_data[ID].scoreinfo_string);
	MESSAGE_END();*/

	gIgnore_Self_Send_Msg = true;
	g_pEngTable->pfnMessageBegin(MSG_ALL, ScoreInfo_ID, NULL, NULL);
	g_pEngTable->pfnWriteByte(player_data[ID].scoreinfo_data[SCORE_INFO_ID]);
	g_pEngTable->pfnWriteShort(player_data[ID].scoreinfo_data[SCORE_INFO_SCORE]);
	g_pEngTable->pfnWriteShort(player_data[ID].scoreinfo_data[SCORE_INFO_KILLS]);
	g_pEngTable->pfnWriteShort(player_data[ID].scoreinfo_data[SCORE_INFO_DEATHS]);
	g_pEngTable->pfnWriteShort(player_data[ID].scoreinfo_data[SCORE_INFO_LEVEL]);
	g_pEngTable->pfnWriteByte(player_data[ID].scoreinfo_data[SCORE_INFO_CLASS]);
	g_pEngTable->pfnWriteShort(player_data[ID].scoreinfo_data[SCORE_INFO_ICON]);
	g_pEngTable->pfnWriteShort(player_data[ID].scoreinfo_data[SCORE_INFO_TEAM]);
	g_pEngTable->pfnWriteShort(player_data[ID].scoreinfo_data[SCORE_INFO_HEALTH]);
	g_pEngTable->pfnWriteString(player_data[ID].scoreinfo_string);
	g_pEngTable->pfnMessageEnd();

	gIgnore_Self_Send_Msg = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE /////////
////////// PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE PRIVATE /////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

inline int get_private(edict_t* pEntity, int offset)
{
	return *(int*)((char*)(pEntity->pvPrivateData) + offset);
}

inline float get_private_f(edict_t* pEntity, int offset)
{
	return *(float*)((char*)(pEntity->pvPrivateData) + offset);
}

inline void set_private(edict_t* pEntity, int offset, int value)
{
	*(int*)((char*)(pEntity->pvPrivateData) + offset) = value;
}

inline void set_private_f(edict_t* pEntity, int offset, float value)
{
	*(float*)((char*)(pEntity->pvPrivateData) + offset) = value;
}

inline void add_private(edict_t* pEntity, int offset, int value)
{
	*(int*)((char*)(pEntity->pvPrivateData) + offset) += value;
}

inline void add_private_f(edict_t* pEntity, int offset, float value)
{
	*(float*)((char*)(pEntity->pvPrivateData) + offset) += value;
}




#endif
