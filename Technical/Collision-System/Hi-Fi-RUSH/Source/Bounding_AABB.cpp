#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "Bounding_Capsule.h"
#include "GameInstance.h"

CBounding_AABB::CBounding_AABB()
{
	m_eType = ECOLLIDER::AABB;
}

HRESULT CBounding_AABB::Initialize(const BOUNDING_DESC* pDesc)
{
	const AABB_DESC* pAABBDesc = static_cast<const AABB_DESC*>(pDesc);

	m_pOriginalDesc = new BoundingBox(pAABBDesc->vCenter, pAABBDesc->vExtents);
	m_pDesc = new BoundingBox(*m_pOriginalDesc);

    return S_OK;
}

HRESULT CBounding_AABB::Initialize(const vector<_float3>& vecVTXPositions)
{
	BoundingBox::CreateFromPoints(*m_pOriginalDesc, vecVTXPositions.size(), vecVTXPositions.data(), sizeof _float3);
	m_pDesc = new BoundingBox(*m_pOriginalDesc);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	__super::Update(WorldMatrix);
	m_pOriginalDesc->Transform(*m_pDesc, XMLoadFloat4x4(m_pLocalMatrix));
	m_pDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_AABB::Intersect(CBounding* pTarget)
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

_bool CBounding_AABB::IntersectRay(const RAY& ray, _float& outDist)
{

	return m_pDesc->Intersects(XMLoadFloat4(&ray.vRayOrigin), XMLoadFloat4(&ray.vRayDir), outDist);
}

HRESULT CBounding_AABB::AddDebugRenderCall(_uint iDebugMask)
{
	CHKFAIL(__super::AddDebugRenderCall(iDebugMask))
	DEBUGAABB_DESC _debugDesc;
	_debugDesc.eColor[0] = m_eColor;
	XMStoreFloat4x4(
		&_debugDesc.vLocalMatrix,
		XMMatrixTranslation(m_pDesc->Center.x, m_pDesc->Center.y, m_pDesc->Center.z)
	);
	_debugDesc.vSize = { m_pDesc->Extents.x * 2.f, m_pDesc->Extents.y * 2.f , m_pDesc->Extents.z * 2.f };
	_debugDesc.iDebugMask = iDebugMask;
	m_pGameInstance->AddDebugRenderCall(&_debugDesc);

	return S_OK;
}

_vector CBounding_AABB::ComputeMin()
{
	return XMVectorSet(
		m_pDesc->Center.x - m_pDesc->Extents.x,
		m_pDesc->Center.y - m_pDesc->Extents.y,
		m_pDesc->Center.z - m_pDesc->Extents.z,
		1.f
	);
}

_vector CBounding_AABB::ComputeMax()
{
	return XMVectorSet(
		m_pDesc->Center.x + m_pDesc->Extents.x,
		m_pDesc->Center.y + m_pDesc->Extents.y,
		m_pDesc->Center.z + m_pDesc->Extents.z,
		1.f
	);
}

CONTACTMF_DESC CBounding_AABB::ComputeManifold(CBounding_AABB* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	auto _pTargetDesc = pTarget->Get_Desc();

	auto _vTargetMin =  pTarget->ComputeMin();
	auto _vTargetMax =  pTarget->ComputeMax();
	auto _vSourceMin =  ComputeMin();
	auto _vSourceMax =  ComputeMax();

	auto _vMaxMin		= XMVectorMin(_vTargetMax, _vSourceMax);
	auto _vMinMax		= XMVectorMax(_vTargetMin, _vSourceMin);
	
	auto _vOverWrap = _vMaxMin - _vMinMax;
	
	_desc.fPenetration	= XMVectorGetX(_vOverWrap);
	_desc.vNormal		= { 1,0,0,0 };
	_int _iOverWrapAxis = 0;
	if (XMVectorGetY(_vOverWrap) <= _desc.fPenetration) {
		_desc.fPenetration	= XMVectorGetY(_vOverWrap);
		_desc.vNormal		= { 0,1,0,0 };
		_iOverWrapAxis = 1;
	}
	if (XMVectorGetZ(_vOverWrap) <= _desc.fPenetration) {
		_desc.fPenetration	= XMVectorGetZ(_vOverWrap);
		_desc.vNormal		= { 0,0,1,0 };
		_iOverWrapAxis = 2;
	}
	auto _vDot = XMVector3Dot(
		XMVectorSetW((_vTargetMax + _vTargetMin) * 0.5f - (_vSourceMax + _vSourceMin) * 0.5f, 1.f),
		XMLoadFloat4(&_desc.vNormal)
	);

	if (XMVectorGetX(_vDot) < 0) XMStoreFloat4(&_desc.vNormal, XMLoadFloat4(&_desc.vNormal) * -1.f);
	
	_mfDesc.contactCount = 4;
	_mfDesc.contacts[0] = _mfDesc.contacts[1] = _mfDesc.contacts[2] = _mfDesc.contacts[3] = _desc;
	
	_vector _vContactPlane;
	if ((_desc.vNormal.x + _desc.vNormal.y + _desc.vNormal.z) >= 0)	_vContactPlane = _vSourceMax;
	else															_vContactPlane = _vSourceMin;

	if		(_iOverWrapAxis == 0) {
		_mfDesc.contacts[0].vPosition = { XMVectorGetX(_vContactPlane),XMVectorGetY(_vMaxMin), XMVectorGetZ(_vMaxMin), 1.f };
		_mfDesc.contacts[1].vPosition = { XMVectorGetX(_vContactPlane),XMVectorGetY(_vMinMax), XMVectorGetZ(_vMaxMin), 1.f };
		_mfDesc.contacts[2].vPosition = { XMVectorGetX(_vContactPlane),XMVectorGetY(_vMaxMin), XMVectorGetZ(_vMinMax), 1.f };
		_mfDesc.contacts[3].vPosition = { XMVectorGetX(_vContactPlane),XMVectorGetY(_vMinMax), XMVectorGetZ(_vMinMax), 1.f };
	}
	else if (_iOverWrapAxis == 1) {
		_mfDesc.contacts[0].vPosition = { XMVectorGetX(_vMaxMin),XMVectorGetY(_vContactPlane), XMVectorGetZ(_vMaxMin), 1.f };
		_mfDesc.contacts[1].vPosition = { XMVectorGetX(_vMinMax),XMVectorGetY(_vContactPlane), XMVectorGetZ(_vMaxMin), 1.f };
		_mfDesc.contacts[2].vPosition = { XMVectorGetX(_vMaxMin),XMVectorGetY(_vContactPlane), XMVectorGetZ(_vMinMax), 1.f };
		_mfDesc.contacts[3].vPosition = { XMVectorGetX(_vMinMax),XMVectorGetY(_vContactPlane), XMVectorGetZ(_vMinMax), 1.f };
	}
	else {
		_mfDesc.contacts[0].vPosition = { XMVectorGetX(_vContactPlane),XMVectorGetY(_vMaxMin), XMVectorGetZ(_vMaxMin), 1.f };
		_mfDesc.contacts[1].vPosition = { XMVectorGetX(_vContactPlane),XMVectorGetY(_vMinMax), XMVectorGetZ(_vMaxMin), 1.f };
		_mfDesc.contacts[2].vPosition = { XMVectorGetX(_vContactPlane),XMVectorGetY(_vMaxMin), XMVectorGetZ(_vMinMax), 1.f };
		_mfDesc.contacts[3].vPosition = { XMVectorGetX(_vContactPlane),XMVectorGetY(_vMinMax), XMVectorGetZ(_vMinMax), 1.f };
	}
	
	return _mfDesc;
}

CONTACTMF_DESC CBounding_AABB::ComputeManifold(CBounding_OBB* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	CBounding_OBB* pConvertTarget = Convert();
	_mfDesc = pConvertTarget->GetManifold(pTarget);
	Safe_Release(pConvertTarget);

	return _mfDesc;
}

//AABB기준 연산임
CONTACTMF_DESC CBounding_AABB::ComputeManifold(CBounding_Sphere* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}
	auto _pTargetDesc	= pTarget->Get_Desc();
	auto _vSourceMin	= ComputeMin();
	auto _vSourceMax	= ComputeMax();
	auto _vTargetCenter = XMVectorSet(_pTargetDesc->Center.x, _pTargetDesc->Center.y, _pTargetDesc->Center.z, 1);
	auto _vCloset		= XMVectorClamp(_vTargetCenter, _vSourceMin, _vSourceMax);

	//중심위치 체크
	_bool bIsCenterInBox =
		(	_pTargetDesc->Center.x >= XMVectorGetX(_vSourceMin) &&
			_pTargetDesc->Center.x <= XMVectorGetX(_vSourceMax) &&
			_pTargetDesc->Center.y >= XMVectorGetY(_vSourceMin) &&
			_pTargetDesc->Center.y <= XMVectorGetY(_vSourceMax) &&
			_pTargetDesc->Center.z >= XMVectorGetZ(_vSourceMin) &&
			_pTargetDesc->Center.z <= XMVectorGetZ(_vSourceMax));

	auto _vDelta = XMVectorMin(_vSourceMax - _vTargetCenter, _vTargetCenter - _vSourceMin);
	_float _fMinDeltaV = min(XMVectorGetX(_vDelta), min(XMVectorGetY(_vDelta), XMVectorGetZ(_vDelta)));
	
	_float fDist = XMVectorGetX(XMVector3Length(_vTargetCenter - _vCloset));
	if (fDist > 1e-6f)
		bIsCenterInBox = false;
	else {
		bIsCenterInBox = true;
	}
	//계산
	if (!bIsCenterInBox)
		XMStoreFloat4(&_desc.vNormal,		XMVector3Normalize(_vTargetCenter - _vCloset));
	else {
		if		(_fMinDeltaV == XMVectorGetX(_vDelta)) {
			if (_pTargetDesc->Center.x > m_pDesc->Center.x)
				_desc.vNormal = { +1.f, 0.f, 0.f, 0.f };
			else
				_desc.vNormal = { -1.f, 0.f, 0.f, 0.f };
		}
		else if (_fMinDeltaV == XMVectorGetY(_vDelta)) {
			if (_pTargetDesc->Center.y > m_pDesc->Center.y)
				_desc.vNormal = { 0.f, +1.f, 0.f, 0.f };
			else
				_desc.vNormal = { 0.f, -1.f, 0.f, 0.f };
		}
		else if (_fMinDeltaV == XMVectorGetZ(_vDelta)) {
			if (_pTargetDesc->Center.z > m_pDesc->Center.z)
				_desc.vNormal = { 0.f, 0.f, +1.f, 0.f };
			else
				_desc.vNormal = { 0.f, 0.f, -1.f, 0.f };
		}
	}

	if (!bIsCenterInBox)
		_desc.fPenetration = _pTargetDesc->Radius - fDist;
	else
		_desc.fPenetration = _pTargetDesc->Radius + _fMinDeltaV;

	XMStoreFloat4(&_desc.vPosition, _vCloset);
	
	_mfDesc.contactCount = 1;
	_mfDesc.contacts[0] = _desc;
	
	return _mfDesc;
}

CONTACTMF_DESC CBounding_AABB::ComputeManifold(CBounding_Capsule* pTarget)
{
	auto _mfDesc = pTarget->GetManifold(this);
	XMStoreFloat4(&_mfDesc.contacts[0].vNormal, XMLoadFloat4(&_mfDesc.contacts[0].vNormal));
	;
	_mfDesc.contacts[0].vPosition;

	return _mfDesc;
}

CBounding_AABB* CBounding_AABB::Create(const BOUNDING_DESC* pDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding_AABB* CBounding_AABB::Create(const vector<_float3>& vecVTXPositions)
{
	CBounding_AABB* pInstance = new CBounding_AABB();

	if (FAILED(pInstance->Initialize(vecVTXPositions)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBounding_AABB::Free()
{
    __super::Free();

	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}

HRESULT CBounding_AABB::Render_Inspector(CBounding::BOUNDING_DESC* pDesc)
{
	auto _pDesc = static_cast<AABB_DESC*>(pDesc);
	if (ImGui::DragFloat3("Center", reinterpret_cast<_float*>(&m_pOriginalDesc->Center), 0.01f)) {
		_pDesc->vCenter = m_pOriginalDesc->Center;
	}
	if (ImGui::DragFloat3("Extent", reinterpret_cast<_float*>(&m_pOriginalDesc->Extents), 0.01f)) {
		_pDesc->vExtents = m_pOriginalDesc->Extents;
	}
		
	return S_OK;
}

CONTACTMF_DESC CBounding_AABB::GetManifold(CBounding* pTarget)
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

CBounding_OBB* CBounding_AABB::Convert()
{
	BoundingOrientedBox _desc;
	BoundingOrientedBox::CreateFromBoundingBox(_desc, *m_pDesc);

	return CBounding_OBB::Create(&_desc);
}

void CBounding_AABB::UpdateWithBone(CBone* pSrcBone, CBone* pDstBone)
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

_vector CBounding_AABB::GetCenter()
{
	return  XMVectorSetW(XMLoadFloat3(&m_pDesc->Center), 1.f);
}
