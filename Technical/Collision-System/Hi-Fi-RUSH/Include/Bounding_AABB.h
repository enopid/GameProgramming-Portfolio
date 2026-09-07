#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagAABBDesc final : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents = { 0.f, 0.f, 0.f };
	}AABB_DESC;
private:
	CBounding_AABB();
	virtual ~CBounding_AABB() = default;
public:
	const BoundingBox* Get_Desc() const { return m_pDesc; }

public:
	virtual HRESULT Initialize(const BOUNDING_DESC* pDesc)					override;
	virtual HRESULT Initialize(const  vector<_float3>& vecVTXPositions)		override;
	virtual void	Update(_fmatrix WorldMatrix)							override;
	virtual _bool	Intersect(CBounding* pTarget)							override;
	virtual _bool	IntersectRay(const RAY& ray, _float& outDist)			override;
	CONTACTMF_DESC	GetManifold(CBounding* pTarget)							override;

	class CBounding_OBB* Convert();
	virtual void UpdateWithBone(class CBone* pSrcBone, class CBone* pDstBone) override;
	virtual _vector GetCenter() override;
public:
	virtual HRESULT AddDebugRenderCall(_uint iDebugMask) override;
	_vector ComputeMin();
	_vector ComputeMax();
private:
	CONTACTMF_DESC	ComputeManifold(class CBounding_AABB*	pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_OBB*	pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_Sphere*	pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_Capsule*	pTarget);

private:
	BoundingBox* m_pOriginalDesc = {};
	BoundingBox* m_pDesc = {};

public:
	static CBounding_AABB* Create(const BOUNDING_DESC* pDesc);
	static CBounding_AABB* Create(const  vector<_float3>& vecVTXPositions);
	virtual void Free() override;

	// CBounding을(를) 통해 상속됨
	HRESULT Render_Inspector(CBounding::BOUNDING_DESC* pDesc) override;
};

NS_END

