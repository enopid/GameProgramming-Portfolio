#pragma once

#include "Bounding.h"

NS_BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct tagSphereDesc final : public CBounding::BOUNDING_DESC
	{
		_float		fRadius =1.f;
	}SPHERE_DESC;
private:
	CBounding_Sphere();
	virtual ~CBounding_Sphere() = default;
public:
	const BoundingSphere* Get_Desc() const { return m_pDesc; }
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
	virtual HRESULT AddDebugRenderCall(_uint iDebkougMask) override;
private:
	CONTACTMF_DESC	ComputeManifold(class CBounding_AABB*		pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_OBB*		pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_Sphere*		pTarget);
	CONTACTMF_DESC	ComputeManifold(class CBounding_Capsule*	pTarget);

private:
	BoundingSphere* m_pOriginalDesc = {};
	BoundingSphere* m_pDesc			= {};

public:
	static CBounding_Sphere* Create(const BOUNDING_DESC* pDesc);
	static CBounding_Sphere* Create(const  vector<_float3>& vecVTXPositions);
	virtual void Free() override;

	// CBounding을(를) 통해 상속됨
	HRESULT Render_Inspector(CBounding::BOUNDING_DESC* pDesc) override;
};

NS_END

