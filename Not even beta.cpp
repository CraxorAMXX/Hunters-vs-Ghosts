#include <amxmodx>
#include <fakemeta>
#include <cstrike>
#include <engine>
#include <fun>
#include <hamsandwich>

new const PLUGIN_NAME[] = "Ghosts vs Hunters";
new const VERSION_NUM[] = "b1.0";
new const AUTHOR_NAME[] = "Craxor";


new const PlayerClass[] = "player";


#define is_user_ghost(%0)	( cs_get_user_team(%0) == CS_TEAM_T )
const ButtonBits = ( IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT );
new Ham:Ham_Player_ResetMaxSpeed = Ham_Item_PreFrame

public plugin_init( )
{
	register_plugin
	(
		PLUGIN_NAME,
		VERSION_NUM,
		AUTHOR_NAME
	);

	RegisterHam( Ham_Spawn, PlayerClass, "ham_Spawn_EV", 1 );
	RegisterHam(Ham_Killed, "player", "ham_Killed_EV", 1)  

	RegisterHam(Ham_Player_ResetMaxSpeed,"player","playerResetMaxSpeed", 1)

	register_forward( FM_CmdStart , "fw_FMCmdStart" );

	register_touch( "weaponbox", PlayerClass, "no_ghost_pickup" );
	register_touch( "armoury_entity", PlayerClass, "no_ghost_pickup" );

	register_clcmd( "chooseteam", "blockch" );
	register_clcmd( "jointeam", "blockch" );
}

public blockch( id ) return PLUGIN_HANDLED;

public plugin_precache() 
{ 
	new Entity = create_entity( "info_map_parameters" );
        
	DispatchKeyValue( Entity, "buying", "3" );
	DispatchSpawn( Entity );
}

public plugin_cfg()
{
	set_cvar_num( "mp_roundtime", "20" );
}

public playerResetMaxSpeed(id)
{
       	if( is_user_alive(id) && is_user_ghost(id) )
	{
		set_user_maxspeed( id, 500.0 );
	}
} 

public ham_Killed_EV(victim, attacker, shouldgib) 
{
	if( is_user_ghost(victim) )
	{
		ExecuteHam( Ham_CS_RoundRespawn, victim );
	}
}

public no_ghost_pickup(ent, id)
{
	if( !pev_valid(ent) || !id )
		return -1;

	if( is_user_ghost(id) )
	{
		return PLUGIN_HANDLED;
	}
	return PLUGIN_CONTINUE;
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

public ham_Spawn_EV( id )
{
	if( is_user_alive( id ) )
	{
		strip_user_weapons( id );
		give_item( id, "weapon_knife" );

		set_user_noclip( id, is_user_ghost(id) ? 1 : 0 );
		set_user_health(id, is_user_ghost(id) ? 10 : 125 );
		set_user_maxspeed( id, is_user_ghost(id) ? 500.0 : 250.0 );

		if( !is_user_ghost(id) )
		{
			give_user_weapon( id, CSW_USP, 12, 400 );
			give_user_weapon( id, CSW_M4A1, 30, 800 );
		}
	}
}

public fw_FMCmdStart( id , handle , seed )
{
	if ( is_user_ghost( id ) && is_user_alive( id ) )
	{
		if( get_uc( handle , UC_Buttons ) & ButtonBits )
 		{
			set_user_rendering(id, kRenderFxNone, 0, 0, 0, kRenderNormal, 0)
		}
		else
		{
			set_user_rendering(id, kRenderFxNone, 0, 0, 0, kRenderTransAlpha, 0)
		}
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
