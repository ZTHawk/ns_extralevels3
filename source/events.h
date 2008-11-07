#ifndef _EVENTS_H_
#define _EVENTS_H_

#include "upgrade_data_base.h"

/*
// Events
void EVENT_DeathMsg( int *Msg_receiver , int Msg_arg_num , int value );
void EVENT_DeathMsg_END( int Msg_receiver );

void EVENT_DamageMsg( int Msg_receiver , int Msg_arg_num , int value );

bool EVENT_HUDText2( int ID , const char *Msg );

void EVENT_CurWeapon( int ID , int value , int Msg_arg_num , bool &Msg_correct_data );

void EVENT_Particles( const char *Name );

// Events POST
bool EVENT_HUDText2_POST( const char *Msg );
void EVENT_HUDText2_END_POST( int ID );

bool EVENT_TeamInfo_POST( int ID , const char *TeamName );
void EVENT_TeamInfo_POST_END( int ID );

void EVENT_SetTech_POST( int ID , int value , int &Msg_arg_num , int &Msg_stored_data );

void EVENT_StatusValue_POST( int ID , int Arg1 , int Arg2 );
void EVENT_StatusValue_END_POST( int ID );

void EVENT_DeathMsg_POST( int *Msg_receiver_Post , int Msg_arg_num_Post , int value );
void EVENT_DeathMsg_END_POST( int Msg_receiver_Post );
*/

// Events
inline bool EVENT_HUDText2( int Msg_receiver , const char *Msg )
{
	if ( strcmp(Msg, "ReadyRoomMessage") != 0 )
		return false;
	
	player_senseofancients[Msg_receiver].ReadyRoom();
	player_data[Msg_receiver].resetData(RESET_INGAME);
	
	return true;
}

inline bool EVENT_HUDText2_POST( const char *Msg )
{
	return ( strcmp(Msg, "ReadyRoomMessage") == 0 );
}

inline void EVENT_HUDText2_END_POST( int Msg_receiver )
{
	if ( player_data[Msg_receiver].join_time_10 < gpGlobals->time )	// at least 10 seconds there
	{
		player_data[Msg_receiver].showNotifyMsg();
	}
}

inline void EVENT_CurWeapon( int Msg_receiver , int value , int Msg_arg_num , bool &Msg_correct_data )
{
	switch ( Msg_arg_num )
	{
		case 1:
		{
			if ( value == CURWEAPON_TO_WEAPON
				|| value == CURWEAPON_NO_WEAPON )
			{
				Msg_correct_data = true;
				return;
			}
			break;
		}
		case 2:
		{
			if ( Msg_correct_data )
			{
				if ( value != player_data[Msg_receiver].curWeapon )
				{
					// weapon changed so old weapon cannot be reloading anymore
					player_advancedammopack[Msg_receiver].reset_basic();
					
					player_senseofancients[Msg_receiver].JustBlinked = false;
					
					player_data[Msg_receiver].curWeapon = value;
					
					// this function has to be after curWeapon was set
					player_advancedammopack[Msg_receiver].fixAnimation();
				}
			}
			break;
		}
	}
	
	Msg_correct_data = false;
}

inline void EVENT_ScoreInfo( int Msg_receiver , int Msg_arg_num )
{
	switch ( Msg_arg_num )
	{
		case SCORE_INFO_LEVEL:
		{
			player_data[Msg_receiver].scoreinfo_data[SCORE_INFO_LEVEL] = player_data[Msg_receiver].level;
			
			break;
		}
	}
}

inline void EVENT_DeathMsg( byte &Msg_receiver , int Msg_arg_num , int &Msg_stored_data , int value )
{
	switch ( Msg_arg_num )
	{
		case DEATH_MSG_ARG_KILLER_ID:
		{
			Msg_stored_data = value;
			break;
		}
		case DEATH_MSG_ARG_VICTIM_ID:
		{
			Msg_receiver = value;
			break;
		}
	}
}

inline void EVENT_DeathMsg_END( int Msg_receiver /*victim*/ , int Msg_stored_data /*killer*/ , char *Msg_stored_string )
{
	CLIENT_COMMAND(INDEXENT(Msg_receiver), "slot10\n");
	
	if ( player_data[Msg_receiver].gestation_Emulation )
	{
		player_data[Msg_receiver].resetGestate();
		player_data[Msg_receiver].died_during_gestate = true;
	}
	
	if ( player_data[Msg_stored_data].ingame == true	// killer connected
		&& ( Msg_receiver != Msg_stored_data		// not suicide
		|| strcmp(Msg_stored_string, "turret") == 0
		|| strcmp(Msg_stored_string, "siegeturret") == 0 ) )
		player_acidicvengeance[Msg_receiver].initAV();
	
	for ( int upgrade_ID = UP_START; upgrade_ID < UP_END; ++upgrade_ID )
		upgrade_pl_data[upgrade_ID][Msg_receiver]->reset_basic();
	
	if ( Msg_stored_data != Msg_receiver	// ignore suicide
		&& player_data[Msg_receiver].pTeam != player_data[Msg_stored_data].pTeam )	// ignore team killing
	{
		player_data[Msg_stored_data].got_xp = true;
		player_data[Msg_stored_data].is_killer_of = Msg_receiver;
	}
}


inline void EVENT_DamageMsg( int Msg_receiver , int Msg_arg_num , int value )
{
	switch ( Msg_arg_num )
	{
		case DAMAGE_MSG_ARG_DMG:
		{
			if ( value > 0 )
			{
				player_bloodlust[Msg_receiver].drink_my_Blood();
				player_senseofancients[Msg_receiver].check_Parasite();
			}
			break;
		}
	}
}

// Events POST
inline void EVENT_DeathMsg_POST( byte &Msg_receiver_Post , int Msg_arg_num_Post , int &Msg_stored_data_Post , int value )
{
	switch ( Msg_arg_num_Post )
	{
		case DEATH_MSG_ARG_KILLER_ID:
		{
			Msg_stored_data_Post = value;
			break;
		}
		case DEATH_MSG_ARG_VICTIM_ID:
		{
			Msg_receiver_Post = value;
			break;
		}
	}
}

inline bool EVENT_TeamInfo_POST( byte Msg_receiver_Post , const char *TeamName )
{
	if ( strcmp(TeamName, "alien1team") == 0
		|| strcmp(TeamName, "alien2team") == 0 )
		player_data[Msg_receiver_Post].team = ALIEN;
	else if ( strcmp(TeamName, "marine1team") == 0
		|| strcmp(TeamName, "marine2team") == 0 )
		player_data[Msg_receiver_Post].team = MARINE;
	else
		player_data[Msg_receiver_Post].team = 0;
	
	return ( player_data[Msg_receiver_Post].team != 0 );
}

inline void EVENT_TeamInfo_END_POST( byte Msg_receiver_Post )
{
	if ( player_data[Msg_receiver_Post].team )
		player_data[Msg_receiver_Post].showNotifyMsg();
}

inline void EVENT_SetTech_POST( byte Msg_receiver_Post , int value , byte Msg_arg_num_Post , int &Msg_stored_data_Post )
{
	switch ( Msg_arg_num_Post )
	{
		case 1:
		{
			Msg_stored_data_Post = value;
			break;
		}
		case 7:	
		{
			if ( value != SETTECH_MSG_ARG_ON )
				break;
			
			switch ( Msg_stored_data_Post )
			{
				case IMPULSE_RESUPPLY:
				{
					player_data[Msg_receiver_Post].got_resupply = true;
					break;
				}
				case IMPULSE_SCAN:
				{
					player_data[Msg_receiver_Post].got_scan = true;
					break;
				}
				case IMPULSE_SHOTGUN:
				case IMPULSE_HMG:
				case IMPULSE_GL:
				{
					player_advancedammopack[Msg_receiver_Post].setWeaponData_Ammo(1);
					player_uraniumammo[Msg_receiver_Post].setWeaponData_Dmg(1);
					break;
				}
				case IMPULSE_GRENADE:
				{
					player_uraniumammo[Msg_receiver_Post].setWeaponData_Dmg(1);
					break;
				}
			}
			break;
		}
	}
}

inline bool EVENT_StatusValue_Byte_POST( int value )
{
	switch ( value )
	{
		case STATUS_VALUE_PLAYER_ID:
		{
			return true;
		}
	}
	return false;
}

inline void EVENT_StatusValue_Short_POST( byte Msg_receiver_Post , int value )
{
	player_data[Msg_receiver_Post].statusvalue_pointing_at = value;
	if ( 1 <= value
		&& value <= gpGlobals->maxClients )
	{
		player_data[Msg_receiver_Post].statusvalue_correct_level_of = value;
	}else
	{
		player_data[Msg_receiver_Post].statusvalue_correct_level_of = 0;
	}
}

inline void EVENT_StatusValue_END_POST( byte Msg_receiver_Post )
{
	if ( player_data[Msg_receiver_Post].statusvalue_correct_level_of )
	{
		MESSAGE_BEGIN(MSG_ONE, StatusValue_ID, NULL, INDEXENT(Msg_receiver_Post));
		WRITE_BYTE(STATUS_VALUE_PLAYER_LEVEL);
		WRITE_SHORT(player_data[player_data[Msg_receiver_Post].statusvalue_correct_level_of].level);
		MESSAGE_END();
	}
}

inline void EVENT_Particles( const char *Name )
{
	if ( Xenocide_Particle_ID == -1
		|| BileBomb_Particle_ID == -1
		|| AcidHit_Particle_ID == - 1)
	{
		for ( int i = 0; i < ParticleCount; ++i )
		{
			// if already found current particle
			if ( strcmp(ParticleNames[i], Name) == 0 )
				return;
		}
		strcpy(ParticleNames[ParticleCount], Name);
		if ( strcmp(Name, "Xenocide") == 0 )
			Xenocide_Particle_ID = ParticleCount;
		else if ( strcmp(Name, "BileBomb") == 0 )
			BileBomb_Particle_ID = ParticleCount;
		else if ( strcmp(Name, "AcidHit") == 0 )
			AcidHit_Particle_ID = ParticleCount;
	}
	++ParticleCount;
}











#endif


