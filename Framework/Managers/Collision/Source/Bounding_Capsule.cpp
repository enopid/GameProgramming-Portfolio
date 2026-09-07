#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "Bounding_Capsule.h"
#include "GameInstance.h"

CBounding_Capsule::CBounding_Capsule()
{
	m_eType = ECOLLIDER::CAPSULE;
}

HRESULT CBounding_Capsule::Initialize(const BOUNDING_DESC* pDesc)
{
	const CAPSULE_DESC* pCapsuleDesc = static_cast<const CAPSULE_DESC*>(pDesc);

	_float4		vQuaternion = {};
	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(pCapsuleDesc->vAngles.x),
		XMConvertToRadians(pCapsuleDesc->vAngles.y),
		XMConvertToRadians(pCapsuleDesc->vAngles.z)));


	m_pOriginalDesc = new BoundingCapsule(pCapsuleDesc->vCenter, pCapsuleDesc->fRadius, pCapsuleDesc->fHalfHeight, vQuaternion);
	m_pDesc			= new BoundingCapsule(*m_pOriginalDesc);

    return S_OK;
}

HRESULT CBounding_Capsule::Initialize(const vector<_float3>& vecVTXPositions)
{
	BoundingCapsule::CreateFromPoints(*m_pOriginalDesc, vecVTXPositions.size(), vecVTXPositions.data(), sizeof _float3);
	m_pDesc = new BoundingCapsule(*m_pOriginalDesc);
	
	return S_OK;
}

void CBounding_Capsule::Update(_fmatrix WorldMatrix)
{
	__super::Update(WorldMatrix);
	m_pOriginalDesc->Transform(*m_pDesc, XMLoadFloat4x4(m_pLocalMatrix));
	m_pDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_Capsule::Intersect(CBounding* pTarget)
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
		isCollision = m_pDesc->Intersects(*dynamic_cast<CBounding_Capsule*>(pTarget)->Get_Desc());
		break;
	}

	return isCollision;
}

_bool CBounding_Capsule::IntersectRay(const RAY& ray, _float& outDist)
{
	return m_pDesc->Intersects(XMLoadFloat4(&ray.vRayOrigin), XMLoadFloat4(&ray.vRayDir), outDist);
}

CONTACTMF_DESC CBounding_Capsule::GetManifold(CBounding* pTarget)
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

void CBounding_Capsule::UpdateWithBone(CBone* pSrcBone, CBone* pDstBone)
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
		m_pOriginalDesc->HalfHeight = XMVectorGetX(XMVector3Length(_vDir)) * 0.5f;
		if (abs(m_pOriginalDesc->HalfHeight) > 0.00001f) {
			XMVECTOR q = XMLoadFloat4(&m_pOriginalDesc->Orientation);
			XMMATRIX m = XMMatrixRotationQuaternion(q);

			XMVECTOR vAxisX = XMVectorSetW(XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f), m),0.f);
			XMVECTOR vAxisY = XMVectorSetW(XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f), m),0.f);
			XMVECTOR vAxisZ = XMVectorSetW(XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f), m),0.f);
			
			vAxisY = XMVectorSetW(XMVector3Normalize(_vDir), 0.f);

			float dotXY = fabsf(XMVectorGetX(XMVector3Dot(vAxisX, vAxisY)));
			if (dotXY > 0.99f)
			{
				XMVECTOR worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
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


_vector CBounding_Capsule::GetCenter()
{
	return  XMVectorSetW(XMLoadFloat3(&m_pDesc->Center), 1.f);
}

HRESULT CBounding_Capsule::AddDebugRenderCall(_uint iDebugMask)
{
	CHKFAIL(__super::AddDebugRenderCall(iDebugMask))
	DEBUGCAPSULE_DESC _debugDesc;
	_debugDesc.eColor[0] = m_eColor;
	XMStoreFloat4x4(
		&_debugDesc.vLocalMatrix,
		XMMatrixRotationQuaternion(XMLoadFloat4(&m_pDesc->Orientation)) *
		XMMatrixTranslation(m_pDesc->Center.x, m_pDesc->Center.y, m_pDesc->Center.z)
	);
	_debugDesc.fRadius		= m_pDesc->Radius;
	_debugDesc.fHeight		= m_pDesc->HalfHeight * 2.f;
	_debugDesc.iDebugMask	= iDebugMask;
	_debugDesc.eColor;
	
	m_pGameInstance->AddDebugRenderCall(&_debugDesc);
	return S_OK;
}

CONTACTMF_DESC CBounding_Capsule::ComputeManifold(CBounding_AABB* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}
	
	auto box = *pTarget->Get_Desc();
	auto _srcRay = m_pDesc->GetRay();

	float t = 0.f;
	auto _vMin = XMVectorSet(
		box.Center.x - box.Extents.x,
		box.Center.y - box.Extents.y,
		box.Center.z - box.Extents.z,
		1.f
	); auto _vMax = XMVectorSet(
		box.Center.x + box.Extents.x,
		box.Center.y + box.Extents.y,
		box.Center.z + box.Extents.z,
		1.f
	);
	
	_vector boxCenter	= XMLoadFloat3(&box.Center);
	_vector segDir		= XMLoadFloat4(&_srcRay.vRayDir);
	_vector P0			= XMLoadFloat4(&_srcRay.vRayOrigin);
	float segLenSq		= XMVectorGetX(XMVector3LengthSq(segDir));
	if (segLenSq > 0.000001f)
	{
		t = XMVectorGetX(
			XMVector3Dot(boxCenter - P0, segDir)
		) / segLenSq;

		t = std::clamp(t, 0.f, 1.f);
	}


	t = clamp(t, 0.0f, 1.0f);
	_vector _vPoint = XMLoadFloat4(&_srcRay.vRayOrigin) + XMLoadFloat4(&_srcRay.vRayDir) * t;
	_vector _vClosest = XMVectorClamp(_vPoint, _vMin, _vMax);

	auto _vDist = _vClosest - _vPoint;
	auto _fDist = XMVectorGetX(XMVector3Length(_vDist));


	if (_fDist > 0.00001f)
	{
		XMStoreFloat4(&_desc.vNormal, XMVector3Normalize(_vDist));
		_desc.fPenetration = m_pDesc->Radius - _fDist;
		XMStoreFloat4(&_desc.vPosition, _vPoint + XMVector3Normalize(_vDist) * _desc.fPenetration);
	}
	else {
		_vector boxCenter	= XMLoadFloat3(&box.Center);
		_vector extents		= XMLoadFloat3(&box.Extents);

		// 박스 로컬 좌표
		_vector local = _vPoint - boxCenter;
		_vector absLocal = XMVectorAbs(local);

		// 각 축에서 내부로 들어간 거리
		_vector delta = extents - absLocal;

		float dx = XMVectorGetX(delta);
		float dy = XMVectorGetY(delta);
		float dz = XMVectorGetZ(delta);

		_vector normal;
		float penetrationAxis = 0.f;

		// 가장 penetration이 작은 축 선택 (MTV 축)
		if (dx < dy && dx < dz)
		{
			normal = XMVectorSet(
				(XMVectorGetX(local) > 0.f ? -1.f : 1.f),
				0.f, 0.f, 0.f);

			penetrationAxis = dx;
		}
		else if (dy < dz)
		{
			normal = XMVectorSet(
				0.f,
				(XMVectorGetY(local) > 0.f ? -1.f : 1.f),
				0.f, 0.f);

			penetrationAxis = dy;
		}
		else
		{
			normal = XMVectorSet(
				0.f, 0.f,
				(XMVectorGetZ(local) > 0.f ? -1.f : 1.f),
				0.f);

			penetrationAxis = dz;
		}

		// 최종 penetration
		float penetration = m_pDesc->Radius + penetrationAxis;

		_desc.fPenetration = penetration;

		// 접촉 위치는 박스 표면
		_vector contactPoint = _vPoint + normal * m_pDesc->Radius;

		XMStoreFloat4(&_desc.vNormal, normal);
		XMStoreFloat4(&_desc.vPosition, contactPoint);
		//PrintDebug("Normal : ", _desc.vNormal);
		//PrintDebug("Penetration : ", _desc.fPenetration);
	}

	_desc.vNormal.w		= 0.f;
	_desc.vPosition.w	= 1.f;

	_mfDesc.contactCount = 1;
	_mfDesc.contacts[0] = _desc;

	return _mfDesc;
}

CONTACTMF_DESC CBounding_Capsule::ComputeManifold(CBounding_OBB* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto bIsCollision = m_pDesc->Intersects(*pTarget->Get_Desc());
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	auto box = *pTarget->Get_Desc();
	auto _srcRay = m_pDesc->GetRay();

	auto _rotM = XMMatrixInverse(nullptr, XMMatrixRotationQuaternion(XMLoadFloat4(&box.Orientation)));

	XMStoreFloat4(
		&_srcRay.vRayDir,
		XMVector3TransformNormal(XMLoadFloat4(&_srcRay.vRayDir), _rotM)
	);
	XMStoreFloat4(
		&_srcRay.vRayOrigin,
		XMVectorSetW(
			XMVector3TransformCoord(XMLoadFloat4(&_srcRay.vRayOrigin) - XMLoadFloat3(&box.Center), _rotM),
			1.f
		)
	);

	float t = 0.f;
	auto _vMin = XMVectorSet(
		- box.Extents.x,
		- box.Extents.y,
		- box.Extents.z,
		1.f
	); auto _vMax = XMVectorSet(
		+ box.Extents.x,
		+ box.Extents.y,
		+ box.Extents.z,
		1.f
	);

	_vector boxCenter = XMLoadFloat3(&box.Center);
	_vector segDir = XMLoadFloat4(&_srcRay.vRayDir);
	_vector P0 = XMLoadFloat4(&_srcRay.vRayOrigin);
	float segLenSq = XMVectorGetX(XMVector3LengthSq(segDir));
	if (segLenSq > 0.000001f)
	{
		t = XMVectorGetX(
			XMVector3Dot(- P0, segDir)
		) / segLenSq;

		t = std::clamp(t, 0.f, 1.f);
	}


	t = clamp(t, 0.0f, 1.0f);
	_vector _vPoint = XMLoadFloat4(&_srcRay.vRayOrigin) + XMLoadFloat4(&_srcRay.vRayDir) * t;
	_vector _vClosest = XMVectorClamp(_vPoint, _vMin, _vMax);

	auto _vDist = _vClosest - _vPoint;
	auto _fDist = XMVectorGetX(XMVector3Length(_vDist));


	if (_fDist > 0.00001f)
	{
		XMStoreFloat4(&_desc.vNormal, XMVector3Normalize(_vDist));
		_desc.fPenetration = m_pDesc->Radius - _fDist;
		XMStoreFloat4(&_desc.vPosition, _vPoint + XMVector3Normalize(_vDist) * _desc.fPenetration);
	}
	else {
		_vector extents = XMLoadFloat3(&box.Extents);

		// 박스 로컬 좌표
		_vector local = _vPoint;
		_vector absLocal = XMVectorAbs(local);

		// 각 축에서 내부로 들어간 거리
		_vector delta = extents - absLocal;

		float dx = XMVectorGetX(delta);
		float dy = XMVectorGetY(delta);
		float dz = XMVectorGetZ(delta);

		_vector normal;
		float penetrationAxis = 0.f;

		// 가장 penetration이 작은 축 선택 (MTV 축)
		if (dx < dy && dx < dz)
		{
			normal = XMVectorSet(
				(XMVectorGetX(local) > 0.f ? -1.f : 1.f),
				0.f, 0.f, 0.f);

			penetrationAxis = dx;
		}
		else if (dy < dz)
		{
			normal = XMVectorSet(
				0.f,
				(XMVectorGetY(local) > 0.f ? -1.f : 1.f),
				0.f, 0.f);

			penetrationAxis = dy;
		}
		else
		{
			normal = XMVectorSet(
				0.f, 0.f,
				(XMVectorGetZ(local) > 0.f ? -1.f : 1.f),
				0.f);

			penetrationAxis = dz;
		}

		// 최종 penetration
		float penetration = m_pDesc->Radius + penetrationAxis;

		_desc.fPenetration = penetration;

		// 접촉 위치는 박스 표면
		_vector contactPoint = _vPoint + normal * m_pDesc->Radius;

		XMStoreFloat4(&_desc.vNormal, normal);
		XMStoreFloat4(&_desc.vPosition, contactPoint);
		//PrintDebug("Normal : ", _desc.vNormal);
		//PrintDebug("Penetration : ", _desc.fPenetration);
	}

	_matrix rot = XMMatrixRotationQuaternion(XMLoadFloat4(&box.Orientation));

	// 노말 (방향벡터 → Normal transform)
	_vector worldNormal = XMVector3TransformNormal(XMLoadFloat4(&_desc.vNormal), rot);
	worldNormal = XMVector3Normalize(worldNormal);

	// 접촉점 (좌표 → Coord transform + Center 복원)
	_vector worldPos =
		XMVector3TransformCoord(XMLoadFloat4(&_desc.vPosition), rot)
		+ XMLoadFloat3(&box.Center);

	XMStoreFloat4(&_desc.vNormal, worldNormal);
	XMStoreFloat4(&_desc.vPosition, XMVectorSetW(worldPos, 1.f));

	_desc.vNormal.w = 0.f;
	_desc.vPosition.w = 1.f;

	_mfDesc.contactCount = 1;
	_mfDesc.contacts[0] = _desc;

	return _mfDesc;
}

CONTACTMF_DESC CBounding_Capsule::ComputeManifold(CBounding_Sphere* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto _pTargetDesc = pTarget->Get_Desc();
	auto bIsCollision = m_pDesc->Intersects(*_pTargetDesc);
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	auto _vAxisDir = XMVector3TransformNormal(
		XMVectorSet(0.f, 1.f, 0.f, 0.f),
		XMMatrixRotationQuaternion(XMLoadFloat4(&m_pDesc->Orientation))
	);
	_vAxisDir = XMVector3Normalize(_vAxisDir);
	_vector _vSrt = XMLoadFloat3(&m_pDesc->Center) - _vAxisDir * m_pDesc->HalfHeight;
	_vector _vSHCenter = XMLoadFloat3(&_pTargetDesc->Center);

	_float fRatio = XMVectorGetX(XMVector3Dot(_vAxisDir, _vSHCenter - _vSrt));
	fRatio = clamp(fRatio, 0.f, m_pDesc->HalfHeight * 2);
	_vector _vProjCenter = _vSrt + fRatio * _vAxisDir;

	_desc.fPenetration = (m_pDesc->Radius + _pTargetDesc->Radius) - XMVectorGetX(XMVector3Length(_vSHCenter - _vProjCenter));
	XMStoreFloat4(
		&_desc.vNormal,
		XMVector3Normalize(_vSHCenter - _vProjCenter)
	);
	XMStoreFloat4(
		&_desc.vPosition,
		_vProjCenter + XMVector3Normalize(_vSHCenter - _vProjCenter) * m_pDesc->Radius
	);
	_desc.vNormal.w = 0.f;
	_desc.vPosition.w = 1.f;

	_mfDesc.contactCount = 1;
	_mfDesc.contacts[0] = _desc;

	return _mfDesc;
}

CONTACTMF_DESC CBounding_Capsule::ComputeManifold(CBounding_Capsule* pTarget)
{
	CONTACTMF_DESC _mfDesc;
	CONTACTPT_DESC _desc;

	auto _pTargetDesc = pTarget->Get_Desc();
	auto bIsCollision = m_pDesc->Intersects(*_pTargetDesc);
	if (!bIsCollision) {
		_mfDesc.contactCount = 0;
		return _mfDesc;
	}

	//직선사이의 최근접점
	auto _srcRay = m_pDesc->GetRay();
	auto _dstRay = _pTargetDesc->GetRay();

	_vector u = XMLoadFloat4(&_srcRay.vRayDir);                                        //u
	_vector v = XMLoadFloat4(&_dstRay.vRayDir);                                        //v
	_vector w = XMLoadFloat4(&_srcRay.vRayOrigin) - XMLoadFloat4(&_dstRay.vRayOrigin); //w

	auto a = XMVector3Dot(u, u); // A의 길이
	auto b = XMVector3Dot(u, v); // A B 내적
	auto c = XMVector3Dot(v, v); // B의 길이
	auto d = XMVector3Dot(u, w); //시작점 A 내적
	auto e = XMVector3Dot(v, w); //시작점 B 내적

	float a_ = XMVectorGetX(a);
	float b_ = XMVectorGetX(b);
	float c_ = XMVectorGetX(c);
	float d_ = XMVectorGetX(d);
	float e_ = XMVectorGetX(e);
	float D_ = a_ * c_ - b_ * b_;

	float sN, sD = D_;
	float tN, tD = D_;

	if (D_ < 1e-6f) // 거의 평행
	{
		sN = 0.0f;
		sD = 1.0f;
		tN = e_;
		tD = c_;
	}
	else
	{
		sN = (b_ * e_ - c_ * d_);
		tN = (a_ * e_ - b_ * d_);

		if (sN < 0.f)
		{
			sN = 0.f;
			tN = e_;
			tD = c_;
		}
		else if (sN > sD)
		{
			sN = sD;
			tN = e_ + b_;
			tD = c_;
		}
	}

	if (tN < 0.f)
	{
		tN = 0.f;

		if (-d_ < 0.f)
			sN = 0.f;
		else if (-d_ > a_)
			sN = sD;
		else
		{
			sN = -d_;
			sD = a_;
		}
	}
	else if (tN > tD)
	{
		tN = tD;

		if ((-d_ + b_) < 0.f)
			sN = 0;
		else if ((-d_ + b_) > a_)
			sN = sD;
		else
		{
			sN = (-d_ + b_);
			sD = a_;
		}
	}

	float s = (fabs(sN) < 1e-6f ? 0.f : sN / sD);
	float t = (fabs(tN) < 1e-6f ? 0.f : tN / tD);
	
	auto pA = XMLoadFloat4(&_srcRay.vRayOrigin);
	if (abs(XMVectorGetX(XMVector3Length(XMLoadFloat4(&_srcRay.vRayDir)))) > 0.00001f) pA += s * XMLoadFloat4(&_srcRay.vRayDir);
	

	auto pB = XMLoadFloat4(&_dstRay.vRayOrigin);
	if (abs(XMVectorGetX(XMVector3Length(XMLoadFloat4(&_dstRay.vRayDir)))) > 0.00001f) pB += t * XMLoadFloat4(&_dstRay.vRayDir);

	_desc.fPenetration = (m_pDesc->Radius + _pTargetDesc->Radius) - XMVectorGetX(XMVector3Length(pB - pA));
	XMStoreFloat4(
		&_desc.vNormal,
		XMVector3Normalize(pB - pA)
	);
	XMStoreFloat4(
		&_desc.vPosition,
		pA + XMVector3Normalize(pB - pA) * m_pDesc->Radius
	);
	_desc.vNormal.w		= 0.f;
	_desc.vPosition.w	= 1.f;

	_mfDesc.contactCount = 1;
	_mfDesc.contacts[0] = _desc;
	
	return _mfDesc;
}

CBounding_Capsule* CBounding_Capsule::Create(const BOUNDING_DESC* pDesc)
{
	CBounding_Capsule* pInstance = new CBounding_Capsule();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : CBounding_Capsule");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding_Capsule* CBounding_Capsule::Create(const vector<_float3>& vecVTXPositions)
{
	CBounding_Capsule* pInstance = new CBounding_Capsule();

	if (FAILED(pInstance->Initialize(vecVTXPositions)))
	{
		MSG_BOX("Failed to Created : CBounding_Capsule");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBounding_Capsule::Free()
{
    __super::Free();

	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}

HRESULT CBounding_Capsule::Render_Inspector(CBounding::BOUNDING_DESC* pDesc)
{
	auto _pDesc = static_cast<CAPSULE_DESC*>(pDesc);
	if (ImGui::DragFloat3("Center", reinterpret_cast<_float*>(&m_pOriginalDesc->Center),		0.01f)) {
		_pDesc->vCenter		= m_pOriginalDesc->Center;
	}
	if (ImGui::DragFloat("Radius", reinterpret_cast<_float*>(&m_pOriginalDesc->Radius),			0.01f)) {
		_pDesc->fRadius		= m_pOriginalDesc->Radius;
	}
	if (ImGui::DragFloat("HalfHeight", reinterpret_cast<_float*>(&m_pOriginalDesc->HalfHeight), 0.01f)) {
		_pDesc->fHalfHeight = m_pOriginalDesc->HalfHeight;
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
