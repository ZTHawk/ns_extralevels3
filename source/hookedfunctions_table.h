#ifndef _HOOKEDFUNCTIONS_TABLE_H_
#define _HOOKEDFUNCTIONS_TABLE_H_

//these are the functions we want to receive, be sure to declare the functions in hookedfunctions.h, then simply
//place the name of the function in the table here without paranthases after it or anything

#include <extdll.h> //always

#include <string.h> //to prevent linux errors while processing meta_api.h
#include <meta_api.h> //because almost everything in here is setup for metamod stuff
#include "hookedfunctions.h" //so these tables dont whine about functions that they think don't exist

// Global vars from metamod:
meta_globals_t* gpMetaGlobals;		// metamod globals
gamedll_funcs_t* gpGamedllFuncs;	// gameDLL function tables
mutil_funcs_t* gpMetaUtilFuncs;		// metamod utility functions

enginefuncs_t g_engfuncs;
globalvars_t* gpGlobals;

static DLL_FUNCTIONS gFunctionTable =
{
	NULL,					// pfnGameInit
	Spawn,					// pfnSpawn
	NULL,					// pfnThink
	NULL,					// pfnUse
	NULL,					// pfnTouch
	NULL,					// pfnBlocked
	KeyValue,					// pfnKeyValue
	NULL,					// pfnSave
	NULL,					// pfnRestore
	NULL,					// pfnSetAbsBox

	NULL,					// pfnSaveWriteFields
	NULL,					// pfnSaveReadFields

	NULL,					// pfnSaveGlobalState
	NULL,					// pfnRestoreGlobalState
	NULL,					// pfnResetGlobalState

	NULL,					// pfnClientConnect
	ClientDisconnect,					// pfnClientDisconnect
	NULL,					// pfnClientKill
	NULL,					// pfnClientPutInServer
	ClientCommand,					// pfnClientCommand
	NULL,					// pfnClientUserInfoChanged
	NULL,					// pfnServerActivate
	NULL,					// pfnServerDeactivate

	ClientPreThink,					// pfnPlayerPreThink
	NULL,					// pfnPlayerPostThink

	ServerFrame,					// pfnStartFrame
	NULL,					// pfnParmsNewLevel
	NULL,					// pfnParmsChangeLevel

	NULL,					// pfnGetGameDescription
	NULL,					// pfnPlayerCustomization

	NULL,					// pfnSpectatorConnect
	NULL,					// pfnSpectatorDisconnect
	NULL,					// pfnSpectatorThink

	NULL,					// pfnSys_Error

	NULL,					// pfnPM_Move
	NULL,					// pfnPM_Init
	NULL,					// pfnPM_FindTextureType

	NULL,					// pfnSetupVisibility
	NULL,					// pfnUpdateClientData
	NULL,					// pfnAddToFullPack
	NULL,					// pfnCreateBaseline
	NULL,					// pfnRegisterEncoders
	NULL,					// pfnGetWeaponData
	CmdStart,					// pfnCmdStart
	NULL,					// pfnCmdEnd
	NULL,					// pfnConnectionlessPacket
	NULL,					// pfnGetHullBounds
	NULL,					// pfnCreateInstancedBaselines
	NULL,					// pfnInconsistentFile
	NULL,					// pfnAllowLagCompensation
};

static DLL_FUNCTIONS gFunctionTable_Post =
{
	NULL,					// pfnGameInit
	NULL,					// pfnSpawn
	NULL,					// pfnThink
	NULL,					// pfnUse
	NULL,					// pfnTouch
	NULL,					// pfnBlocked
	NULL,					// pfnKeyValue
	NULL,					// pfnSave
	NULL,					// pfnRestore
	NULL,					// pfnSetAbsBox

	NULL,					// pfnSaveWriteFields
	NULL,					// pfnSaveReadFields

	NULL,					// pfnSaveGlobalState
	NULL,					// pfnRestoreGlobalState
	NULL,					// pfnResetGlobalState

	ClientConnect_Post,					// pfnClientConnect
	NULL,					// pfnClientDisconnect
	NULL,					// pfnClientKill
	ClientPutInServer_Post,					// pfnClientPutInServer
	NULL,					// pfnClientCommand
	ClientUserInfoChanged_Post,					// pfnClientUserInfoChanged
	ServerActivate_Post,					// pfnServerActivate
	ServerDeactivate_Post,					// pfnServerDeactivate

	ClientPreThink_Post,					// pfnPlayerPreThink
	NULL,					// pfnPlayerPostThink

	NULL,					// pfnStartFrame
	NULL,					// pfnParmsNewLevel
	NULL,					// pfnParmsChangeLevel

	NULL,					// pfnGetGameDescription
	NULL,					// pfnPlayerCustomization

	NULL,					// pfnSpectatorConnect
	NULL,					// pfnSpectatorDisconnect
	NULL,					// pfnSpectatorThink

	NULL,					// pfnSys_Error

	NULL,					// pfnPM_Move
	NULL,					// pfnPM_Init
	NULL,					// pfnPM_FindTextureType

	NULL,					// pfnSetupVisibility
	NULL,					// pfnUpdateClientData
	NULL,					// pfnAddToFullPack
	NULL,					// pfnCreateBaseline
	NULL,					// pfnRegisterEncoders
	NULL,					// pfnGetWeaponData
	NULL,					// pfnCmdStart
	NULL,					// pfnCmdEnd
	NULL,					// pfnConnectionlessPacket
	NULL,					// pfnGetHullBounds
	NULL,					// pfnCreateInstancedBaselines
	NULL,					// pfnInconsistentFile
	NULL,					// pfnAllowLagCompensation
};

static NEW_DLL_FUNCTIONS gNewFunctionTable =
{
	NULL,					//! pfnOnFreeEntPrivateData()	Called right before the object's memory is freed.  Calls its destructor.
	NULL,					//! pfnGameShutdown()
	NULL,					//! pfnShouldCollide()
	NULL,					// pfnCvarValue()
	NULL,					// pfnCvarValue2()
};

static NEW_DLL_FUNCTIONS gNewFunctionTable_Post =
{
	NULL,					//! pfnOnFreeEntPrivateData()	Called right before the object's memory is freed.  Calls its destructor.
	NULL,					//! pfnGameShutdown()
	NULL,					//! pfnShouldCollide()
	NULL,					// pfnCvarValue()
	NULL,					// pfnCvarValue2()
};

enginefuncs_t meta_engfuncs = {
	NULL,					// pfnPrecacheModel()
	NULL,					// pfnPrecacheSound()
	NULL,					// pfnSetModel()
	NULL,					// pfnModelIndex()
	NULL,					// pfnModelFrames()

	NULL,					// pfnSetSize()
	NULL,					// pfnChangeLevel()
	NULL,					// pfnGetSpawnParms()
	NULL,					// pfnSaveSpawnParms()

	NULL,					// pfnVecToYaw()
	NULL,					// pfnVecToAngles()
	NULL,					// pfnMoveToOrigin()
	NULL,					// pfnChangeYaw()
	NULL,					// pfnChangePitch()

	NULL,					// pfnFindEntityByString()
	NULL,					// pfnGetEntityIllum()
	NULL,					// pfnFindEntityInSphere()
	NULL,					// pfnFindClientInPVS()
	NULL,					// pfnEntitiesInPVS()

	NULL,					// pfnMakeVectors()
	NULL,					// pfnAngleVectors()

	NULL,					// pfnCreateEntity()
	NULL,					// pfnRemoveEntity()
	NULL,					// pfnCreateNamedEntity()

	NULL,					// pfnMakeStatic()
	NULL,					// pfnEntIsOnFloor()
	NULL,					// pfnDropToFloor()

	NULL,					// pfnWalkMove()
	NULL,					// pfnSetOrigin()

	NULL,					// pfnEmitSound()
	NULL,					// pfnEmitAmbientSound()

	NULL,					// pfnTraceLine()
	NULL,					// pfnTraceToss()
	NULL,					// pfnTraceMonsterHull()
	NULL,					// pfnTraceHull()
	NULL,					// pfnTraceModel()
	NULL,					// pfnTraceTexture()
	NULL,					// pfnTraceSphere()
	NULL,					// pfnGetAimVector()

	NULL,					// pfnServerCommand()
	NULL,					// pfnServerExecute()
	NULL,					// pfnClientCommand()	// d'oh, ClientCommand in dllapi too

	NULL,					// pfnParticleEffect()
	NULL,					// pfnLightStyle()
	NULL,					// pfnDecalIndex()
	NULL,					// pfnPointContents()

	pfnMessageBegin,					// pfnMessageBegin()
	pfnMessageEnd,					// pfnMessageEnd()

	pfnWriteByte,					// pfnWriteByte()
	NULL,					// pfnWriteChar()
	pfnWriteShort,					// pfnWriteShort()
	pfnWriteLong,					// pfnWriteLong()
	NULL,					// pfnWriteAngle()
	pfnWriteCoord,					// pfnWriteCoord()
	pfnWriteString,					// pfnWriteString()
	NULL,					// pfnWriteEntity()

	NULL,					// pfnCVarRegister()
	NULL,					// pfnCVarGetFloat()
	NULL,					// pfnCVarGetString()
	NULL,					// pfnCVarSetFloat()
	NULL,					// pfnCVarSetString()

	pfnAlertMessage,					// pfnAlertMessage()
	NULL,					// pfnEngineFprintf()

	NULL,					// pfnPvAllocEntPrivateData()
	NULL,					// pfnPvEntPrivateData()
	NULL,					// pfnFreeEntPrivateData()

	NULL,					// pfnSzFromIndex()
	NULL,					// pfnAllocString()

	NULL, 					// pfnGetVarsOfEnt()
	NULL,					// pfnPEntityOfEntOffset()
	NULL,					// pfnEntOffsetOfPEntity()
	NULL,					// pfnIndexOfEdict()
	NULL,					// pfnPEntityOfEntIndex()
	NULL,					// pfnFindEntityByVars()
	NULL,					// pfnGetModelPtr()

	NULL,					// pfnRegUserMsg()

	NULL,					// pfnAnimationAutomove()
	NULL,					// pfnGetBonePosition()

	NULL,					// pfnFunctionFromName()
	NULL,					// pfnNameForFunction()

	NULL,					// pfnClientPrintf()			//! JOHN: engine callbacks so game DLL can print messages to individual clients
	NULL,					// pfnServerPrint()

	NULL,					// pfnCmd_Args()				//! these 3 added 
	NULL,					// pfnCmd_Argv()				//! so game DLL can easily 
	NULL,					// pfnCmd_Argc()				//! access client 'cmd' strings

	NULL,					// pfnGetAttachment()

	NULL,					// pfnCRC32_Init()
	NULL,					// pfnCRC32_ProcessBuffer()
	NULL,					// pfnCRC32_ProcessByte()
	NULL,					// pfnCRC32_Final()

	NULL,					// pfnRandomLong()
	NULL,					// pfnRandomFloat()

	NULL,					// pfnSetView()
	NULL,					// pfnTime()
	NULL,					// pfnCrosshairAngle()

	NULL,					// pfnLoadFileForMe()
	NULL,					// pfnFreeFile()

	NULL,					// pfnEndSection()				//! trigger_endsection
	NULL,					// pfnCompareFileTime()
	NULL,					// pfnGetGameDir()
	NULL,					// pfnCvar_RegisterVariable()
	NULL,					// pfnFadeClientVolume()
	NULL,					// pfnSetClientMaxspeed()
	NULL,					// pfnCreateFakeClient() 		//! returns NULL if fake client can't be created
	NULL,					// pfnRunPlayerMove()
	NULL,					// pfnNumberOfEntities()

	NULL,					// pfnGetInfoKeyBuffer()		//! passing in NULL gets the serverinfo
	NULL,					// pfnInfoKeyValue()
	NULL,					// pfnSetKeyValue()
	NULL,					// pfnSetClientKeyValue()

	NULL,					// pfnIsMapValid()
	NULL,					// pfnStaticDecal()
	NULL,					// pfnPrecacheGeneric()
	NULL, 					// pfnGetPlayerUserId()			//! returns the server assigned userid for this player.
	NULL,					// pfnBuildSoundMsg()
	NULL,					// pfnIsDedicatedServer()		//! is this a dedicated server?
	NULL,					// pfnCVarGetPointer()
	NULL,					// pfnGetPlayerWONId()			//! returns the server assigned WONid for this player.

	//! YWB 8/1/99 TFF Physics additions
	NULL,					// pfnInfo_RemoveKey()
	NULL,					// pfnGetPhysicsKeyValue()
	NULL,					// pfnSetPhysicsKeyValue()
	NULL,					// pfnGetPhysicsInfoString()
	NULL,					// pfnPrecacheEvent()
	pfnPlaybackEvent,					// pfnPlaybackEvent()

	NULL,					// pfnSetFatPVS()
	NULL,					// pfnSetFatPAS()

	NULL,					// pfnCheckVisibility()

	NULL,					// pfnDeltaSetField()
	NULL,					// pfnDeltaUnsetField()
	NULL,					// pfnDeltaAddEncoder()
	NULL,					// pfnGetCurrentPlayer()
	NULL,					// pfnCanSkipPlayer()
	NULL,					// pfnDeltaFindField()
	NULL,					// pfnDeltaSetFieldByIndex()
	NULL,					// pfnDeltaUnsetFieldByIndex()

	NULL,					// pfnSetGroupMask()

	NULL,					// pfnCreateInstancedBaseline()		// d'oh, CreateInstancedBaseline in dllapi too
	NULL,					// pfnCvar_DirectSet()

	NULL,					// pfnForceUnmodified()

	NULL,					// pfnGetPlayerStats()

	NULL,					// pfnAddServerCommand()

	// Added in SDK 2.2:
	NULL,					// pfnVoice_GetClientListening()
	NULL,					// pfnVoice_SetClientListening()

	// Added for HL 1109 (no SDK update):
	NULL,					// pfnGetPlayerAuthId()

	NULL,					// pfnSequenceGet()
	NULL,					// pfnSequencePickSentence()
	NULL,					// pfnGetFileSize()
	NULL,					// pfnGetApproxWavePlayLen()
	NULL,					// pfnIsCareerMatch()
	NULL,					// pfnGetLocalizedStringLength()
	NULL,					// pfnRegisterTutorMessageShown()
	NULL,					// pfnGetTimesTutorMessageShown()
	NULL,					// pfnProcessTutorMessageDecayBuffer()
	NULL,					// pfnConstructTutorMessageDecayBuffer()
	NULL,					// pfnResetTutorMessageDecayData()
	NULL,					// pfnQueryClientCvarValue()
	NULL,					// pfnQueryClientCvarValue2()
};

enginefuncs_t meta_engfuncs_post = {
	NULL,					// pfnPrecacheModel()
	NULL,					// pfnPrecacheSound()
	NULL,					// pfnSetModel()
	NULL,					// pfnModelIndex()
	NULL,					// pfnModelFrames()

	NULL,					// pfnSetSize()
	NULL,					// pfnChangeLevel()
	NULL,					// pfnGetSpawnParms()
	NULL,					// pfnSaveSpawnParms()

	NULL,					// pfnVecToYaw()
	NULL,					// pfnVecToAngles()
	NULL,					// pfnMoveToOrigin()
	NULL,					// pfnChangeYaw()
	NULL,					// pfnChangePitch()

	NULL,					// pfnFindEntityByString()
	NULL,					// pfnGetEntityIllum()
	NULL,					// pfnFindEntityInSphere()
	NULL,					// pfnFindClientInPVS()
	NULL,					// pfnEntitiesInPVS()

	NULL,					// pfnMakeVectors()
	NULL,					// pfnAngleVectors()

	NULL,					// pfnCreateEntity()
	NULL,					// pfnRemoveEntity()
	NULL,					// pfnCreateNamedEntity()

	NULL,					// pfnMakeStatic()
	NULL,					// pfnEntIsOnFloor()
	NULL,					// pfnDropToFloor()

	NULL,					// pfnWalkMove()
	NULL,					// pfnSetOrigin()

	NULL,					// pfnEmitSound()
	NULL,					// pfnEmitAmbientSound()

	NULL,					// pfnTraceLine()
	NULL,					// pfnTraceToss()
	NULL,					// pfnTraceMonsterHull()
	NULL,					// pfnTraceHull()
	NULL,					// pfnTraceModel()
	NULL,					// pfnTraceTexture()
	NULL,					// pfnTraceSphere()
	NULL,					// pfnGetAimVector()

	NULL,					// pfnServerCommand()
	NULL,					// pfnServerExecute()
	NULL,					// pfnClientCommand()	// d'oh, ClientCommand in dllapi too

	NULL,					// pfnParticleEffect()
	NULL,					// pfnLightStyle()
	NULL,					// pfnDecalIndex()
	NULL,					// pfnPointContents()

	pfnMessageBegin_Post,					// pfnMessageBegin()
	pfnMessageEnd_Post,					// pfnMessageEnd()

	pfnWriteByte_Post,					// pfnWriteByte()
	NULL,					// pfnWriteChar()
	pfnWriteShort_Post,					// pfnWriteShort()
	pfnWriteLong_Post,					// pfnWriteLong()
	NULL,					// pfnWriteAngle()
	pfnWriteCoord_Post,					// pfnWriteCoord()
	pfnWriteString_Post,					// pfnWriteString()
	NULL,					// pfnWriteEntity()

	NULL,					// pfnCVarRegister()
	NULL,					// pfnCVarGetFloat()
	NULL,					// pfnCVarGetString()
	NULL,					// pfnCVarSetFloat()
	NULL,					// pfnCVarSetString()

	NULL,					// pfnAlertMessage()
	NULL,					// pfnEngineFprintf()

	NULL,					// pfnPvAllocEntPrivateData()
	NULL,					// pfnPvEntPrivateData()
	NULL,					// pfnFreeEntPrivateData()

	NULL,					// pfnSzFromIndex()
	NULL,					// pfnAllocString()

	NULL, 					// pfnGetVarsOfEnt()
	NULL,					// pfnPEntityOfEntOffset()
	NULL,					// pfnEntOffsetOfPEntity()
	NULL,					// pfnIndexOfEdict()
	NULL,					// pfnPEntityOfEntIndex()
	NULL,					// pfnFindEntityByVars()
	NULL,					// pfnGetModelPtr()

	NULL,					// pfnRegUserMsg()

	NULL,					// pfnAnimationAutomove()
	NULL,					// pfnGetBonePosition()

	NULL,					// pfnFunctionFromName()
	NULL,					// pfnNameForFunction()

	NULL,					// pfnClientPrintf()			//! JOHN: engine callbacks so game DLL can print messages to individual clients
	NULL,					// pfnServerPrint()

	NULL,					// pfnCmd_Args()				//! these 3 added 
	NULL,					// pfnCmd_Argv()				//! so game DLL can easily 
	NULL,					// pfnCmd_Argc()				//! access client 'cmd' strings

	NULL,					// pfnGetAttachment()

	NULL,					// pfnCRC32_Init()
	NULL,					// pfnCRC32_ProcessBuffer()
	NULL,					// pfnCRC32_ProcessByte()
	NULL,					// pfnCRC32_Final()

	NULL,					// pfnRandomLong()
	NULL,					// pfnRandomFloat()

	NULL,					// pfnSetView()
	NULL,					// pfnTime()
	NULL,					// pfnCrosshairAngle()

	NULL,					// pfnLoadFileForMe()
	NULL,					// pfnFreeFile()

	NULL,					// pfnEndSection()				//! trigger_endsection
	NULL,					// pfnCompareFileTime()
	NULL,					// pfnGetGameDir()
	NULL,					// pfnCvar_RegisterVariable()
	NULL,					// pfnFadeClientVolume()
	NULL,					// pfnSetClientMaxspeed()
	NULL,					// pfnCreateFakeClient() 		//! returns NULL if fake client can't be created
	NULL,					// pfnRunPlayerMove()
	NULL,					// pfnNumberOfEntities()

	NULL,					// pfnGetInfoKeyBuffer()		//! passing in NULL gets the serverinfo
	NULL,					// pfnInfoKeyValue()
	NULL,					// pfnSetKeyValue()
	NULL,					// pfnSetClientKeyValue()

	NULL,					// pfnIsMapValid()
	NULL,					// pfnStaticDecal()
	NULL,					// pfnPrecacheGeneric()
	NULL, 					// pfnGetPlayerUserId()			//! returns the server assigned userid for this player.
	NULL,					// pfnBuildSoundMsg()
	NULL,					// pfnIsDedicatedServer()		//! is this a dedicated server?
	NULL,					// pfnCVarGetPointer()
	NULL,					// pfnGetPlayerWONId()			//! returns the server assigned WONid for this player.

	//! YWB 8/1/99 TFF Physics additions
	NULL,					// pfnInfo_RemoveKey()
	NULL,					// pfnGetPhysicsKeyValue()
	NULL,					// pfnSetPhysicsKeyValue()
	NULL,					// pfnGetPhysicsInfoString()
	NULL,					// pfnPrecacheEvent()
	NULL,					// pfnPlaybackEvent()

	NULL,					// pfnSetFatPVS()
	NULL,					// pfnSetFatPAS()

	NULL,					// pfnCheckVisibility()

	NULL,					// pfnDeltaSetField()
	NULL,					// pfnDeltaUnsetField()
	NULL,					// pfnDeltaAddEncoder()
	NULL,					// pfnGetCurrentPlayer()
	NULL,					// pfnCanSkipPlayer()
	NULL,					// pfnDeltaFindField()
	NULL,					// pfnDeltaSetFieldByIndex()
	NULL,					// pfnDeltaUnsetFieldByIndex()

	NULL,					// pfnSetGroupMask()

	NULL,					// pfnCreateInstancedBaseline()		// d'oh, CreateInstancedBaseline in dllapi too
	NULL,					// pfnCvar_DirectSet()

	NULL,					// pfnForceUnmodified()

	NULL,					// pfnGetPlayerStats()

	NULL,					// pfnAddServerCommand()

	// Added in SDK 2.2:
	NULL,					// pfnVoice_GetClientListening()
	NULL,					// pfnVoice_SetClientListening()

	// Added for HL 1109 (no SDK update):
	NULL,					// pfnGetPlayerAuthId()

	NULL,					// pfnSequenceGet()
	NULL,					// pfnSequencePickSentence()
	NULL,					// pfnGetFileSize()
	NULL,					// pfnGetApproxWavePlayLen()
	NULL,					// pfnIsCareerMatch()
	NULL,					// pfnGetLocalizedStringLength()
	NULL,					// pfnRegisterTutorMessageShown()
	NULL,					// pfnGetTimesTutorMessageShown()
	NULL,					// pfnProcessTutorMessageDecayBuffer()
	NULL,					// pfnConstructTutorMessageDecayBuffer()
	NULL,					// pfnResetTutorMessageDecayData()
	NULL,					// pfnQueryClientCvarValue()
	NULL,					// pfnQueryClientCvarValue2()
};

#endif
