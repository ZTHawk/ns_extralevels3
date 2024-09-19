#ifndef _NS_CONST_ADVANCED_H_
#define _NS_CONST_ADVANCED_H_

// Offsets (used in utilfunctions.cpp)

#ifdef NS_3_2

#define OFFSET_WIN_RESOURCES		1820	//454 * 4
#define OFFSET_LIN_RESOURCES		1840	//459 * 4

#define OFFSET_WIN_WEAPDMG		416	//102 * 4
#define OFFSET_LIN_WEAPDMG		432	//106 * 4

#define OFFSET_WIN_WEAPRANGE		412	//101 * 4
#define OFFSET_LIN_WEAPRANGE		428	//105 * 4

#define OFFSET_WIN_WEAPCLIP		364	//91 * 4
#define OFFSET_LIN_WEAPCLIP		380	//95 * 4

#define OFFSET_WIN_HIVE_TRAIT		488	//122 * 4
#define OFFSET_LIN_HIVE_TRAIT		504	//126 * 4

#define OFFSET_WIN_SCORE		6588	//1647 * 4
#define OFFSET_LIN_SCORE		6608	//1652 * 4

#define OFFSET_WIN_EXP			6512	//1628 * 4
#define OFFSET_LIN_EXP			6532	//1633 * 4

#define OFFSET_WIN_POINTS		6520	//1630 * 4
#define OFFSET_LIN_POINTS		6540	//1635 * 4

#define OFFSET_WIN_AMMO_LMG		1116	//279 * 4
#define OFFSET_LIN_AMMO_LMG		1136	//284 * 4

#define OFFSET_WIN_AMMO_PISTOL		1120	//280 * 4
#define OFFSET_LIN_AMMO_PISTOL		1140	//285 * 4

#define OFFSET_WIN_AMMO_SHOTGUN		1124	//281 * 4
#define OFFSET_LIN_AMMO_SHOTGUN		1144	//286 * 4

#define OFFSET_WIN_AMMO_HMG		1128	//282 * 4
#define OFFSET_LIN_AMMO_HMG		1148	//287 * 4

#define OFFSET_WIN_AMMO_GL		1132	//283 * 4
#define OFFSET_LIN_AMMO_GL		1152	//288 * 4

#define OFFSET_WIN_AMMO_HG		1136	//284 * 4
#define OFFSET_LIN_AMMO_HG		1156	//289 * 4

#define OFFSET_WIN_DEATHS		1380	//345 * 4
#define OFFSET_LIN_DEATHS		1400	//350 * 4

#define OFFSET_WIN_STRUCTOWNER		324	//81 * 4
#define OFFSET_LIN_STRUCTOWNER		340	//85 * 4

#define OFFSET_WIN_HIVEABILITY		6248	//1562 * 4
#define OFFSET_LIN_HIVEABILITY		6268	//1567 * 4

#define OFFSET_WIN_WEAPON_ID		324	// 81 * 4	--->>> Check the weapon ID of the weapon ( eg: NS_WEAPON_PISTOL, NS_WEAPON_LMG, ... )
#define OFFSET_LIN_WEAPON_ID		340	// (81+4) * 4	--->>> linux offset

#define OFFSET_WIN_WEAPON_RELOADING	376	// 94 * 4	--->>> this offset is needed to check if weapon is reloading
#define OFFSET_LIN_WEAPON_RELOADING	392	// (94+4) * 4	--->>> linux offset

#define OFFSET_WIN_WEAPON_RELOAD_TIME	504	// 126 * 4	--->>> this offset is needed to check if player is reloading right now or now (allowed to fire his weapon)
#define OFFSET_LIN_WEAPON_RELOAD_TIME	524	// (126+5) * 4	--->>> linux offset

#define OFFSET_WIN_UNCLOAKTIME		1800	// 449 * 4	--->>> this offset sets the time when alien started uncloak (-1.0 when cloaked)
#define OFFSET_LIN_UNCLOAKTIME		1820	// (449+5) * 4	--->>> defines the difference between win32 to linux

#define OFFSET_WIN_CLOAK		1804	// 450 * 4	--->>> this offset is needed to check if player is "cloaked" / "cloak fade in/out" right now or not (int = cloaked or not // float = sequence of cloak fade in/out)
#define OFFSET_LIN_CLOAK		1824	// (450+5) * 4	--->>> defines the difference between win32 to linux

#define OFFSET_WIN_OLD_CLASS		6260	// 1565 * 4	--->>> this offset is needed to set players old class for gestate emulation
#define OFFSET_LIN_OLD_CLASS		6280	// (1565+5) * 4 --->>> linux offset

#define OFFSET_WIN_DIGESTING_PLAYER	6440	// 1610 * 4	--->>> this offset is needed to check which player is currently digested by the onos
#define OFFSET_LIN_DIGESTING_PLAYER	6460	// (1610+5) * 4	--->>> linux offset

#define OFFSET_WIN_SOUND_EFFECTS	6452	// 1613 * 4	--->>> this offset is needed to check if the player has sound effects running (eg: when being digested) (at least setting to 0 it disables things like that)
#define OFFSET_LIN_SOUND_EFFECTS	6472	// (1613+5) * 4	--->>> linux offset

#else

#define OFFSET_WIN_RESOURCES			1820
#define OFFSET_LIN_RESOURCES			1840

#define OFFSET_WIN_WEAPDMG				416
#define OFFSET_LIN_WEAPDMG				432

#define OFFSET_WIN_WEAPRANGE			412
#define OFFSET_LIN_WEAPRANGE			428

#define OFFSET_WIN_WEAPCLIP				364
#define OFFSET_LIN_WEAPCLIP				380

#define OFFSET_WIN_HIVE_TRAIT			512
#define OFFSET_LIN_HIVE_TRAIT			528

#define OFFSET_WIN_SCORE				6644
#define OFFSET_LIN_SCORE				6744

#define OFFSET_WIN_EXP					6576
#define OFFSET_LIN_EXP					6644

#define OFFSET_WIN_POINTS				6584
#define OFFSET_LIN_POINTS				6652

#define OFFSET_WIN_AMMO_LMG				1116
#define OFFSET_LIN_AMMO_LMG				1136

#define OFFSET_WIN_AMMO_PISTOL			1120
#define OFFSET_LIN_AMMO_PISTOL			1140

#define OFFSET_WIN_AMMO_SHOTGUN			1124
#define OFFSET_LIN_AMMO_SHOTGUN			1144

#define OFFSET_WIN_AMMO_HMG				1128
#define OFFSET_LIN_AMMO_HMG				1148

#define OFFSET_WIN_AMMO_GL				1132
#define OFFSET_LIN_AMMO_GL				1152

#define OFFSET_WIN_AMMO_HG				1136
#define OFFSET_LIN_AMMO_HG				1156

#define OFFSET_WIN_DEATHS				1380
#define OFFSET_LIN_DEATHS				1400

#define OFFSET_WIN_STRUCTOWNER			324
#define OFFSET_LIN_STRUCTOWNER			340

#define OFFSET_WIN_HIVEABILITY			6276
#define OFFSET_LIN_HIVEABILITY			6344

#define OFFSET_WIN_WEAPON_ID			324	// --->>> Check the weapon ID of the weapon ( eg: NS_WEAPON_PISTOL, NS_WEAPON_LMG, ... )
#define OFFSET_LIN_WEAPON_ID			340	// linux offset

#define OFFSET_WIN_WEAPON_RELOADING		376	// this offset is needed to check if weapon is reloading
#define OFFSET_LIN_WEAPON_RELOADING		392	// linux offset

#define OFFSET_WIN_WEAPON_RELOAD_TIME	504	// this offset is needed to check if player is reloading right now or now (allowed to fire his weapon)
#define OFFSET_LIN_WEAPON_RELOAD_TIME	524	// linux offset

#define OFFSET_WIN_UNCLOAKTIME			1800	// this offset sets the time when alien started uncloak (-1.0 when cloaked)
#define OFFSET_LIN_UNCLOAKTIME			1820	// defines the difference between win32 to linux

#define OFFSET_WIN_CLOAK				1804	// this offset is needed to check if player is "cloaked" / "cloak fade in/out" right now or not (int = cloaked or not // float = sequence of cloak fade in/out)
#define OFFSET_LIN_CLOAK				1824	// defines the difference between win32 to linux

#define OFFSET_WIN_OLD_CLASS			6288	// this offset is needed to set players old class for gestate emulation
#define OFFSET_LIN_OLD_CLASS			6356	// linux offset

#define OFFSET_WIN_DIGESTING_PLAYER		6492	// this offset is needed to check which player is currently digested by the onos
#define OFFSET_LIN_DIGESTING_PLAYER		6560	// linux offset

#define OFFSET_WIN_SOUND_EFFECTS		6504	// this offset is needed to check if the player has sound effects running (eg: when being digested) (at least setting to 0 it disables things like that)
#define OFFSET_LIN_SOUND_EFFECTS		6572	// linux offset

#endif // NS version check

#ifdef __linux__
#define MAKE_OFFSET( name )	OFFSET_LIN_##name
#else
#define MAKE_OFFSET( name )	OFFSET_WIN_##name
#endif

#endif
