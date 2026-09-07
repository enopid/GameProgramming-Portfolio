#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "Bounding_Capsule.h"
#include "GameInstance.h"

CBounding_Sphere::CBounding_Sphere()
{

	m_eType = ECOLLIDER::SPHERE;
}

HRESULT CBounding_Sphere::Initialize(const BOUNDING_DESC* pDesc)
{
	const SPHERE_DESC* pSphereDesc = static_cast<const SPHERE_DESC*>(pDesc);

	m_pOriginalDesc = new BoundingSphere(pSphereDesc->vCenter, pSphereDesc->fRadius);
	m_pDesc			= new BoundingSphere(*m_pOriginalDesc);

    return S_OK;
}

HRESULT CBounding_Sphere::Initialize(const vector<_float3>& vecVTXPositions)
{
	BoundingSphere::CreateFromPoints(*m_pOriginalDesc, vecVTXPositions.size(), vecVTXPositions.data(), sizeof _float3);
	m_pDesc = new BoundingSphere(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix WorldMatrix)
{
	__super::Update(WorldMatrix);
	m_pOriginalDesc->Transform(*m_pDesc, XMLoadFloat4x4(m_pLocalMatrix));
	m_pDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_Sphere::Intersect(CBounding* pTarget)
{
	_bool			isCollision = { false };

	switch (pTarget->GetType())
	{
	case ECOLLIDER::AABB:
		isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_AABB*>(pTarget)->Get_Desc());
		break;
	case ECOLLIDER::OBB:
		isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_OBB*>(pTarget)->Get_Desc());
		break;
	case ECOLLIDER::SPHERE:
		isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_Sphere*>(pTarget)->Get_Desc());
		break;
	case ECOLLIDER::CAPSULE:
		isCollision = dynamic_cast<CBounding_Capsule*>(pTarget)->Get_Desc()->Intersects(*m_pDesc);
		break;
	}

	return isCollision;
}

_bool CBounding_Sphere::IntersectRay(const RAY& ray, _float& outDist)
{
	return m_pDesc->Intersects(XMLoadFloat4(&ray.vRayOrigin), XMLoadFloat4(&ray.vRayDir), outDist);
}

CONTACTMF_DESC CBounding_Sphere::GetManifold(CBounding* pTarget)
{
	CONTACTMF_DESC _desc;

	switch (pTarget->GetType())
	{
	case ECOLLIDER::AABB:
		_desc = ComputeManifold(static_cast<CBounding_AABB*>(pTarget));
		break;
	case ECOLLIDER::OBB:
		_desc = ComputeManifold(static_cast<CBounding_OBB*>(pTarget));
		break;
	case ECOLLIDER::SPHERE:
		_desc = ComputeManifold(static_cast<CBounding_Sphere*>(pTarget));
		break;
	case ECOLLIDER::CAPSULE:
		_desc = ComputeManifold(static_cast<CBounding_Capsule*>(pTarget));
		break;
	}

	return _desc;
}

void CBounding_Sphere::UpdateWithBone(CBone* pSrcBone, CBone* pDstBone)
{
	__super::UpdateWithBone(pSrcBone, pDstBone);
	if (pSrcBone && !pDstBone) {
		XMStoreFloat3(
			&m_pOriginalDesc->Center,
			XMLoadFloat3(&m_vSrcBonePos)
		);
	}
	else if (pSrcBone && pDstBone) {
		XMStoreFloat3(
			&m_pOriginalDesc->Center,
			(XMLoadFloat3(&m_vSrcBonePos) + XMLoadFloat3(&m_vDstBonePos)) * 0.5f
		);
	}
}


_vector CBounding_Sphere::GetCenter()
{
	return  XMVectorSetW(XMLoadFloat3(&m_pDesc->Center), 1.f);
}

HRESULT CBounding_Sphere::AddDebugRenderCall(_uint iDebugMask)
{
	CHKFAIL(__super::AddDebugRenderCall(iDebugMask))
	DEBUGSPHERE_DESC _debugDesc;
	_debugDesc.eColor[0] = m_eColor;
	XMStoreFloat4x4(
		&_debugDesc.vLocalMatrix,
		XMMatrixTranslation(m_pDesc->Center.x, m_pDesc->Center.y, m_pDesc->Center.z)
	);
	_debugDesc.fRadius		= m_pDesc->Radius;
	_debugDesc.iDebugMask	= iDebugMask;
	m_pGameInstance->AddDebugRenderCall(&_debugDesc);
	return S_OK;
}

CONTACTMF_DESC CBounding_Sphere::ComputeManifold(CBounding_AABB* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	_mfDesc = pTarget->GetManifold(this);
	XMStoreFloat4(&_mfDesc.contacts[0].vNormal, XMLoadFloat4(&_mfDesc.contacts[0].vNormal)*-1.f);
	auto _vPosition = XMLoadFloat3(&m_pDesc->Center) + XMLoadFloat4(&_mfDesc.contacts[0].vNormal) * m_pDesc->Radius;
	XMStoreFloat4(&_mfDesc.contacts[0].vPosition, _vPosition);

	return _mfDesc;
}

CONTACTMF_DESC CBounding_Sphere::ComputeManifold(CBounding_OBB* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	_mfDesc = pTarget->GetManifold(this);
	XMStoreFloat4(&_mfDesc.contacts[0].vNormal, XMLoadFloat4(&_mfDesc.contacts[0].vNormal) * -1.f);
	auto _vPosition = XMLoadFloat3(&m_pDesc->Center) + XMLoadFloat4(&_mfDesc.contacts[0].vNormal) * m_pDesc->Radius;
	XMStoreFloat4(&_mfDesc.contacts[0].vPosition, _vPosition);

	return CONTACTMF_DESC();
}

CONTACTMF_DESC CBounding_Sphere::ComputeManifold(CBounding_Sphere* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	auto _pTargetDesc = pTarget->Get_Desc();

	auto _vDist		= XMLoadFloat3(&_pTargetDesc->Center) - XMLoadFloat3(&m_pDesc->Center);
	float _fDist	= XMVectorGetX(XMVector3Length(_vDist));

	_desc.fPenetration = _pTargetDesc->Radius + m_pDesc->Radius - _fDist;
	if (_fDist > 0.00001f)	XMStoreFloat4(&_desc.vNormal, XMVector3Normalize(_vDist));
	else					_desc.vNormal	= {0.f, 0.f, 1.f, 0.f};
	XMStoreFloat4(&_desc.vPosition, XMLoadFloat4(&_desc.vNormal) * m_pDesc->Radius + XMLoadFloat3(&m_pDesc->Center));

	_desc.vNormal.w		= 0.f;
	_desc.vPosition.w	= 1.f;

	_mfDesc.contactCount	= 1;
	_mfDesc.contacts[0]		= _desc;

	return _mfDesc;
}

CONTACTMF_DESC CBounding_Sphere::ComputeManifold(CBounding_Capsule* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = pTarget->Get_Desc()->Intersects(*m_pDesc);
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	_mfDesc = pTarget->GetManifold(this);
	XMStoreFloat4(&_mfDesc.contacts[0].vNormal, XMLoadFloat4(&_mfDesc.contacts[0].vNormal) * -1.f);
	auto _vPosition = XMLoadFloat3(&m_pDesc->Center) + XMLoadFloat4(&_mfDesc.contacts[0].vNormal) * m_pDesc->Radius;
	XMStoreFloat4(&_mfDesc.contacts[0].vPosition, _vPosition);

	return _mfDesc;
}

CBounding_Sphere* CBounding_Sphere::Create(const BOUNDING_DESC* pDesc)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding_Sphere* CBounding_Sphere::Create(const vector<_float3>& vecVTXPositions)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere();

	if (FAILED(pInstance->Initialize(vecVTXPositions)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBounding_Sphere::Free()
{
    __super::Free();

	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}

HRESULT CBounding_Sphere::Render_Inspector(CBounding::BOUNDING_DESC* pDesc)
{
	auto _pDesc = static_cast<SPHERE_DESC*>(pDesc);
	
	if (ImGui::DragFloat3("Center", reinterpret_cast<_float*>(&m_pOriginalDesc->Center), 0.01f)) {
		_pDesc->vCenter = m_pOriginalDesc->Center;
	}
	if (ImGui::DragFloat("Radius", reinterpret_cast<_float*>(&m_pOriginalDesc->Radius), 0.01f)) {
		_pDesc->fRadius = m_pOriginalDesc->Radius;
	}
	
	return S_OK;
}
