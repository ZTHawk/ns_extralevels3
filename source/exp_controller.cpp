#include "exp_controller.h"
#include "utilfunctions.h"
#include "player_data.h"

EXP_Controller exp_controller;

void EXP_Controller::init()
{
	exp_data.clear();
	next_think = 0.0;
}

void EXP_Controller::put_in_server(edict_t* pEntity)
{
	int ID = ENTINDEX(pEntity);
	//if ( player_data[ID].isBot == true )
	//	return;

	vector<EXP_Data_t>::iterator iter = exp_data.begin();
	while ( iter != exp_data.end() )
	{
		if ( strcmp(player_data[ID].SteamID, iter->SteamID) != 0 )
		{
			++iter;
			continue;
		}

		// if player reconnects within RECONNECT_EXPLOIT_TIME seconds, he must have used the command "retry"
		// prevent exploiting
		if ( gpGlobals->time <= iter->next_reconnect_time - RECONNECT_TIME + RECONNECT_EXPLOIT_TIME )
			exp_data.erase(iter);

		break;
	}
}

void EXP_Controller::disconnect(edict_t* pEntity)
{
	int ID = ENTINDEX(pEntity);
	//if ( player_data[ID].isBot == true )
	//	return;

	//if ( strlen(player_data[ID].SteamID) < STEAMID_LENGTH )
	//	return;

	if ( strcmp(player_data[ID].SteamID, "HLTV") == 0
		|| strncmp(player_data[ID].SteamID, "STEAM_ID", 8) == 0
		|| strncmp(player_data[ID].SteamID, "VALVE_ID", 8) == 0
		|| strcmp(player_data[ID].SteamID, "4294967295") == 0 )		// this ID is taken from amxX (2^32 - 1)
		return;

	EXP_Data_t new_data;
	new_data.exp = UTIL_getEXP(pEntity);
	new_data.next_reconnect_time = gpGlobals->time + RECONNECT_TIME;
	new_data.team = player_data[ID].team;
	new_data.pTeam = player_data[ID].pTeam;
	strcpy(new_data.SteamID, player_data[ID].SteamID);

	exp_data.push_back(new_data);
}

void EXP_Controller::join_team(edict_t* pEntity)
{
	byte ID = ENTINDEX(pEntity);
	//if ( player_data[ID].isBot == true )
	//	return;

	//if ( strlen(player_data[ID].SteamID) < STEAMID_LENGTH )
	//	return;

	vector<EXP_Data_t>::iterator iter = exp_data.begin();
	while ( iter != exp_data.end() )
	{
		if ( strcmp(player_data[ID].SteamID, iter->SteamID) != 0 )
		{
			++iter;
			continue;
		}

		if ( player_data[ID].team == iter->team
			&& pEntity->v.team == iter->pTeam
			&& gpGlobals->time <= iter->next_reconnect_time )
		{
			UTIL_setEXP(pEntity, iter->exp);
		}

		exp_data.erase(iter);

		break;
	}
}

void EXP_Controller::Think()
{
	if ( gpGlobals->time < next_think )
		return;

	next_think = gpGlobals->time + EXP_CONTROLLER_CHECK_TIME;

	vector<EXP_Data_t>::iterator iter = exp_data.begin();
	while ( iter != exp_data.end() )
	{
		if ( gpGlobals->time > iter->next_reconnect_time )
		{
			iter = exp_data.erase(iter);

			if ( exp_data.empty() == true )
				break;

			continue;
		}

		++iter;
	}
}

