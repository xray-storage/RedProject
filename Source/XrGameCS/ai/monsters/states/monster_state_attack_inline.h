#pragma once

#include "monster_state_attack_melee.h"
#include "monster_state_attack_run.h"
#include "monster_state_attack_run_attack.h"
#include "state_hide_from_point.h"
#include "monster_state_find_enemy.h"
#include "monster_state_steal.h"
#include "monster_state_attack_camp.h"
#include "monster_state_home_point_attack.h"

#include "../ai_monster_squad.h"
#include "../ai_monster_squad_manager.h"

#include "../../../actor.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterAttackAbstract CStateMonsterAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::CStateMonsterAttack(_Object *obj) : inherited(obj)
{
	this->add_state	(eStateAttack_Run,				xr_new<CStateMonsterAttackRun<_Object> >		(obj));
	this->add_state	(eStateAttack_Melee,			xr_new<CStateMonsterAttackMelee<_Object> >		(obj));
	this->add_state	(eStateAttack_RunAttack,		xr_new<CStateMonsterAttackRunAttack<_Object> >	(obj));
	this->add_state	(eStateAttack_RunAway,			xr_new<CStateMonsterHideFromPoint<_Object> >	(obj));
	this->add_state	(eStateAttack_FindEnemy,		xr_new<CStateMonsterFindEnemy<_Object> >		(obj));
	this->add_state	(eStateAttack_Steal,			xr_new<CStateMonsterSteal<_Object> >			(obj));
	this->add_state	(eStateAttackCamp,				xr_new<CStateMonsterAttackCamp<_Object> >		(obj));
	this->add_state	(eStateAttack_MoveToHomePoint,	xr_new<CStateMonsterAttackMoveToHomePoint<_Object> >(obj));
}

// Lain: added 
TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::CStateMonsterAttack(_Object *obj, state_ptr state_move2home) : inherited(obj)
{
	this->add_state	(eStateAttack_Run,				xr_new<CStateMonsterAttackRun<_Object> >		(obj));
	this->add_state	(eStateAttack_Melee,			xr_new<CStateMonsterAttackMelee<_Object> >		(obj));
	this->add_state	(eStateAttack_RunAttack,		xr_new<CStateMonsterAttackRunAttack<_Object> >	(obj));
	this->add_state	(eStateAttack_RunAway,			xr_new<CStateMonsterHideFromPoint<_Object> >	(obj));
	this->add_state	(eStateAttack_FindEnemy,		xr_new<CStateMonsterFindEnemy<_Object> >		(obj));
	this->add_state	(eStateAttack_Steal,			xr_new<CStateMonsterSteal<_Object> >			(obj));
	this->add_state	(eStateAttackCamp,				xr_new<CStateMonsterAttackCamp<_Object> >		(obj));
	this->add_state	(eStateAttack_MoveToHomePoint,	state_move2home);
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::CStateMonsterAttack(_Object *obj, state_ptr state_run, state_ptr state_melee) : inherited(obj)
{
	this->add_state	(eStateAttack_Run,				state_run);
	this->add_state	(eStateAttack_Melee,			state_melee);
	this->add_state	(eStateAttack_RunAttack,		xr_new<CStateMonsterAttackRunAttack<_Object> >	(obj));
	this->add_state	(eStateAttack_RunAway,			xr_new<CStateMonsterHideFromPoint<_Object> >	(obj));
	this->add_state	(eStateAttack_FindEnemy,		xr_new<CStateMonsterFindEnemy<_Object> >		(obj));
	this->add_state	(eStateAttack_Steal,			xr_new<CStateMonsterSteal<_Object> >			(obj));
	this->add_state	(eStateAttackCamp,				xr_new<CStateMonsterAttackCamp<_Object> >		(obj));
	this->add_state	(eStateAttack_MoveToHomePoint,	xr_new<CStateMonsterAttackMoveToHomePoint<_Object> >(obj));
}

TEMPLATE_SPECIALIZATION
CStateMonsterAttackAbstract::~CStateMonsterAttack()
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::initialize()
{
	inherited::initialize				();
	this->object->MeleeChecker.init_attack	();
	
	m_time_next_run_away				= 0;
	m_time_start_check_behinder			= 0;
	m_time_start_behinder				= 0;
}

#define FIND_ENEMY_DELAY	6000



TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::execute()
{

	bool selected = false;
	
	if (this->check_home_point()) {
		this->select_state	(eStateAttack_MoveToHomePoint);
		selected		= true;
	}else if (this->check_steal_state()) {
		this->select_state	(eStateAttack_Steal);
		selected		= true;
	} else if (this->check_camp_state()) {
		this->select_state	(eStateAttackCamp);
		selected		= true;
	} else if (this->check_find_enemy_state()) {
		this->select_state	(eStateAttack_FindEnemy);
		selected		= true;
	} else if (this->check_run_away_state()) {
		this->select_state	(eStateAttack_RunAway);
		selected		= true;
	} else if (this->check_run_attack_state()) {
		this->select_state	(eStateAttack_RunAttack);
		selected		= true;
	}
	
	if (!selected) {
		// ���������� ��� �����
		bool b_melee = false; 

		if (this->prev_substate == eStateAttack_Melee) {
			if (!this->get_state_current()->check_completion()) {
				b_melee = true;
			}
		} else if (this->get_state(eStateAttack_Melee)->check_start_conditions()) {
			b_melee = true;
		}

		// ���������� ������� ���������
		if (b_melee) {  
			// check if enemy is behind me for a long time
			// [TODO] make specific state and replace run_away state (to avoid ratation jumps)
			if (check_behinder()) 
				this->select_state(eStateAttack_RunAway);
			else 
				this->select_state(eStateAttack_Melee);
		}
		else this->select_state(eStateAttack_Run);
	}

	// clear behinder var if not melee state selected
	if (this->current_substate != eStateAttack_Melee) m_time_start_check_behinder = 0;
	
	this->get_state_current()->execute();
	
	this->prev_substate = this->current_substate;


	// Notify squad	
	CMonsterSquad *squad	= monster_squad().get_squad(this->object);
	if (squad) {
		SMemberGoal			goal;
		
		goal.type			= MG_AttackEnemy;
		goal.entity			= const_cast<CEntityAlive*>(this->object->EnemyMan.get_enemy());
		
		squad->UpdateGoal	(this->object, goal);
	}
	//////////////////////////////////////////////////////////////////////////
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_steal_state()
{
	if (this->prev_substate == u32(-1)) {
		if (this->get_state(eStateAttack_Steal)->check_start_conditions())	return true;
	} else if (this->prev_substate == eStateAttack_Steal) {
		if (!this->get_state(eStateAttack_Steal)->check_completion())		return true;
	}
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_camp_state()
{
	if (this->prev_substate == u32(-1)) {
		if (this->get_state(eStateAttackCamp)->check_start_conditions())	return true;
	} else if (this->prev_substate == eStateAttackCamp) {
		if (!this->get_state(eStateAttackCamp)->check_completion())		return true;
	}
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_find_enemy_state()
{
	// check state find enemy
	if (this->object->EnemyMan.get_enemy_time_last_seen() + FIND_ENEMY_DELAY < Device->dwTimeGlobal) return true;
	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_run_away_state()
{
	if (m_time_start_behinder != 0) return false;

	if (this->prev_substate == eStateAttack_RunAway) {
		if (!this->get_state(eStateAttack_RunAway)->check_completion()) return true;
		else m_time_next_run_away = Device->dwTimeGlobal + 10000;
	} else if ((this->object->EnemyMan.get_enemy() != Actor()) && this->object->Morale.is_despondent() && (m_time_next_run_away < Device->dwTimeGlobal)) {
		return true;
	}

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_run_attack_state()
{
	if (!this->object->ability_run_attack()) return false;

	if (this->prev_substate == eStateAttack_Run) {
		if (this->get_state(eStateAttack_RunAttack)->check_start_conditions())	return true;
	} else if (this->prev_substate == eStateAttack_RunAttack) {
		if (!this->get_state(eStateAttack_RunAttack)->check_completion())			return true;
	}

	return false;
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_home_point()
{
	if (this->prev_substate != eStateAttack_MoveToHomePoint) {
		if (this->get_state(eStateAttack_MoveToHomePoint)->check_start_conditions())	return true;
	} else {
		if (!this->get_state(eStateAttack_MoveToHomePoint)->check_completion())		return true;
	}

	return false;
}


TEMPLATE_SPECIALIZATION
void CStateMonsterAttackAbstract::setup_substates()
{
	state_ptr state = this->get_state_current();

	if (this->current_substate == eStateAttack_RunAway) {
		
		SStateHideFromPoint		data;
		data.point				= this->object->EnemyMan.get_enemy_position();
		data.accelerated		= true;
		data.braking			= false;
		data.accel_type			= eAT_Aggressive;
		data.distance			= 20.f;
		data.action.action		= ACT_RUN;
		data.action.sound_type	= MonsterSound::eMonsterSoundAggressive;
		data.action.sound_delay = this->object->db().m_dwAttackSndDelay;
		data.action.time_out	= 5000;

		state->fill_data_with(&data, sizeof(SStateHideFromPoint));

		return;
	}
}

#define TIME_CHECK_BEHINDER 2000
#define TIME_IN_BEHINDER	3000
#define ANGLE_START_CHECK_BEHINDER		2 * PI_DIV_3
#define ANGLE_CONTINUE_CHECK_BEHINDER	PI_DIV_2

TEMPLATE_SPECIALIZATION
bool CStateMonsterAttackAbstract::check_behinder()
{
	// if we are not in behinder state
	if (m_time_start_behinder == 0) {
		// check if we can start behinder
		
		
		// - check if we start checking 
		if (m_time_start_check_behinder == 0) {
			
			// - check if this->object is behind
			if (!this->object->control().direction().is_face_target(this->object->EnemyMan.get_enemy(), ANGLE_START_CHECK_BEHINDER)) {
				m_time_start_check_behinder = time();
			}

		} else {
			// if we already in check mode
			
			// - check if this->object is not behind (break checker)
			if (this->object->control().direction().is_face_target(this->object->EnemyMan.get_enemy(), ANGLE_CONTINUE_CHECK_BEHINDER)) {
				m_time_start_check_behinder = 0;
			} 

			// check if time is not out
			if (m_time_start_check_behinder + TIME_CHECK_BEHINDER > time())	return false;
			
			m_time_start_behinder		= time();
			m_time_start_check_behinder	= 0;
		}
	} 

	// if we are not in behinder state

	if (m_time_start_behinder != 0) {
		if (m_time_start_behinder + TIME_IN_BEHINDER > time()) return true;	
		else m_time_start_behinder = 0;
	}
	
	return false;
}


#undef TEMPLATE_SPECIALIZATION
#undef CStateMonsterAttackAbstract
