//there should be absolutely no reason to edit or look at this file

#include "plugin.h"
#include <enginecallback.h>		// ALERT()
#include <dllapi.h>
#include <h_export.h>


#include "hookedfunctions_table.h" //all the listed functions we want to intercept
#include "hookedfunctions.h"
#include "utilfunctions.h"

// Description of plugin
plugin_info_t Plugin_info = {
	META_INTERFACE_VERSION,	// ifvers
	PLUGIN_NAME,		// name
	PLUGIN_VERSION,		// version
	PLUGIN_DATE,		// date
	PLUGIN_AUTHOR,		// author
	PLUGIN_URL,		// url
	PLUGIN_CVAR,		// logtag
	PLUGIN_LOADABLE,	// (when) loadable
	PLUGIN_UNLOADABLE,	// (when) unloadable
};

/*C_DLLEXPORT int GetEntityAPI_Post( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion )
{
	LOG_DEVELOPER(PLID, "called: GetEntityAPI_Post; version=%d", interfaceVersion);
	if(!pFunctionTable) {
		LOG_ERROR(PLID, "GetEntityAPI_Post called with null pFunctionTable");
		return(FALSE);
	}
	else if(interfaceVersion != INTERFACE_VERSION) {
		LOG_ERROR(PLID, "GetEntityAPI_Post version mismatch; requested=%d ours=%d", interfaceVersion, INTERFACE_VERSION);
		return(FALSE);
	}

	memcpy(pFunctionTable, &gFunctionTable_Post, sizeof( DLL_FUNCTIONS ) );
	return(TRUE);
}*/

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion)
{
	if ( !pFunctionTable )
	{
		UTIL_LogPrintf("GetEntityAPI2 called with null pFunctionTable");
		return(FALSE);
	} else if ( *interfaceVersion != INTERFACE_VERSION )
	{
		UTIL_LogPrintf("GetEntityAPI2 version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return(FALSE);
	}

	memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));

	g_pFunctionTable = pFunctionTable;

	return(TRUE);
}

C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS* pFunctionTable, int* interfaceVersion)
{
	LOG_DEVELOPER(PLID, "called: GetEntityAPI2_Post; version=%d", *interfaceVersion);
	if ( !pFunctionTable )
	{
		LOG_ERROR(PLID, "GetEntityAPI2_Post called with null pFunctionTable");
		return(FALSE);
	} else if ( *interfaceVersion != INTERFACE_VERSION )
	{
		LOG_ERROR(PLID, "GetEntityAPI2_Post version mismatch; requested=%d ours=%d", *interfaceVersion, INTERFACE_VERSION);
		//! Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return(FALSE);
	}

	memcpy(pFunctionTable, &gFunctionTable_Post, sizeof(DLL_FUNCTIONS));

	g_pFunctionTable_Post = pFunctionTable;

	return(TRUE);
}

C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS* pNewFunctionTable, int* interfaceVersion)
{
	LOG_DEVELOPER(PLID, "called: GetNewDLLFunctions; version=%d", *interfaceVersion);
	if ( !pNewFunctionTable )
	{
		LOG_ERROR(PLID, "GetNewDLLFunctions called with null pNewFunctionTable");
		return(FALSE);
	} else if ( *interfaceVersion != NEW_DLL_FUNCTIONS_VERSION )
	{
		LOG_ERROR(PLID, "GetNewDLLFunctions version mismatch; requested=%d ours=%d", *interfaceVersion, NEW_DLL_FUNCTIONS_VERSION);
		//! Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = NEW_DLL_FUNCTIONS_VERSION;
		return(FALSE);
	}

	memcpy(pNewFunctionTable, &gNewFunctionTable, sizeof(NEW_DLL_FUNCTIONS));
	return TRUE;
}


C_DLLEXPORT int GetNewDLLFunctions_Post(NEW_DLL_FUNCTIONS* pNewFunctionTable, int* interfaceVersion)
{
	LOG_DEVELOPER(PLID, "called: GetNewDLLFunctions_Post; version=%d", *interfaceVersion);
	if ( !pNewFunctionTable )
	{
		LOG_ERROR(PLID, "GetNewDLLFunctions_Post called with null pNewFunctionTable");
		return(FALSE);
	} else if ( *interfaceVersion != NEW_DLL_FUNCTIONS_VERSION )
	{
		LOG_ERROR(PLID, "GetNewDLLFunctions_Post version mismatch; requested=%d ours=%d", *interfaceVersion, NEW_DLL_FUNCTIONS_VERSION);
		//! Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = NEW_DLL_FUNCTIONS_VERSION;
		return(FALSE);
	}

	memcpy(pNewFunctionTable, &gNewFunctionTable_Post, sizeof(NEW_DLL_FUNCTIONS));
	return TRUE;
}

C_DLLEXPORT int GetEngineFunctions(enginefuncs_t* pengfuncsFromEngine, int* interfaceVersion)
{
	LOG_DEVELOPER(PLID, "called: GetEngineFunctions; version=%d", *interfaceVersion);
	if ( !pengfuncsFromEngine )
	{
		LOG_ERROR(PLID, "GetEngineFunctions called with null pengfuncsFromEngine");
		return(FALSE);
	} else if ( *interfaceVersion != ENGINE_INTERFACE_VERSION )
	{
		LOG_ERROR(PLID, "GetEngineFunctions version mismatch; requested=%d ours=%d", *interfaceVersion, ENGINE_INTERFACE_VERSION);
		// Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = ENGINE_INTERFACE_VERSION;
		return(FALSE);
	}

	memcpy(pengfuncsFromEngine, &meta_engfuncs, sizeof(enginefuncs_t));

	g_pengfuncsTable = pengfuncsFromEngine;

	return TRUE;
}

C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t* pengfuncsFromEngine, int* interfaceVersion)
{
	LOG_DEVELOPER(PLID, "called: GetEngineFunctions_Post; version=%d", *interfaceVersion);
	if ( !pengfuncsFromEngine )
	{
		LOG_ERROR(PLID, "GetEngineFunctions_Post called with null pengfuncsFromEngine");
		return(FALSE);
	} else if ( *interfaceVersion != ENGINE_INTERFACE_VERSION )
	{
		LOG_ERROR(PLID, "GetEngineFunctions_Post version mismatch; requested=%d ours=%d", *interfaceVersion, ENGINE_INTERFACE_VERSION);
		// Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = ENGINE_INTERFACE_VERSION;
		return(FALSE);
	}

	memcpy(pengfuncsFromEngine, &meta_engfuncs_post, sizeof(enginefuncs_t));

	g_pengfuncsTable_Post = pengfuncsFromEngine;

	return TRUE;
}


// Metamod requesting info about this plugin
//  ifvers			(given) interface_version metamod is using
//  pPlugInfo		(requested) struct with info about plugin
//  pMetaUtilFuncs	(given) table of utility functions provided by metamod
C_DLLEXPORT int Meta_Query(char* ifvers, plugin_info_t** pPlugInfo,
	mutil_funcs_t* pMetaUtilFuncs)
{
	if ( (int)CVAR_GET_FLOAT("developer") != 0 )
		UTIL_LogPrintf("[%s] dev: called: Meta_Query; version=%s, ours=%s\n",
			Plugin_info.logtag, ifvers, Plugin_info.ifvers);

	// Check for valid pMetaUtilFuncs before we continue.
	if ( !pMetaUtilFuncs )
	{
		UTIL_LogPrintf("[%s] ERROR: Meta_Query called with null pMetaUtilFuncs\n", Plugin_info.logtag);
		return(FALSE);
	}
	gpMetaUtilFuncs = pMetaUtilFuncs;

	// Give metamod our plugin_info struct.
	*pPlugInfo = &Plugin_info;

	// Check for interface version compatibility.
	if ( !FStrEq(ifvers, Plugin_info.ifvers) )
	{
		int mmajor = 0, mminor = 0, pmajor = 0, pminor = 0;
		LOG_MESSAGE(PLID, "WARNING: meta-interface version mismatch; requested=%s ours=%s",
			Plugin_info.logtag, ifvers);
		// If plugin has later interface version, it's incompatible (update
		// metamod).
		sscanf(ifvers, "%d:%d", &mmajor, &mminor);
		sscanf(META_INTERFACE_VERSION, "%d:%d", &pmajor, &pminor);
		if ( pmajor > mmajor || (pmajor == mmajor && pminor > mminor) )
		{
			LOG_ERROR(PLID, "metamod version is too old for this plugin; update metamod");
			return(FALSE);
		}
		// If plugin has older major interface version, it's incompatible
		// (update plugin).
		else if ( pmajor < mmajor )
		{
			LOG_ERROR(PLID, "metamod version is incompatible with this plugin; please find a newer version of this plugin");
			return(FALSE);
		}
		// Minor interface is older, but this is guaranteed to be backwards
		// compatible, so we warn, but we still accept it.
		else if ( pmajor == mmajor && pminor < mminor )
			LOG_MESSAGE(PLID, "WARNING: metamod version is newer than expected; consider finding a newer version of this plugin");
		else
			LOG_ERROR(PLID, "unexpected version comparison; metavers=%s, mmajor=%d, mminor=%d; plugvers=%s, pmajor=%d, pminor=%d", ifvers, mmajor, mminor, META_INTERFACE_VERSION, pmajor, pminor);
	}

	return(TRUE);
}

// Metamod attaching plugin to the server.
//  now				(given) current phase, ie during map, during changelevel, or at startup
//  pFunctionTable	(requested) table of function tables this plugin catches
//  pMGlobals		(given) global vars from metamod
//  pGamedllFuncs	(given) copy of function tables from game dll
C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS* pFunctionTable,
	meta_globals_t* pMGlobals, gamedll_funcs_t* pGamedllFuncs)
{
	if ( now > Plugin_info.loadable )
	{
		LOG_ERROR(PLID, "Can't load plugin right now");
		return(FALSE);
	}
	if ( !pMGlobals )
	{
		LOG_ERROR(PLID, "Meta_Attach called with null pMGlobals");
		return(FALSE);
	}
	gpMetaGlobals = pMGlobals;
	if ( !pFunctionTable )
	{
		LOG_ERROR(PLID, "Meta_Attach called with null pFunctionTable");
		return(FALSE);
	}


	META_FUNCTIONS gMetaFunctionTable = {
		NULL,						// pfnGetEntityAPI				HL SDK; called before game DLL
		NULL,						// pfnGetEntityAPI_Post			META; called after game DLL
		NULL,						// pfnGetEntityAPI2				HL SDK2; called before game DLL
		NULL,						// pfnGetEntityAPI2_Post		META; called after game DLL
		NULL,						// pfnGetNewDLLFunctions		HL SDK2; called before game DLL
		NULL,						// pfnGetNewDLLFunctions_Post	META; called after game DLL
		NULL,						// pfnGetEngineFunctions		META; called before HL engine
		NULL,						// pfnGetEngineFunctions_Post	META; called after HL engine
	};



	//START - Analyzing our function tables to eliminate unused ones
	void** pMem;
	void** pStop;

	//gFunctionTable
	pStop = (void**)(((unsigned char*)&gFunctionTable) + sizeof(DLL_FUNCTIONS));
	for ( pMem = (void**)&gFunctionTable; pMem != pStop; ++pMem )
	{
		if ( pMem != NULL )
		{
			gMetaFunctionTable.pfnGetEntityAPI2 = GetEntityAPI2;
			break;
		}
	}

	//gFunctionTable_Post
	pStop = (void**)(((unsigned char*)&gFunctionTable_Post) + sizeof(DLL_FUNCTIONS));
	for ( pMem = (void**)&gFunctionTable_Post; pMem != pStop; ++pMem )
	{
		if ( pMem != NULL )
		{
			gMetaFunctionTable.pfnGetEntityAPI2_Post = GetEntityAPI2_Post;
			break;
		}
	}

	//gNewFunctionTable
	pStop = (void**)(((unsigned char*)&gNewFunctionTable) + sizeof(NEW_DLL_FUNCTIONS));
	for ( pMem = (void**)&gNewFunctionTable; pMem != pStop; ++pMem )
	{
		if ( pMem != NULL )
		{
			gMetaFunctionTable.pfnGetNewDLLFunctions = GetNewDLLFunctions;
			break;
		}
	}

	//gNewFunctionTable_Post
	pStop = (void**)(((unsigned char*)&gNewFunctionTable_Post) + sizeof(NEW_DLL_FUNCTIONS));
	for ( pMem = (void**)&gNewFunctionTable_Post; pMem != pStop; ++pMem )
	{
		if ( pMem != NULL )
		{
			gMetaFunctionTable.pfnGetNewDLLFunctions_Post = GetNewDLLFunctions_Post;
			break;
		}
	}

	//meta_engfuncs
	pStop = (void**)(((unsigned char*)&meta_engfuncs) + sizeof(enginefuncs_t));
	for ( pMem = (void**)&meta_engfuncs; pMem != pStop; ++pMem )
	{
		if ( pMem != NULL )
		{
			gMetaFunctionTable.pfnGetEngineFunctions = GetEngineFunctions;
			break;
		}
	}

	//meta_engfuncs_post
	pStop = (void**)(((unsigned char*)&meta_engfuncs_post) + sizeof(enginefuncs_t));
	for ( pMem = (void**)&meta_engfuncs_post; pMem != pStop; ++pMem )
	{
		if ( pMem != NULL )
		{
			gMetaFunctionTable.pfnGetEngineFunctions_Post = GetEngineFunctions_Post;
			break;
		}
	}
	//END - Analyzing our function tables to eliminate unused ones



	memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));
	gpGamedllFuncs = pGamedllFuncs;

	GET_HOOK_TABLES(PLID, &g_pEngTable, NULL, NULL);

	//MPlugin MPlug_class;
	//int dist = (int)(&MPlug_class.gamedll_funcs) - (int)(MPlug_class.filename);
	// get position of chararray that stores the location of our dll
	// the path that is stored in metamod plugins.ini
	// char *filename = (char *)((int)(&*pGamedllFuncs) - (PATH_MAX+1) - (MAX_DESC_LEN+1) - sizeof(char) - (PATH_MAX+1));

	return(TRUE);
}

// Metamod detaching plugin from the server.
// now		(given) current phase, ie during map, etc
// reason	(given) why detaching (refresh, console unload, forced unload, etc)
C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason)
{
	if ( now > Plugin_info.unloadable && reason != PNL_CMD_FORCED )
	{
		LOG_ERROR(PLID, "Can't unload plugin right now");
		return(FALSE);
	}

	plugin_quit();

	return(TRUE);
}

// Receive engine function table from engine.
// This appears to be the _first_ DLL routine called by the engine, so we
// do some setup operations here.
void WINAPI GiveFnptrsToDll(enginefuncs_t* pengfuncsFromEngine, globalvars_t* pGlobals)
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;
	plugin_init();
}


