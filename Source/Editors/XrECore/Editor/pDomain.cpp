#include "stdafx.h"
#pragma hdrstop

#include "../../Editors/Public/PropertiesListHelper.h"
#include "ParticleEffectActions.h"

using namespace PAPI;

PDomain::PDomain(EType et, BOOL ra, u32 color, PDomainEnum t,	
										float inA0,	float inA1,	float inA2,	
								   		float inA3,	float inA4,	float inA5,
										float inA6,	float inA7,	float inA8	)
{
	flags.set(flRenderable,ra);
	e_type = et;
	type = t;
    clr	 = color;
	f[0] = inA0;
	f[1] = inA1;
	f[2] = inA2;    
	f[3] = inA3;
	f[4] = inA4;
	f[5] = inA5;
	f[6] = inA6;
	f[7] = inA7;
	f[8] = inA8;
}

PDomain::PDomain(const PDomain& inDomain)
{
	e_type 	= inDomain.e_type;
    flags	= inDomain.flags;
	type 	= inDomain.type;
    clr	 	= inDomain.clr;
	f[0]	= inDomain.f[0];
	f[1]	= inDomain.f[1];
	f[2]	= inDomain.f[2];
	f[3]	= inDomain.f[3];
	f[4]	= inDomain.f[4];
	f[5]	= inDomain.f[5];
	f[6]	= inDomain.f[6];
	f[7]	= inDomain.f[7];
	f[8]	= inDomain.f[8];
}

PDomain::~PDomain()
{
}

void PDomain::Load(IReader& F)
{
	type		= PDomainEnum(F.r_u32());
	F.r_fvector3(v[0]);
	F.r_fvector3(v[1]);
	F.r_fvector3(v[2]);
}

void PDomain::Load2(CInifile& ini, const shared_str& sect)
{
	type		= PDomainEnum(ini.r_u32(sect,"type"));
	v[0]		= ini.r_fvector3(sect,"v0");
	v[1]		= ini.r_fvector3(sect,"v1");
	v[2]		= ini.r_fvector3(sect,"v2");
}

void PDomain::Save(IWriter& F)
{
	F.w_u32		(type);
	F.w_fvector3(v[0]);
	F.w_fvector3(v[1]);
	F.w_fvector3(v[2]);
}

void PDomain::Save2(CInifile& ini, const shared_str& sect)
{
	ini.w_u32		(sect.c_str(), "type", type);
	ini.w_fvector3	(sect.c_str(), "v0", v[0]);
	ini.w_fvector3	(sect.c_str(), "v1", v[1]);
	ini.w_fvector3	(sect.c_str(), "v2", v[2]);
}

/*
void	PDomain::set(PAPI::PDomainEnum t, 		float inA0, float inA1, float inA2,
                                                float inA3, float inA4, float inA5, 
                                                float inA6, float inA7, float inA8	)
{
	type 	= t;
	f[0]	= inA0;
	f[1]	= inA1;
	f[2]	= inA2;
	f[3]	= inA3;
	f[4]	= inA4;
	f[5]	= inA5;
	f[6]	= inA6;
	f[7]	= inA7;
	f[8]	= inA8;
}
*/

//--------------------------------------------------------------------
void 	PDomain::Render		(u32 clr, const Fmatrix& parent)
{
	
}

xr_token					domain_token	[ ]={
	{ "Point",				PDPoint		},
	{ "Line",			   	PDLine		},
	{ "Triangle",			PDTriangle	},
	{ "Plane",			   	PDPlane		},
	{ "Box",				PDBox		},
	{ "Sphere",				PDSphere	},
	{ "Cylinder",			PDCylinder	},
	{ "Cone",		   		PDCone		},
	{ "Blob",		   		PDBlob		},
	{ "Disc",		  		PDDisc		},
	{ "Rectangle",	   		PDRectangle	},
	{ 0,					0		   	}
};

void PDomain::OnTypeChange(PropValue* sender)
{               
	
}

void 	PDomain::FillProp	(PropItemVec& items, LPCSTR pref, u32 clr)
{
}

//--------------------------------------------------------------------
/*
void	PDomain::MoveXYZ(float x, float y, float z)
{
	switch(type)
	{
		case PDPoint:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDLine:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDTriangle:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			a6+=x;	a7+=y;	a8+=z;
			break;
		case PDPlane:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDBox:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDSphere:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDCylinder:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDCone:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDBlob:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDDisc:
			a0+=x;	a1+=y;	a2+=z;
			break;
	}
	
}

//----------------------------------------------------------
void	PDomain::RotateXYZ	(float x, float y, float z)
{
	MVertex	v1;
	MVertex v2;
	MVertex v3;
	MVertex	r1;
	MVertex r2;
	MVertex r3;
//	MVertex	tmpV;
	MMatrix	trans;
	
	MVertex	c;
	
	trans.setIdentity();
	trans.rotateX(x);
	trans.rotateZ(z);
	trans.rotateY(y);
//	float	xloc;
//	float	yloc;
//	float	zloc;
	switch(type)
	{
		case PDPoint:
			break;
		case PDLine:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			break;
		case PDTriangle:
			c = GetCenter();
			r1 = trans*(v1.set( (float)a0-c.X(), (float)a1-c.Y(), (float)a2-c.Z() ));
			r2 = trans*(v2.set( (float)a3-c.X(), (float)a4-c.Y(), (float)a5-c.Z() ));
			r3 = trans*(v3.set( (float)a6-c.X(), (float)a7-c.Y(), (float)a8-c.Z() ));
			a0 = r1.X()+c.X();	a1 = r1.Y()+c.Y();   a2 = r1.Z()+c.Z();
			a3 = r2.X()+c.X();	a4 = r2.Y()+c.Y();   a5 = r2.Z()+c.Z();
			a6 = r3.X()+c.X();	a7 = r3.Y()+c.Y();   a8 = r3.Z()+c.Z();
			break;
		case PDPlane:
//			r2 = trans*(v2.set( a3, a4, a5 ));
//			r3 = trans*(v3.set( a6, a7, a8 ));
//			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
//			a6 = r3.X();	a7 = r3.Y();	a8 = r3.Z();
			
			c = GetCenter();
			v1.set(a0-c.X(), a1-c.Y(), a2-c.Z());
			v2.set((float)a0+((float)a3/2.0), (float)a1+((float)a4/2.0), (float)a2+((float)a5/2.0));
			v3.set((float)a0+((float)a6/2.0), (float)a1+((float)a7/2.0), (float)a2+((float)a8/2.0));
			
			r1 = trans*v1;
			r2 = trans*v2;
			r3 = trans*v3;
			
			a0 = r1.X()+c.X();	a1 = r1.Y()+c.Y();   a2 = r1.Z()+c.Z();
			
			
			break;
		case PDBox:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
			break;
		case PDSphere:
			r1 = trans*(v1.set( a0, a1, a2 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			break;
		case PDCylinder:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
			break;
		case PDCone:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
			break;
		case PDBlob:
			r1 = trans*(v1.set( a0, a1, a2 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			break;
		case PDDisc:
			break;
	}
}
//----------------------------------------------------------
void	PDomain::ScaleXYZ	(float x, float y, float z)
{
	x = x;
	y = y;
	z = z;
	
	switch(type)
	{
		case PDPoint:
			break;
		case PDLine:
			break;
		case PDTriangle:
			break;
		case PDPlane:
			break;
		case PDBox:
			break;
		case PDSphere:
			break;
		case PDCylinder:
			break;
		case PDCone:
			break;
		case PDBlob:
			break;
		case PDDisc:
			break;
	}
}

//----------------------------------------------------------
MVertex&	PDomain::GetCenter()
{
	static MVertex center;
	MVertex	v1;
	MVertex	v2;
	MVertex	v3;
	MVertex	v4;
	
	switch(type)
	{
		case PDPoint:
			center.set(a0, a1, a2);
			break;
		case PDLine:
			break;
		case PDTriangle:
			center.set( ((float)a0+(float)a3+(float)a6)/3.0, 
						((float)a1+(float)a4+(float)a7)/3.0, 
						((float)a2+(float)a5+(float)a8)/3.0 );
			break;
		case PDPlane:
			v1.set(a0, a1, a2);
			v2.set(a0+(a3/2.0f), a1+(a4/2.0f), a2+(a5/2.0f));
			v3.set(a0+(a6/2.0f), a1+(a7/2.0f), a2+(a8/2.0f));
			v4.set(a0+(a6/2.0f)+(a3/2.0f), a1+(a7/2.0f)+(a4/2.0f), a2+(a8/2.0f)+(a5/2.0f));
			center.set(	(v1.X()+v2.X()+v3.X()+v4.X())/4.0,
						(v1.Y()+v2.Y()+v3.Y()+v4.Y())/4.0,
						(v1.Z()+v2.Z()+v3.Z()+v4.Z())/4.0 );
			break;
		case PDBox:
			break;
		case PDSphere:
			break;
		case PDCylinder:
			break;
		case PDCone:
			break;
		case PDBlob:
			break;
		case PDDisc:
			center.set(a0, a1, a2);
			break;
	}
	return center;
}
*/
