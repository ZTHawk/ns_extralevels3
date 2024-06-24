#ifndef _EXP_CONTROLLER_H_
#define _EXP_CONTROLLER_H_

#include "plugin.h"
#include "upgrade_data_base.h"
#include <vector>
using namespace std;

#define RECONNECT_TIME			180.0
#define EXP_CONTROLLER_CHECK_TIME	10.0
#define STEAMID_LENGTH			10
#define RECONNECT_EXPLOIT_TIME		5.0

class EXP_Controller
{
	public:
	void init( );
	void put_in_server( edict_t *pEntity );
	void disconnect( edict_t *pEntity );
	void join_team( edict_t *player );
	void Think( );
	
	private:
	typedef struct EXP_Data_s
	{
		float exp;
		float next_reconnect_time;
		byte team;
		byte pTeam;
		char SteamID[32];
	}EXP_Data_t;
	
	vector<EXP_Data_t> exp_data;
	
	float next_think;
	
};

extern EXP_Controller exp_controller;

#endif
