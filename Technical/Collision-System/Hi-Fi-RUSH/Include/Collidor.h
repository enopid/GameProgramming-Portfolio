#pragma once

#include "Component.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "Bounding_Capsule.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCollidor final : public CComponent
{
	friend class CComponent;
	friend class CCollision_Manager;
	
	enum ETESTLVL {
		BROAD,
		MID,
		NARROW,
		END
	};

	union BOUNDINGINFO
	{
		CBounding_AABB::AABB_DESC		m_AABBDesc;
		CBounding_OBB::OBB_DESC			m_OBBDesc;
		CBounding_Sphere::SPHERE_DESC	m_SphereDesc;
		CBounding_Capsule::CAPSULE_DESC m_CapsuleDesc;
	};
	struct tagSingleCollidorDesc
	{
		_float4x4						m_LocalMatrix		= IDENTITYMATRIX;
		_int							m_eCollidorType		= ENUM_TO_UINT(ECOLLIDER::NONE);
		BOUNDINGINFO					m_boundingInfo {};

		_int							m_eBoundingSrcType	= ENUM_TO_UINT(EBOUNDINGSOURCE::CUSTOM);
		_int							m_iMeshIdx			= -1;
		_int							m_iSrcBoneIdx		= -1;
		_int							m_iDstBoneIdx		= -1;
	};
	typedef struct tagCollidorDesc
	{
		tagSingleCollidorDesc			m_collidorDesc[3];
		_int							m_iCollidorIdx = 0;
	} COLLIDOR_DESC;

private:
	CCollidor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollidor(const CCollidor& Prototype);
	virtual ~CCollidor() = default;
private:
	HRESULT SetBoneDrivenSetting(_uint idx);
	HRESULT SetBakedSetting		(_uint idx);
	HRESULT SetCustomSetting	(_uint idx);
protected:
	void Update(float fDeltaTime) override;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Late_Initialize() override;
	HRESULT SetCollidor(const size_t& i);
	const COLLIDOR_DESC* GetDesc();
	_vector Get_Center();
public:
	_bool	Check_Intersection(WPCollidor _wpCollidor, ETESTLVL eLevel);
	void	Compute_Manifolds(WPCollidor _wpCollidor);
	void	Clear_Manifolds();
	void	Set_Func(EKEYACTIONSTATE eState, function<void(const CONTACTMF_DESC&, CCollidor*)> func);
	void	Execute_Func();
	void	TestSolver();

	_bool	RayCast(RAY ray, _float& fOutDist);
public:
	virtual SPComponent Clone(void* pArg)			override;
	virtual void Free()								override;

	void Render_Inspector()							override;
	void Render_SingleCollidor(_uint idx);
	HRESULT Save(void* _pDesc, _uint& _iSize) const	override;
	HRESULT Load(void* _pDesc)						override;
private:
	COLLIDOR_DESC								m_desc;
	class CBounding*							m_pBounding[3] = { nullptr , nullptr , nullptr };
	
	weak_ptr<class CModel>						m_wpModel;
	pair<class CBone*, class CBone*>			m_Bones = { nullptr, nullptr };
	weak_ptr<class CMeshRenderer>				m_wpMeshRenderer;

	map<CCollidor*, CONTACTMF_DESC>				m_mapCurManifolds;
	map<CCollidor*, CONTACTMF_DESC>				m_mapPrevManifolds;

	list<function<void(const CONTACTMF_DESC&, CCollidor*)>>	m_Funcs[3];
};

NS_END