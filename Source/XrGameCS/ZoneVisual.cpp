#include "stdafx.h"
#include "CustomZone.h"
//#include "../XrEngine/Render/KinematicsAnimated.h"
#include "../XrEngine/Render/KinematicsAnimated.h"
#include "ZoneVisual.h"
#include "xrServer_Objects_ALife_Monsters.h"
//#include "../XrEngine/Render/KinematicsAnimated.h"
#include "../XrEngine/Render/RenderVisual.h"
CVisualZone::CVisualZone						()
{
}
CVisualZone::~CVisualZone						()
{

}

BOOL CVisualZone::	net_Spawn						(CSE_Abstract* DC)
{
	if (!inherited::net_Spawn(DC))
		return					(FALSE);

	CSE_Abstract				*e  = (CSE_Abstract*)(DC);
	CSE_ALifeZoneVisual			*Z  = smart_cast<CSE_ALifeZoneVisual*>(e);
	IKinematicsAnimated			*SA = CastToIKinematicsAnimated(Visual());
	m_attack_animation			= SA->ID_Cycle_Safe(Z->attack_animation);
	R_ASSERT2				(
		m_attack_animation.valid(),
		make_string(
			"object[%s]: cannot find attack animation[%s] in model[%s]",
			cName().c_str(),
			Z->attack_animation.c_str(),
			cNameVisual().c_str()
		)
	) ;

	m_idle_animation			= SA->ID_Cycle_Safe(Z->startup_animation);
	R_ASSERT2				(
		m_idle_animation.valid(),
		make_string(
			"object[%s]: cannot find startup animation[%s] in model[%s]",
			cName().c_str(),
			Z->startup_animation.c_str(),
			cNameVisual().c_str()
		)
	) ;

	SA->PlayCycle				(m_idle_animation);

	setVisible					(TRUE);

	return						(TRUE);
}

void CVisualZone::SwitchZoneState(EZoneState new_state)
{
	if(m_eZoneState==eZoneStateBlowout && new_state != eZoneStateBlowout)
	{
	//	IKinematicsAnimated*	SA=CastToIKinematicsAnimated(Visual());
		CastToIKinematicsAnimated(Visual())->PlayCycle(m_idle_animation);
	}

	inherited::SwitchZoneState(new_state);

}
void CVisualZone::Load(LPCSTR section)
{
	inherited::Load(section);
	m_dwAttackAnimaionStart		=pSettings->r_u32(section,"attack_animation_start");
	m_dwAttackAnimaionEnd		=pSettings->r_u32(section,"attack_animation_end");
	VERIFY2(m_dwAttackAnimaionStart<m_dwAttackAnimaionEnd,"attack_animation_start must be less then attack_animation_end");
}

void CVisualZone::UpdateBlowout()
{
	inherited::UpdateBlowout();
	if(m_dwAttackAnimaionStart >=(u32)m_iPreviousStateTime && 
		m_dwAttackAnimaionStart	<(u32)m_iStateTime)
				CastToIKinematicsAnimated(Visual())->PlayCycle(m_attack_animation);
		
	if(m_dwAttackAnimaionEnd >=(u32)m_iPreviousStateTime && 
		m_dwAttackAnimaionEnd	<(u32)m_iStateTime)
				CastToIKinematicsAnimated(Visual())->PlayCycle(m_idle_animation);
}