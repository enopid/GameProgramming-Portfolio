#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "Bounding_Capsule.h"
#include "GameInstance.h"

CBounding_OBB::CBounding_OBB()
{
	m_eType = ECOLLIDER::OBB;
}

HRESULT CBounding_OBB::Initialize(const BOUNDING_DESC* pDesc)
{
	const OBB_DESC* pOBBDesc = static_cast<const OBB_DESC*>(pDesc);

	_float4		vQuaternion = {};
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(pOBBDesc->vAngles.x),
		XMConvertToRadians(pOBBDesc->vAngles.y), 
		XMConvertToRadians(pOBBDesc->vAngles.z)));	

	m_pOriginalDesc = new BoundingOrientedBox(pOBBDesc->vCenter, pOBBDesc->vExtents, vQuaternion);
	m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

    return S_OK;
}

HRESULT CBounding_OBB::Initialize(const vector<_float3>& vecVTXPositions)
{
	BoundingOrientedBox::CreateFromPoints(*m_pOriginalDesc, vecVTXPositions.size(), vecVTXPositions.data(), sizeof _float3);
	m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);
	
	return S_OK;
}

HRESULT CBounding_OBB::Initialize(BoundingOrientedBox* _pDesc)
{
	m_pDesc			= new BoundingOrientedBox(*_pDesc);
	m_pOriginalDesc = new BoundingOrientedBox(*_pDesc);
	return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
	__super::Update(WorldMatrix);
	m_pOriginalDesc->Transform(*m_pDesc, XMLoadFloat4x4(m_pLocalMatrix));
	m_pDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_OBB::Intersect(CBounding* pTarget)
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

_bool CBounding_OBB::IntersectRay(const RAY& ray, _float& outDist)
{
	return m_pDesc->Intersects(XMLoadFloat4(&ray.vRayOrigin), XMLoadFloat4(&ray.vRayDir), outDist);
}

CONTACTMF_DESC CBounding_OBB::GetManifold(CBounding* pTarget)
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
		break;
	}

	return _desc;
}

void CBounding_OBB::UpdateWithBone(CBone* pSrcBone, CBone* pDstBone)
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

		auto _vDir = (XMLoadFloat3(&m_vDstBonePos) - XMLoadFloat3(&m_vSrcBonePos));
		m_pOriginalDesc->Extents.y = XMVectorGetX(XMVector3Length(_vDir))*0.5f;
		if (abs(m_pOriginalDesc->Extents.y) > 0.00001f) {
			XMVECTOR q = XMLoadFloat4(&m_pOriginalDesc->Orientation);
			XMMATRIX m = XMMatrixRotationQuaternion(q);

			XMVECTOR vAxisX = XMVectorSetW(XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f), m), 0.f);
			XMVECTOR vAxisY = XMVectorSetW(XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), m), 0.f);
			XMVECTOR vAxisZ = XMVectorSetW(XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), m), 0.f);

			vAxisY = XMVectorSetW(XMVector3Normalize(_vDir), 0.f);

			float dotXY = fabsf(XMVectorGetX(XMVector3Dot(vAxisX, vAxisY)));
			if (dotXY > 0.99f)
			{
				XMVECTOR worldUp = XMVectorSet(0.f, 0.f, 1.f, 0.f);
				if (fabsf(XMVectorGetX(XMVector3Dot(worldUp, vAxisY))) > 0.99f)
					worldUp = XMVectorSet(1.f, 0.f, 0.f, 0.f);

				vAxisX = XMVector3Normalize(XMVector3Cross(worldUp, vAxisY));
			}
			else
			{
				vAxisX = XMVector3Normalize(
					vAxisX - vAxisY * XMVectorGetX(XMVector3Dot(vAxisX, vAxisY))
				);
			}

			vAxisZ = XMVectorSetW(XMVector3Normalize(XMVector3Cross(vAxisX, vAxisY)), 0.f);
			vAxisX = XMVectorSetW(XMVector3Normalize(XMVector3Cross(vAxisY, vAxisZ)), 0.f);

			m.r[0] = vAxisX;
			m.r[1] = vAxisY;
			m.r[2] = vAxisZ;
			m.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

			XMStoreFloat4(&m_pOriginalDesc->Orientation, XMQuaternionRotationMatrix(m));
		}
	}
}


_vector CBounding_OBB::GetCenter()
{
	return  XMVectorSetW(XMLoadFloat3(&m_pDesc->Center), 1.f);
}

HRESULT CBounding_OBB::AddDebugRenderCall(_uint iDebugMask)
{
	CHKFAIL(__super::AddDebugRenderCall(iDebugMask))
	DEBUGOBB_DESC _debugDesc;
	_debugDesc.eColor[0] = m_eColor;
	XMStoreFloat4x4(
		&_debugDesc.vLocalMatrix,
		XMMatrixRotationQuaternion(XMLoadFloat4(&m_pDesc->Orientation)) *
		XMMatrixTranslation(m_pDesc->Center.x, m_pDesc->Center.y, m_pDesc->Center.z)
	);
	_debugDesc.vSize = { m_pDesc->Extents.x *2.f, m_pDesc->Extents.y * 2.f, m_pDesc->Extents.z * 2.f };
	_debugDesc.iDebugMask = iDebugMask;
	m_pGameInstance->AddDebugRenderCall(&_debugDesc);

	return S_OK;
}

array<_vector, 3> CBounding_OBB::ComputeAxis()
{
	array<_vector, 3> _arrAxis;
	auto _rotMat = XMMatrixRotationQuaternion(XMLoadFloat4(&m_pDesc->Orientation));
	_arrAxis[0] = XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f), _rotMat);
	_arrAxis[1] = XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), _rotMat);
	_arrAxis[2] = XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), _rotMat);
	return _arrAxis;
}

CONTACTMF_DESC CBounding_OBB::ComputeManifold(CBounding_AABB* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	CBounding_OBB* pConvertTarget = pTarget->Convert();
	_mfDesc =  ComputeManifold(pConvertTarget);
	Safe_Release(pConvertTarget);

	return _mfDesc;
}

CONTACTMF_DESC CBounding_OBB::ComputeManifold(CBounding_OBB* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	auto _pTargetDesc		= pTarget->Get_Desc();
	auto _arrTargetAxises	= pTarget->ComputeAxis();

	auto _arrSourceAxises	= ComputeAxis();
	auto _vCenterDist		= XMLoadFloat3(&_pTargetDesc->Center) - XMLoadFloat3(&m_pDesc->Center);
	_vector _arrAxises[6]	= { _arrSourceAxises[0], _arrSourceAxises[1], _arrSourceAxises[2], _arrTargetAxises[0], _arrTargetAxises[1], _arrTargetAxises[2] };

	int		iMinOverlapIdx	= -1;
	float	fPenetration	= 2.f * XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pDesc->Extents)));

	for (int i = 0; i < 6; i++)
	{
		_float _fProjA(0.f), _fProjB(0.f), _fDist(0.f);
		_fProjA += abs(XMVectorGetX(XMVector3Dot(_arrAxises[i], _arrSourceAxises[0]))) * m_pDesc->Extents.x;
		_fProjA += abs(XMVectorGetX(XMVector3Dot(_arrAxises[i], _arrSourceAxises[1]))) * m_pDesc->Extents.y;
		_fProjA += abs(XMVectorGetX(XMVector3Dot(_arrAxises[i], _arrSourceAxises[2]))) * m_pDesc->Extents.z;

		_fProjB += abs(XMVectorGetX(XMVector3Dot(_arrAxises[i], _arrTargetAxises[0]))) * _pTargetDesc->Extents.x;
		_fProjB += abs(XMVectorGetX(XMVector3Dot(_arrAxises[i], _arrTargetAxises[1]))) * _pTargetDesc->Extents.y;
		_fProjB += abs(XMVectorGetX(XMVector3Dot(_arrAxises[i], _arrTargetAxises[2]))) * _pTargetDesc->Extents.z;
		
		_fDist	+= abs(XMVectorGetX(XMVector3Dot(_arrAxises[i], _vCenterDist)));
		
		_float fOverlap = _fProjA + _fProjB - _fDist;

		if (fOverlap <= 0.f) {
			_mfDesc.contactCount = 0;
			return _mfDesc;
		}

		if (fOverlap < fPenetration) {
			fPenetration	= fOverlap;
			iMinOverlapIdx	= i;
		}
	}

	_bool bIsReverse = XMVectorGetX(XMVector3Dot(_vCenterDist, _arrAxises[iMinOverlapIdx])) < 0;

	_desc.fPenetration	= fPenetration;
	XMStoreFloat4(&_desc.vNormal, _arrAxises[iMinOverlapIdx] * ((bIsReverse) ? -1.f : 1.f));

	_mfDesc.contacts[0] = _mfDesc.contacts[1] = _mfDesc.contacts[2] = _mfDesc.contacts[3] = _desc;
	_mfDesc.contactCount = 4;

	//CalcPos
	bool bRefIsSource = (iMinOverlapIdx < 3);

	auto	pRefDesc	= bRefIsSource ? m_pDesc : _pTargetDesc;
	auto	refAxes		= bRefIsSource ? _arrSourceAxises : _arrTargetAxises;
	
	int		refFaceIdx = iMinOverlapIdx%3;

	auto vRefCenter = XMLoadFloat3(&pRefDesc->Center);
	auto vNormal	= XMLoadFloat4(&_desc.vNormal);

	// 해당 축의 half extent
	float half =
		(refFaceIdx == 0) ? pRefDesc->Extents.x :
		(refFaceIdx == 1) ? pRefDesc->Extents.y :
		pRefDesc->Extents.z;

	// face 중심
	XMVECTOR refToOther = bRefIsSource ? _vCenterDist : XMVectorNegate(_vCenterDist);
	XMVECTOR faceAxis = refAxes[refFaceIdx%3];
	float sign = (XMVectorGetX(XMVector3Dot(refToOther, faceAxis)) >= 0.f) ? 1.f : -1.f;
	_vector faceCenter = vRefCenter + faceAxis * (sign * half);


	int t0 = (refFaceIdx + 1) % 3;
	int t1 = (refFaceIdx + 2) % 3;

	auto axis0 = refAxes[t0];
	auto axis1 = refAxes[t1];

	float half0 =
		(t0 == 0) ? pRefDesc->Extents.x :
		(t0 == 1) ? pRefDesc->Extents.y :
		pRefDesc->Extents.z;

	float half1 =
		(t1 == 0) ? pRefDesc->Extents.x :
		(t1 == 1) ? pRefDesc->Extents.y :
		pRefDesc->Extents.z;

	auto p0 = faceCenter + axis0 * half0 + axis1 * half1;
	auto p1 = faceCenter - axis0 * half0 + axis1 * half1;
	auto p2 = faceCenter + axis0 * half0 - axis1 * half1;
	auto p3 = faceCenter - axis0 * half0 - axis1 * half1;

	XMStoreFloat4(&_mfDesc.contacts[0].vPosition, p0);
	XMStoreFloat4(&_mfDesc.contacts[1].vPosition, p1);
	XMStoreFloat4(&_mfDesc.contacts[2].vPosition, p2);
	XMStoreFloat4(&_mfDesc.contacts[3].vPosition, p3);
	
	for (int i = 0; i < 4; ++i) _mfDesc.contacts[i].vPosition.w = 1.f;

	return _mfDesc;
}

CONTACTMF_DESC CBounding_OBB::ComputeManifold(CBounding_Sphere* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	auto _pTargetDesc	= pTarget->Get_Desc();
	auto _arrAxises		= ComputeAxis();

	auto _vCenterDist = XMVectorSetW(
		XMLoadFloat3(&_pTargetDesc->Center) - XMLoadFloat3(&m_pDesc->Center), 0.f
	);

	auto _rotMat			= XMMatrixRotationQuaternion(XMLoadFloat4(&m_pDesc->Orientation));
	auto _rotInvMat			= XMMatrixTranspose(_rotMat);
	auto _vLocalCenterDist	= XMVector3TransformNormal(_vCenterDist, _rotInvMat);
	auto _fRadius			= _pTargetDesc->Radius;

	CBounding_AABB::AABB_DESC _aabbDesc;
	_aabbDesc.vCenter	= { 0.f,0.f,0.f };
	_aabbDesc.vExtents	= m_pDesc->Extents;
	auto _pTmpAABB = CBounding_AABB::Create(&_aabbDesc);

	CBounding_Sphere::SPHERE_DESC _sphereDesc;
	XMStoreFloat3(&_sphereDesc.vCenter, _vLocalCenterDist);
	_sphereDesc.fRadius = _fRadius;
	auto _pTmpSphere = CBounding_Sphere::Create(&_sphereDesc);

	_mfDesc = _pTmpAABB->GetManifold(_pTmpSphere);

	Safe_Release(_pTmpAABB);
	Safe_Release(_pTmpSphere);

	XMStoreFloat4(
		&_mfDesc.contacts[0].vPosition,
		XMVector3TransformCoord(
			XMLoadFloat4(&_mfDesc.contacts[0].vPosition),
			_rotMat
		) + XMLoadFloat3(&m_pDesc->Center)
	);

	XMStoreFloat4(
		&_mfDesc.contacts[0].vNormal,
		XMVector3TransformNormal(
			XMLoadFloat4(&_mfDesc.contacts[0].vNormal),
			_rotMat
		)
	);
	_mfDesc.contacts[0].vNormal.w = 0.f;
	_mfDesc.contacts[0].vPosition.w = 1.f;

	return _mfDesc;
}

CBounding_OBB* CBounding_OBB::Create(const BOUNDING_DESC* pDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding_OBB* CBounding_OBB::Create(const vector<_float3>& vecVTXPositions)
{
	CBounding_OBB* pInstance = new CBounding_OBB();

	if (FAILED(pInstance->Initialize(vecVTXPositions)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding_OBB* CBounding_OBB::Create(BoundingOrientedBox* _pDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB();

	if (FAILED(pInstance->Initialize(_pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBounding_OBB::Free()
{
    __super::Free();

	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}

HRESULT CBounding_OBB::Render_Inspector(CBounding::BOUNDING_DESC* pDesc)
{
	auto _pDesc = static_cast<OBB_DESC*>(pDesc);
	if (ImGui::DragFloat3("Center", reinterpret_cast<_float*>(&m_pOriginalDesc->Center), 0.01f)) {
		_pDesc->vCenter = m_pOriginalDesc->Center;
	}
	if (ImGui::DragFloat3("Extent", reinterpret_cast<_float*>(&m_pOriginalDesc->Extents), 0.01f)) {
		_pDesc->vExtents = m_pOriginalDesc->Extents;
	}

	_float3 _vAngles;
	{
		XMVECTOR q = XMLoadFloat4(&m_pOriginalDesc->Orientation);
		XMMATRIX m = XMMatrixRotationQuaternion(q);

		float pitch = asinf(-m.r[2].m128_f32[1]);                 // X
		float yaw = atan2f(m.r[2].m128_f32[0], m.r[2].m128_f32[2]); // Y
		float roll = atan2f(m.r[0].m128_f32[1], m.r[1].m128_f32[1]); // Z

		_vAngles.x = XMConvertToDegrees(pitch);
		_vAngles.y = XMConvertToDegrees(yaw);
		_vAngles.z = XMConvertToDegrees(roll);

	}

	if (ImGui::DragFloat3("Angles", reinterpret_cast<_float*>(&_vAngles), 0.01f)) {
		_pDesc->vAngles = _vAngles;
	}
	XMStoreFloat4(&m_pOriginalDesc->Orientation, XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(_vAngles.x),
		XMConvertToRadians(_vAngles.y),
		XMConvertToRadians(_vAngles.z)));

	return S_OK;
}
