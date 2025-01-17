#ifndef	_PHYSICS_EXTERNAL_COMMON_
#define _PHYSICS_EXTERNAL_COMMON_
#pragma once
struct dContactGeom;
struct dContact;
class SGameMtl;
namespace	CDB{	class TRI;	}
class		IBoneInstance;

typedef void 	ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
typedef	void	ObjectContactCallbackFun(bool& do_colide,bool bo1,dContact& c,SGameMtl* material_1,SGameMtl* material_2);


typedef void	_BCL	BoneCallbackFun(IBoneInstance* B);





typedef			void	PhysicsStepTimeCallback		(u32	step_start,u32	step_end);
//extern			PhysicsStepTimeCallback		*physics_step_time_callback;
struct dxGeomUserData;
struct dContactGeom;
XRPHYSICS_API	bool ContactShotMarkGetEffectPars(dContactGeom *c, dxGeomUserData* &data, float &vel_cret, bool &b_invert_normal );

template <typename geom_type>
void	t_get_box(const geom_type* shell, const	Fmatrix& form, Fvector& sz, Fvector& c)
{
	c.set(0, 0, 0);
	auto GetExtensionsFrom = [shell, &sz, &c](const Fvector& ax,int i)
	{
		float lo, hi;
		shell->get_Extensions(ax, 0, lo, hi);
		sz[i] = hi - lo; c.add(Fvector().mul(ax, (lo + hi) / 2));
	};
	GetExtensionsFrom(form.i,0);
	GetExtensionsFrom(form.j,1);
	GetExtensionsFrom(form.k,2);
}

enum ERestrictionType
{

	rtStalker						=0	,
	rtStalkerSmall						,
	rtMonsterMedium						,
	rtNone								,
	rtActor
};
#endif

