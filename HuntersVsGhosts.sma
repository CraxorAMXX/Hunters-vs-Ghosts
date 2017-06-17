/*
					 @Hunters vs Ghost@
					(www.amxmodx.org/)



*/
#pragma semicolon 1

#include <amxmodx>
#include <fakemeta>
#include <cstrike>
#include <engine>
#include <fun>
#include <hamsandwich>


/*		Const String Variables 		*/

/*  `````````````````````````````````````````````````````````````````````````````````   */
	new const PLUGIN_NAME[] = "Ghosts vs Hunters";
	new const VERSION_NUM[] = "b1.4";
	new const AUTHOR_NAME[] = "Craxor";
	new const PlayerClass[] = "player";
/*  `````````````````````````````````````````````````````````````````````````````````   */



/*		Macross				*/

/*  `````````````````````````````````````````````````````````````````````````````````   */
	#define is_user_ghost(%0)	( cs_get_user_team(%0) == CS_TEAM_T )
/*  `````````````````````````````````````````````````````````````````````````````````   */



/*		Const Interger Variables	*/

/*  `````````````````````````````````````````````````````````````````````````````````   */
	const ButtonBits = ( IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT );
	const Ham:Ham_Player_ResetMaxSpeed = Ham_Item_PreFrame;

/*  `````````````````````````````````````````````````````````````````````````````````   */



/*	  	Cvars				*/

/*  `````````````````````````````````````````````````````````````````````````````````   */
	new Respawn;
	new HunterMaxSpeed;
	new GhostMaxSpeed;
	new HunterHealth;
	new GhostHealth;

/*  `````````````````````````````````````````````````````````````````````````````````   */

public plugin_init( )
{
	register_plugin
	(
		PLUGIN_NAME,
		VERSION_NUM,
		AUTHOR_NAME
	);

	RegisterHam( Ham_Spawn, PlayerClass, "ham_Spawn_EV", 1 );
	RegisterHam( Ham_Killed, PlayerClass, "ham_Killed_EV", 1 );  
	RegisterHam( Ham_Player_ResetMaxSpeed, PlayerClass, "ham_ResetMaxSpeed_EV", 1 );
	RegisterHam( Ham_TakeDamage, PlayerClass, "ham_TakeDamage_EV" );

	register_forward( FM_CmdStart , "fw_FMCmdStart" ); 

	register_touch( "weaponbox", PlayerClass, "touch_func_EV" );
	register_touch( "armoury_entity", PlayerClass, "touch_func_EV" );
	
	Respawn = register_cvar("hvg_allow_respawn", "1" );
	HunterMaxSpeed = register_cvar("hvg_hunter_maxspeed", "400" );
	GhostMaxSpeed = register_cvar("hvg_ghost_maxspeed", "800" );
	HunterHealth = register_cvar("hvg_hunter_health", "125" );
	GhostHealth = register_cvar("hvg_ghost_health", "30" );
}

public plugin_precache() 
{ 
	new Entity = create_entity( "info_map_parameters" );
        
	DispatchKeyValue( Entity, "buying", "3" );
	DispatchSpawn( Entity );
}

public ham_ResetMaxSpeed_EV(id)
{
       	if( is_user_alive(id) )
	{
		set_user_maxspeed( id, is_user_ghost(id) ? float( get_pcvar_num(GhostMaxSpeed) ) : float( get_pcvar_num(HunterMaxSpeed) ) );
	}
} 

public ham_Killed_EV(victim, attacker, shouldgib) 
{
	new Players[32], Num;
	get_players( Players, Num, "e", "CT" );


	if( !is_user_ghost(victim) && Num > 3 )
	{
		cs_set_user_team( victim, CS_TEAM_T, CS_DONTCHANGE );
		
	}
	
	if( get_pcvar_num(Respawn) )
	{
		ExecuteHam( Ham_CS_RoundRespawn, victim );
	}

	return PLUGIN_CONTINUE;
}

public ham_Spawn_EV( id )
{
	if( is_user_alive( id ) )
	{
		strip_user_weapons( id );
		give_item( id, "weapon_knife" );
		set_user_noclip( id, is_user_ghost(id) ? 1 : 0 );

		set_user_health(id, is_user_ghost(id) ? get_pcvar_num(GhostHealth) : get_pcvar_num(HunterHealth) );
		set_user_maxspeed( id, is_user_ghost(id) ? float( get_pcvar_num(GhostMaxSpeed) ) : float( get_pcvar_num(HunterMaxSpeed) ) );

		if( !is_user_ghost(id) )
		{
			give_user_weapon( id, CSW_USP, 12, 400 );
			give_user_weapon( id, CSW_MP5NAVY, 400, 1000 );
	
			give_item( id, "weapon_hegrenade" );
		}
	}
}

bool:isPlayer(i)
{
	new classname[8];
	pev( i, pev_classname, classname, charsmax(classname) );

	return bool:(classname[0]=='p'&&classname[1]=='l'&&classname[3]=='y'&&classname[5]=='r');
}

public ham_TakeDamage_EV( iVictim, iInflictor, iAttacker, Float:fDamage ) 
{
	if ( iInflictor == iAttacker && isPlayer(iAttacker) && is_user_ghost(iAttacker) ) 
	{
		SetHamParamFloat ( 4, fDamage * 2.0 );
		return HAM_HANDLED;
	}
	return HAM_IGNORED;   
}

public touch_func_EV(ent, id)
{
	if( !pev_valid(ent) || !id )
		return -1;

	return is_user_ghost(id) ? PLUGIN_HANDLED : PLUGIN_CONTINUE;
}

public pfn_keyvalue( Entity )  
{ 
	new ClassName[ 20 ], Dummy[ 2 ];
	copy_keyvalue( ClassName, charsmax( ClassName ), Dummy, charsmax( Dummy ), Dummy, charsmax( Dummy ) );
        
	if( equal( ClassName, "info_map_parameters" ) ) 
	{ 
		remove_entity( Entity );
		return PLUGIN_HANDLED ;
	} 
        
	return PLUGIN_CONTINUE;
}

public fw_FMCmdStart( id , handle , seed )
{
	if ( is_user_ghost( id ) && is_user_alive( id ) )
	{
		if( get_uc( handle , UC_Buttons ) & ButtonBits )
 		{
			set_user_rendering(id, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
		}
		else
		{
			set_user_rendering(id, kRenderFxNone, 0, 0, 0, kRenderTransAlpha, 0);
		}
	}

	else
	{
		set_user_rendering(id, kRenderFxNone, 0, 0, 0, kRenderNormal, 0);
	}

}

give_user_weapon( index , iWeaponTypeID , iClip=0 , iBPAmmo=0 , szWeapon[]="" , maxchars=0 )
{
	if ( !( CSW_P228 <= iWeaponTypeID <= CSW_P90 ) || ( iClip < 0 ) || ( iBPAmmo < 0 ) || !is_user_alive( index ) )
		return -1;
	
	new szWeaponName[ 20 ] , iWeaponEntity , bool:bIsGrenade;
	
	const GrenadeBits = ( ( 1 << CSW_HEGRENADE ) | ( 1 << CSW_FLASHBANG ) | ( 1 << CSW_SMOKEGRENADE ) | ( 1 << CSW_C4 ) );
	
	if ( ( bIsGrenade = bool:!!( GrenadeBits & ( 1 << iWeaponTypeID ) ) ) )
		iClip = clamp( iClip ? iClip : iBPAmmo , 1 );
	
	get_weaponname( iWeaponTypeID , szWeaponName , charsmax( szWeaponName ) );
	
	if ( ( iWeaponEntity = user_has_weapon( index , iWeaponTypeID ) ? find_ent_by_owner( -1 , szWeaponName , index ) : give_item( index , szWeaponName ) ) > 0 )
	{
		if ( iWeaponTypeID != CSW_KNIFE )
		{
			if ( iClip && !bIsGrenade )
				cs_set_weapon_ammo( iWeaponEntity , iClip );
		
			if ( iWeaponTypeID == CSW_C4 ) 
				cs_set_user_plant( index , 1 , 1 );
			else
				cs_set_user_bpammo( index , iWeaponTypeID , bIsGrenade ? iClip : iBPAmmo ); 
		}
		
		if ( maxchars )
			copy( szWeapon , maxchars , szWeaponName[7] );
	}
	
	return iWeaponEntity;
}	
