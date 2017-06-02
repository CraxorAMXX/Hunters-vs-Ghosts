#include <amxmodx>
#include <fakemeta>
#include <cstrike>
#include <engine>
#include <fun>
#include <hamsandwich>
#include <debug>

new const PLUGIN_NAME[] = "Ghosts vs Hunters";
new const VERSION_NUM[] = "1.0.0";
new const AUTHOR_NAME[] = "Craxor";


new const PlayerClass[] = "player";


#define is_user_ghost(%0)	( cs_get_user_team(%0) == CS_TEAM_T )
const ButtonBits = ( IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT );

public plugin_init( )
{
	register_plugin
	(
		PLUGIN_NAME,
		VERSION_NUM,
		AUTHOR_NAME
	);

	RegisterHam( Ham_Spawn, PlayerClass, "ham_Spawn_EV", 1 );
	register_forward( FM_CmdStart , "fw_FMCmdStart" );

	register_clcmd( "say /test", "test" );
}

public test( id )
{ 
	debug_log( 1, "Test called" );
	set_user_godmode(id, 1 );
	debug_log( 1,"Is user godmode? %i", get_user_godmode(id) );
}

public ham_Spawn_EV( id )
{
	if( is_user_alive( id ) )
	{
		strip_user_weapons( id );
		give_item( id, "weapon_knife" );

		set_user_godmode( id, is_user_ghost(id) ? 1 : 0 );
		set_user_health( id, is_user_ghost(id) ? 10 : 125 );

		if( !is_user_ghost(id) )
		{
			give_user_weapon( id, CSW_USP, 12, 400 );
			give_user_weapon( id, CSW_M4A1, 30, 800 );
			give_user_weapon( id, CSW_HEGRENADE, 2 );
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
