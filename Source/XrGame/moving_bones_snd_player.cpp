#include	"stdafx.h"

#include	"moving_bones_snd_player.h"

#include "ai_sounds.h"
#include "../XrEngine/Render/Kinematics.h"
#include "../xrEngine/bone.h"

#include "gameobject.h"
#include "../xrphysics/matrix_utils.h"
#ifdef DEBUG
#include "phdebug.h"
#endif
moving_bones_snd_player::moving_bones_snd_player( IKinematics *K, CInifile* ini, LPCSTR section, const Fmatrix &object  ):
	bone_id( BI_NONE ),
	min_factor( -FLT_MAX ),
	max_factor( -FLT_MAX ),
	base_velocity( -FLT_MAX ),
	smothed_velocity( -FLT_MAX ),
	previous_position( Fidentity ),
	kinematics( K )
{
	VERIFY( K );
	VERIFY( ini );
	load( *K, *ini, section, object );
}
moving_bones_snd_player::~moving_bones_snd_player()
{
	sound.Reset();
}
const  Fmatrix& moving_bones_snd_player::bone_matrix( )
{
	VERIFY( kinematics );
	VERIFY( bone_id!= BI_NONE );
	return ( kinematics->LL_GetTransform( bone_id ) );
}
#ifdef DEBUG
BOOL dbg_moving_bones_snd_player = FALSE;
#endif
static const float sm_factor = 0.99f;
static const float play_threthhold = 0.2f; 
void moving_bones_snd_player::update( float time_delta, CGameObject &object )
{
	
	VERIFY( sound.IsValid() );

	Fmatrix new_position;
	new_position.mul_43( object.XFORM(), bone_matrix() );

	Fvector lv, aw;
	matrix_diff( lv, aw, previous_position, new_position, time_delta );

	float aw_speed = aw.magnitude();
	smothed_velocity = smothed_velocity * sm_factor  + aw_speed * ( 1.f - sm_factor );
	
	if( !sound.IsPlaying() )
	{
		if( smothed_velocity > play_threthhold )
			play( object );
		else
			return;
	}


	float factor = smothed_velocity/base_velocity;

	VERIFY( min_factor < max_factor );
	VERIFY( min_factor > 0.f );
	VERIFY( max_factor > 0.f );

//	clamp( factor, min_factor, max_factor ); 
	float frequency_factor = 1.f;
	if( factor > max_factor )
		frequency_factor = max_factor;
	if(factor < min_factor )
		frequency_factor = min_factor;

#ifdef DEBUG
	if( dbg_moving_bones_snd_player )
	{
		DBG_OutText( "smoothed angular speed							 : %f", smothed_velocity );
		//DBG_OutText( "angular speed         : %f",  aw_speed );
		DBG_OutText( "velocity factor = base_speed/smoothed speed=       : %f", factor );
		DBG_OutText( "min_factor=										 : %f", min_factor );
		DBG_OutText( "max_factor=										 : %f", max_factor );
		DBG_OutText( "frequency_factor=									 : %f", frequency_factor );
		//DBG_OutText( "time delta            : %f", time_delta );
		//Msg( "smoothed angular speed: %f, angular speed: %f, factor: %f time delta: %f ", smothed_velocity, aw_speed, factor, time_delta );
	}
#endif
	

	sound.SetFrequency( frequency_factor );
	sound.SetPosition( new_position.c );

	if( smothed_velocity< play_threthhold )
					sound.StopWithFade();


	previous_position.set( new_position );

}


void moving_bones_snd_player::play( CGameObject &O )
{
	VERIFY( sound.IsValid() );
	previous_position.mul_43( O.XFORM(), bone_matrix() );
	sound.Play( &O, O.XFORM().c,true ); 
}

void moving_bones_snd_player::stop( )
{
	VERIFY( sound.IsValid() );
	sound.Stop();
}

moving_bones_snd_player* create_moving_bones_snd_player( CInifile* ini, IKinematics& K, const Fmatrix &obj  )
{
	if( !ini || !ini->section_exist( "moving_bones_snd_player" ) )
		return 0;
	return xr_new<moving_bones_snd_player>( &K, ini, "moving_bones_snd_player", obj );
}

moving_bones_snd_player* create_moving_bones_snd_player( CGameObject &O )
{
	IKinematics *K = CastToIKinematics( O.Visual() );
	VERIFY( K );
	moving_bones_snd_player	*ret = create_moving_bones_snd_player( O.spawn_ini(), *K,  O.XFORM() );
	if(ret)
		return ret;
	return create_moving_bones_snd_player( K->LL_UserData(), *K,  O.XFORM() );
}


void moving_bones_snd_player::load( IKinematics &K, CInifile& ini, LPCSTR section, const Fmatrix &object )
{
	VERIFY( ini.section_exist( section ) );
	kinematics = &K;
	VERIFY( kinematics );

	sound.Create( ini.r_string( section, "sound" ),  SOUND_TYPE_FROM_SOURCE );
	VERIFY( sound.IsValid() );
	bone_id			= kinematics->LL_BoneID( ini.r_string( section, "bone" ) );
	VERIFY( BI_NONE != bone_id );
	min_factor		= ini.r_float(section, "min_factor" );
	max_factor		= ini.r_float(section, "max_factor" );
	base_velocity	= ini.r_float(section, "base_velocity" );

	VERIFY2( min_factor > 0.f, make_string( "moving_bones_snd_player: bad params: min_factor: %f < 0 ", min_factor ) );
	VERIFY2( max_factor > 0.f, make_string( "moving_bones_snd_player: bad params: max_factor: %f < 0 ", max_factor ) );
	VERIFY2( max_factor > 0.f, make_string( "moving_bones_snd_player: bad params: base_velocity: %f < 0 ", base_velocity ) );
	VERIFY2( min_factor <= max_factor, make_string( "moving_bones_snd_player: bad params: min_factor %f > max_factor %f, ", min_factor, max_factor ) );

	smothed_velocity = base_velocity;
	previous_position.set( object );
}