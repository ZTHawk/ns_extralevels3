
#include "upgrade_etherealshift.h"
#include "utilfunctions.h"
#include "player_data.h"
#include "ns_const.h"

Upgrade_Etherealshift data_etherealshift;
EL_Etherealshift player_etherealshift[MAX_PLAYERS_PLUS1];

void Upgrade_Etherealshift::init( )
{
	Config_data upgrade_data[] =
	{
		{&(available = ETHEREALSHIFT), TYPE_BOOL, (char *)ES_config_names[0]},
		{&(req_points = ES_COST), TYPE_INT, (char *)ES_config_names[1]},
		{&(max_level = ES_MAX), TYPE_INT, (char *)ES_config_names[2]},
		{&(req_level = ES_LEVEL), TYPE_INT, (char *)ES_config_names[3]},
		{&(ShiftInitial = ES_INITIAL), TYPE_FLOAT, (char *)ES_config_names[4]},
		{&(ShiftLevel = ES_LVLTIME), TYPE_FLOAT, (char *)ES_config_names[5]},
		{&(ShiftClassMultiplier_percent = ES_CLASSMULTI), TYPE_FLOAT, (char *)ES_config_names[6]},
		{&(OnosShift = ES_ONOSSHIFT), TYPE_BOOL, (char *)ES_config_names[7]},
		{&(ShiftDelay = ES_DELAY), TYPE_FLOAT, (char *)ES_config_names[8]}
	};
	
	UTIL_getUpgradeDataFromFile(upgrade_data, ARRAYSIZE(upgrade_data));
	
	strcpy(upgrade_name, "Ethereal Shift");
	strcpy(upgrade_description, "Shifts you to ethereal state making you invisible until you attack, or your time runs out\n"
					"Press flashlight key to activate ( costs energy! )\n"
					"Max shift time of [%3.2f] second%s (for current life form)\n\n"
					"Requires: Adrenaline , Cloaking , Level %d, %d Point%s\n\n"
					"Next level [%d]\n\n"
					"%s%s\n\n0. Exit\n\n\n\n\n\n\n\n");
	
	team = ALIEN;
	max_alien_points += available * max_level * req_points;
}

bool Upgrade_Etherealshift::add_to_menu( byte ID , int num , int &Keys , char *menu )
{
	char dummy_string[MENU_OPTION_LEN];
	if ( !available )
	{
		sprintf(dummy_string, "#. %s           (Disabled)\n", upgrade_name);
		//return false;
	}else if ( player_etherealshift[ID].cur_level == max_level )
	{
		sprintf(dummy_string, "#. %s           ( max / %3i )\n", upgrade_name, max_level);
	}else
	{
		Keys |= (1<<(num - 1));
		sprintf(dummy_string, "%d. %s           ( %3i / %3i )\n", num, upgrade_name, player_etherealshift[ID].cur_level, max_level);
	}
	strcat(menu, dummy_string);
	return true;
}

void Upgrade_Etherealshift::show_upgrade_menu( edict_t *pEntity )
{
	byte ID = ENTINDEX(pEntity);
	char menu[UPGRADE_DESCRIPTION_LEN];
	const char *dummy_description = upgrade_description;
	
	float shift_level = ShiftLevel;
	if ( player_data[ID].pClass == CLASS_SKULK
		|| player_data[ID].pClass == CLASS_GORGE
		|| player_data[ID].pClass == CLASS_LERK )
		shift_level += shift_level / 100.0 * ShiftClassMultiplier_percent;
	
	float newShiftTime = ShiftInitial + shift_level * ( player_etherealshift[ID].cur_level + 1 );
	
	bool req_correct = player_etherealshift[ID].check_Requirements();
	sprintf(menu, dummy_description,
			newShiftTime,
			newShiftTime != 1.0 ? "s" : "",
			req_level + player_etherealshift[ID].cur_level,
			req_points,
			req_points > 1 ? "s" : "",
			player_etherealshift[ID].cur_level + 1,
			req_correct ? "2. Buy " : "",
			req_correct ? upgrade_name : ""
			);
	
	int Keys = MENU_KEY_0;
	if ( req_correct
		&& available )
		Keys |= MENU_KEY_2;
	
	UTIL_ShowMenu(pEntity, Keys, -1, menu);
}

void Upgrade_Etherealshift::precache( )
{
	if ( isMvM == true )
		return;
	
	for ( int i = 0; i < ES_MAX_SOUNDS; ++i )
		PRECACHE_SOUND((char *)ES_sound_files[i]);
}

//////////

void EL_Etherealshift::reset( )
{
	cur_level = 0;
	maxShiftTime = 0.0;
	maxShiftTime_FadeOnos = 0.0;
	reset_basic();
}

void EL_Etherealshift::reset_basic( )
{
	Shifting = false;
	endShiftTime = 0.0;
}

bool EL_Etherealshift::check_Requirements( )
{
	return ( player_data[ID].level >= data_etherealshift.req_level + cur_level
		&& player_data[ID].points_available >= data_etherealshift.req_points
		&& cur_level < data_etherealshift.max_level
		&& UTIL_getMask(pEntity, MASK_ADRENALINE)
		&& ( UTIL_getMask(pEntity, MASK_CLOAKING)
			|| Shifting ) );
}

void EL_Etherealshift::buy_upgrade( )
{
	if ( check_Requirements() == false )
		return;
	
	player_data[ID].gestation_Emulation = GESTATE_START;
	
	++cur_level;
	
	set_upgrade_values();
	
	UTIL_addPoints(pEntity, data_etherealshift.req_points);
	
	char Msg[POPUP_MSG_LEN];
	sprintf(Msg, "You got Level %d of %d levels of %s",
							cur_level,
							data_etherealshift.max_level,
							data_etherealshift.upgrade_name);
	UTIL_showPopup(pEntity, Msg);
}

void EL_Etherealshift::set_upgrade_values( )
{
	float shift_level_bonus = data_etherealshift.ShiftLevel
		+ (data_etherealshift.ShiftLevel / 100.0 * data_etherealshift.ShiftClassMultiplier_percent);
	maxShiftTime = data_etherealshift.ShiftInitial + (float)cur_level * shift_level_bonus;
	maxShiftTime_FadeOnos = data_etherealshift.ShiftInitial + (float)cur_level * data_etherealshift.ShiftLevel;
}

void EL_Etherealshift::start_EtherealShift( )
{
	if ( cur_level == 0 )
		return;
	
	if ( gpGlobals->time - endShiftTime < data_etherealshift.ShiftDelay
		|| pEntity->v.fuser3 < SHIFT_ENERGY_COST					// energy
		|| ( data_etherealshift.OnosShift == false && player_data[ID].pClass == CLASS_ONOS ) )
		return;
	
	if ( player_data[ID].pClass == CLASS_SKULK
		|| player_data[ID].pClass == CLASS_GORGE
		|| player_data[ID].pClass == CLASS_LERK
		|| player_data[ID].pClass == CLASS_GESTATE )
		endShiftTime = gpGlobals->time + maxShiftTime;
	else
		endShiftTime = gpGlobals->time + maxShiftTime_FadeOnos;
	
	// disable normal cloak so it will not disturb
	set_private_f(pEntity, MAKE_OFFSET(UNCLOAKTIME), endShiftTime);
	set_private_f(pEntity, MAKE_OFFSET(CLOAK), 1.0);
	UTIL_setMask(pEntity, MASK_CLOAKING, false);
	
	// make player realy invisible
	pEntity->v.rendermode = kRenderTransTexture;	// = 2
	
	Shifting = true;
	pEntity->v.fuser3 -= SHIFT_ENERGY_COST;
	if ( !UTIL_getMask(pEntity, MASK_SILENCE) )
		EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, ES_sound_files[ES_sound_cloakstart], 0.5, ATTN_NORM, 0, PITCH_NORM);
	
	// to force HUD update
	player_data[ID].lastHUD_display = 0.0;
}

void EL_Etherealshift::Think( )
{
	if ( Shifting == false )
		return;
	
	if ( gpGlobals->time < endShiftTime )
	{
		if ( pEntity->v.button & IN_ATTACK )
		{
			switch ( player_data[ID].curWeapon )
			{
				case WEAPON_METABOLIZE:
				case WEAPON_BLINK:
				case WEAPON_LEAP:
				case WEAPON_UMBRA:
				case WEAPON_PRIMALSCREAM:
				{
					return;
				}
			}
		}else if ( pEntity->v.button & IN_ATTACK2 )
		{
			switch ( player_data[ID].pClass )
			{
				case CLASS_SKULK:
				case CLASS_FADE:
				{
					return;
				}
			}
		}else
			return;
	}
	
	pEntity->v.rendermode = kRenderNormal;		// = 0
	UTIL_setMask(pEntity, MASK_CLOAKING, true);
	if ( !UTIL_getMask(pEntity, MASK_SILENCE) )
		EMIT_SOUND_DYN2(pEntity, CHAN_ITEM, ES_sound_files[ES_sound_cloakend], 0.5, ATTN_NORM, 0, PITCH_NORM);
	
	// reset shift time
	endShiftTime = gpGlobals->time;
	
	Shifting = false;
	
	// to force HUD update
	UTIL_resetHUD(pEntity);
	player_data[ID].lastHUD_display = 0.0;
}

bool EL_Etherealshift::setHUDText( byte vID , bool is_marine , hudtextparms_t &hud_params , char *CoreT_GL_reload_Shift_text)
{
	if ( cur_level == 0 )
		return false;
	
	if ( Shifting == false )
		return false;
	
	sprintf(CoreT_GL_reload_Shift_text, "<< Shifting ( Time: %3.1f ) >>\n\n\n\n\n\n\n", endShiftTime - gpGlobals->time);		// 7 * \n
	UTIL_setHUD(hud_params
		, is_marine ? MARINE_HUD_COLOR_R : ALIEN_HUD_COLOR_R
		, is_marine ? MARINE_HUD_COLOR_G : ALIEN_HUD_COLOR_G
		, is_marine ? MARINE_HUD_COLOR_B : ALIEN_HUD_COLOR_B
		, -1.0, UTIL_isAlive(INDEXENT(vID)) ? 0.753 : 0.683, 0, 0.0, 3600.0, 0.0, 0.0, HUD_CHANNEL);
	
	return true;
}



