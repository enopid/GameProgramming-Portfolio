#pragma once

#include "Base.h"
#include "BoundingCapsule.h"

NS_BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		_float3			vCenter = { 0.f, 0.f, 0.f};
	}	BOUNDING_DESC;
	
protected:
	CBounding();
	virtual ~CBounding() = default;
protected:
	virtual HRESULT			Initialize	(const BOUNDING_DESC* pDesc)				PURE;
	virtual HRESULT			Initialize	(const  vector<_float3>& vecVTXPositions)	PURE;
public:
	virtual void			Update		(_fmatrix WorldMatrix)						PURE;
	virtual _bool			Intersect	(CBounding* pTarget)						PURE;
	virtual _bool			IntersectRay(const RAY& ray, _float& outDist)			PURE;
	virtual CONTACTMF_DESC 	GetManifold	(CBounding* pTarget)						PURE;
public:
	virtual HRESULT AddDebugRenderCall(_uint iDebugMask)							PURE;
	virtual HRESULT Render_Inspector(CBounding::BOUNDING_DESC* pDesc)				PURE;
	ECOLLIDER GetType() { return m_eType; }
	void SetLocalMatrix(_float4x4* pMatrix);
	virtual void UpdateWithBone(class CBone* pSrcBone, class CBone* pDstBone)		PURE;
	virtual _vector GetCenter() PURE;
public:
	void SetColor(XMVECTORF32	eColor);
	virtual void Free() override;
protected:
	ECOLLIDER	m_eType = ECOLLIDER::END;
	class CGameInstance* m_pGameInstance = nullptr;
	_float4x4*	m_pLocalMatrix	= nullptr;
	_float4x4	m_WorldMatrix	= IDENTITYMATRIX;
	_float3		m_vSrcBonePos;
	_float3		m_vDstBonePos;
	XMVECTORF32	m_eColor = Colors::Black;
};

NS_END