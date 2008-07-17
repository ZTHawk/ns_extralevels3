#include "plugin.h"
#include "upgrade_data_base.h"
#include "utilfunctions.h"

const char *upgrade_symbols[UP_END] =
{
	// Marines
	"C",
	"RA",
	"NA",
	"AA",
	"SF",
	"UA",
	"BS",
	
	// Aliens
	"TS",
	"ES",
	"BL",
	"H",
	"AV",
	"SOA",
	"LS"
};

// default functions if they are not overloaded

base_upgrade_data::~base_upgrade_data( )
{

}

void base_upgrade_data::init( )
{
	
}

void base_upgrade_data::precache( )
{
	
}

void base_upgrade_data::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	strcat(menu, "#. Unknown Upgrade. Contact Author.\n");
}

void base_upgrade_data::show_upgrade_menu( edict_t *player )
{
	
}

base_upgrade_pl_data::~base_upgrade_pl_data( )
{
	
}

void base_upgrade_pl_data::init( edict_t *player )
{
	
}

void base_upgrade_pl_data::Think( )
{
	
}

void base_upgrade_pl_data::Think_Post( )
{
	
}

void base_upgrade_pl_data::respawned( )
{
	
}

void base_upgrade_pl_data::reset( )
{
	cur_level = 0;
	pEntity = NULL;
	ID = 0;
}

void base_upgrade_pl_data::reset_basic( )
{
	
}

bool base_upgrade_pl_data::check_Requirements(  )
{
	return false;
}

void base_upgrade_pl_data::buy_upgrade(  )
{
	
}

void base_upgrade_pl_data::set_upgrade_values( )
{
	
}


