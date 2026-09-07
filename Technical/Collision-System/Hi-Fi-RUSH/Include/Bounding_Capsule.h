#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_Capsule final : public CBounding
{
public:
	typedef struct tagCapsuleDesc final : public CBounding::BOUNDING_DESC
	{
		_float		fRadius = 0.f;
		_float		fHalfHeight = 0.f;
		_float3		vAngles = { 0.f, 0.f, 0.f };
	}	CAPSULE_DESC;
private:
	CBounding_Capsule();
	virtual ~CBounding_Capsule() = default;
public:
	const BoundingCapsule* Get_Desc() const { return m_pDesc; }
public:
	virtual HRESULT Initialize(const BOUNDING_DESC* pDesc)					override;
	virtual HRESULT Initialize(const  vector<_float3>& vecVTXPositions)		override;
	virtual void	Update(_fmatrix WorldMatrix)							override;
	virtual _bool	Intersect(CBounding* pTarget)							override;
	virtual _bool	IntersectRay(const RAY& ray, _float& outDist)			override;
	CONTACTMF_DESC	GetManifold(CBounding* pTarget)							override;
	virtual void UpdateWithBone(class CBone* pSrcBone, class CBone* pDstBone) override;
	
public:
	_vector GetCenter();
	virtual HRESULT AddDebugRenderCall(_uint iDebugMask) override;
private:
	CONTACTMF_DESC	ComputeManifold(class CBounding_AABB*		pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_OBB*		pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_Sphere*		pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_Capsule*	pTarget);

private:
	BoundingCapsule* m_pOriginalDesc = {};
	BoundingCapsule* m_pDesc		 = {};

public:
	static CBounding_Capsule* Create(const BOUNDING_DESC* pDesc);
	static CBounding_Capsule* Create(const  vector<_float3>& vecVTXPositions);
	virtual void Free() override;

	// CBounding을(를) 통해 상속됨
	HRESULT Render_Inspector(CBounding::BOUNDING_DESC* pDesc) override;
};

NS_END

