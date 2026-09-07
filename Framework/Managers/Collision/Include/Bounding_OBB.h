#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	typedef struct tagOBBDesc final : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents	= { 0.f, 0.f, 0.f };
		_float3		vAngles		= { 0.f, 0.f, 0.f };
	}	OBB_DESC;
private:
	CBounding_OBB();
	virtual ~CBounding_OBB() = default;
public:
	const BoundingOrientedBox* Get_Desc() const { return m_pDesc; }
public:
	virtual HRESULT Initialize(const BOUNDING_DESC* pDesc)					override;
	virtual HRESULT Initialize(const  vector<_float3>& vecVTXPositions)		override;
	virtual HRESULT Initialize(BoundingOrientedBox* _pDesc);
	virtual void	Update(_fmatrix WorldMatrix)							override;
	virtual _bool	Intersect(CBounding* pTarget)							override;
	virtual _bool	IntersectRay(const RAY& ray, _float& outDist)			override;
	CONTACTMF_DESC	GetManifold(CBounding* pTarget)							override;
	virtual void UpdateWithBone(class CBone* pSrcBone, class CBone* pDstBone) override;
	virtual _vector GetCenter() override;
public:
	virtual HRESULT AddDebugRenderCall(_uint iDebugMask) override;
	array<_vector, 3> ComputeAxis();
private:
	CONTACTMF_DESC	ComputeManifold(class CBounding_AABB*	pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_OBB*	pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_Sphere*	pTarget);
private:
	BoundingOrientedBox* m_pOriginalDesc	= {};
	BoundingOrientedBox* m_pDesc			= {};
public:
	static CBounding_OBB*	Create(const BOUNDING_DESC* pDesc);
	static CBounding_OBB*	Create(const  vector<_float3>& vecVTXPositions);
	static CBounding_OBB*	Create(BoundingOrientedBox* _pDesc);
	virtual void Free() override;
	HRESULT Render_Inspector(CBounding::BOUNDING_DESC* pDesc) override;

};

NS_END

