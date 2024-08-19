#ifndef _HOOKEDFUNCTIONS_H_
#define _HOOKEDFUNCTIONS_H_

//function declarations for intercepted functions, any function that we want metamod to call needs
//to be declared in this header

#include <extdll.h>

#include <string.h> //to prevent linux errors while processing meta_api.h
#include <meta_api.h>

// forwards
int Spawn(edict_t* pEntity);
void KeyValue(edict_t* pentKeyvalue, KeyValueData* pkvd);
qboolean ClientConnect_Post(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128]);
void ClientDisconnect(edict_t* pEntity);
void ClientCommand(edict_t* pEntity);
void ClientPreThink(edict_t* pEntity);
void ServerFrame(void);
void CmdStart(const edict_t* player, const struct usercmd_s* _cmd, unsigned int random_seed);

// forwards POST
qboolean ClientConnect_Post(edict_t* pEntity, const char* pszName, const char* pszAddress, char szRejectReason[128]);
void ClientPutInServer_Post(edict_t* pEntity);
void ClientUserInfoChanged_Post(edict_t* pEntity, char* infobuffer);
void ServerActivate_Post(edict_t* pEdictList, int edictCount, int clientMax);
void ServerDeactivate_Post(void);
void ClientPreThink_Post(edict_t* pEntity);

// engine funcs
void pfnMessageBegin(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed);
void pfnWriteByte(int value);
void pfnWriteShort(int value);
void pfnWriteLong(int value);
void pfnWriteCoord(float flValue);
void pfnWriteString(const char* string);
void pfnMessageEnd(void);
void pfnPlaybackEvent(int flags, const edict_t* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
void pfnAlertMessage(ALERT_TYPE atype, char* szFmt, ...);

// engine funcs POST
void pfnMessageBegin_Post(int msg_dest, int msg_type, const float* pOrigin, edict_t* ed);
void pfnWriteByte_Post(int value);
void pfnWriteShort_Post(int value);
void pfnWriteLong_Post(int value);
void pfnWriteCoord_Post(float flValue);
void pfnWriteString_Post(const char* string);
void pfnMessageEnd_Post(void);

// misc
void el3_main();
void el3_config_reload();
void el3_set_upgrade(int args);
void Cleanup_Hooks();
void Cleanup_Hooks_basic();
void Cleanup_Init();
void Set_Hooks();
void Set_Hooks_basic();
void Set_Hooks_Message();
void Set_Hooks_Message_Post();
void Clear_Hooks_Message();
void Clear_Hooks_Message_Post();

#endif
