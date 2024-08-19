#ifndef _NS_CONST_ADVANCED_H_
#define _NS_CONST_ADVANCED_H_


#define OFFSET_WIN_WEAPON_ID		324	// 81 * 4	--->>> Check the weapon ID of the weapon ( eg: WEAPON_PISTOL, WEAPON_LMG, ... )
#define OFFSET_LIN_WEAPON_ID		340	// (81+4) * 4	--->>> linux offset

#define OFFSET_WIN_WEAPON_RELOADING	376	// 94 * 4	--->>> this offset is needed to check if weapon is reloading
#define OFFSET_LIN_WEAPON_RELOADING	392	// (94+4) * 4	--->>> linux offset

#define OFFSET_WIN_WEAPON_RELOAD_TIME	504	// 126 * 4	--->>> this offset is needed to check if player is reloading right now or now (allowed to fire his weapon)
#define OFFSET_LIN_WEAPON_RELOAD_TIME	524	// (126+5) * 4	--->>> linux offset

#define OFFSET_WIN_UNCLOAKTIME		1800	// 449 * 4	--->>> this offset sets the time when alien started uncloak (-1.0 when cloaked)
#define OFFSET_LIN_UNCLOAKTIME		1820	// (449+5) * 4	--->>> defines the difference between win32 to linux

#define OFFSET_WIN_CLOAK		1804	// 450 * 4	--->>> this offset is needed to check if player is "cloaked" / "cloak fade in/out" right now or not (int = cloaked or not // float = sequence of cloak fade in/out)
#define OFFSET_LIN_CLOAK		1824	// (450+5) * 4	--->>> defines the difference between win32 to linux

#define OFFSET_WIN_OLD_CLASS		6288	//6260	// 1565 * 4	--->>> this offset is needed to set players old class for gestate emulation
#define OFFSET_LIN_OLD_CLASS		6356	//6280	// (1565+5) * 4 --->>> linux offset

#define OFFSET_WIN_DIGESTING_PLAYER	6492	//6440	// 1610 * 4	--->>> this offset is needed to check which player is currently digested by the onos
#define OFFSET_LIN_DIGESTING_PLAYER	6560	//6460	// (1610+5) * 4	--->>> linux offset

#define OFFSET_WIN_SOUND_EFFECTS	6504	//6452	// 1613 * 4	--->>> this offset is needed to check if the player has sound effects running (eg: when being digested) (at least setting to 0 it disables things like that)
#define OFFSET_LIN_SOUND_EFFECTS	6572	//6472	// (1613+5) * 4	--->>> linux offset


#ifdef __linux__
#define MAKE_OFFSET( name )	OFFSET_LIN_##name
#else
#define MAKE_OFFSET( name )	OFFSET_WIN_##name
#endif


enum
{
	NS_PLAYMODE_UNDEFINED = 0,
	NS_PLAYMODE_READYROOM,
	NS_PLAYMODE_PLAYING,
	NS_PLAYMODE_AWAITINGREINFORCEMENT,		// Player is dead and waiting in line to get back in
	NS_PLAYMODE_REINFORCING,			// Player is in the process of coming back into the game
	NS_PLAYMODE_OBSERVER
};


#endif

