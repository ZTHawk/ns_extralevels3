#ifndef _UPGRADE_DATA_BASE_H_
#define _UPGRADE_DATA_BASE_H_

#include "plugin.h"

#define UPGRADE_DATA \
	public: \
	void init( ); \
	void precache( ); \
	bool add_to_menu( byte ID , int num , int &Keys , char *menu ); \
	void show_upgrade_menu( edict_t *player );

#define UPGRADE_DATA_PLAYER \
	public: \
	inline void init( edict_t *player ) \
	{ \
		ID = ENTINDEX(player); \
		pEntity = player; \
		reset(); \
	}; \
	void Think( ); \
	void reset( ); \
	bool check_Requirements(  ); \
	void buy_upgrade(  ); \
	void set_upgrade_values( );

enum
{
	// Marines
	UP_C = 0,
	UP_RA,
	UP_NA,
	UP_AA,
	UP_SF,
	UP_UA,
	UP_BS,

	// Aliens
	UP_TS,
	UP_ES,
	UP_BL,
	UP_H,
	UP_AV,
	UP_SOA,
	UP_LS,
	UP_CE,

	UP_END
};

#define UP_START		UP_C
#define UP_MARINE_FIRST		UP_C
#define UP_MARINE_LAST		UP_BS
#define UP_ALIEN_FIRST		UP_TS
#define UP_ALIEN_LAST		UP_LS

extern const char* upgrade_symbols[UP_END];

class base_upgrade_data
{
public:
	bool available;
	int req_points;
	int max_level;
	int req_level;
	int team;

	char upgrade_name[UPGRADE_NAME_LEN];
	char upgrade_description[UPGRADE_DESCRIPTION_LEN];

	virtual void init();
	virtual void precache();
	virtual bool add_to_menu(byte ID, int num, int& Keys, char* menu);
	virtual void show_upgrade_menu(edict_t* player);

	virtual ~base_upgrade_data();
};

class base_upgrade_pl_data
{
public:
	int cur_level;
	edict_t* pEntity;
	byte ID;

	virtual void init(edict_t* player);
	virtual void Think();
	virtual void Think_Post();
	virtual void respawned();
	virtual void reset();
	virtual void reset_basic();
	virtual bool check_Requirements();
	virtual void buy_upgrade();
	virtual void set_upgrade_values();

	virtual ~base_upgrade_pl_data();
};

extern base_upgrade_data* upgrade_data[UP_END];
extern base_upgrade_pl_data* upgrade_pl_data[UP_END][MAX_PLAYERS_PLUS1];

#endif
