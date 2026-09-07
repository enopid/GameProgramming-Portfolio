#include "Laser.h"
#include "GameInstance.h"

#include "Collider.h"
#include "Texture.h"
#include "Transform.h"

#include "Toodee.h"
#include "Topdee.h"
#include "PSystem_Dust.h"

#include "Korimlio.h"


CLaser::CLaser(LPDIRECT3DDEVICE9 pGraphic_Device) :CGameObject(pGraphic_Device)
{
}

CLaser::CLaser(const CLaser& Prototype)
	: CGameObject(Prototype),
	m_vecLayerMasks(Prototype.m_vecLayerMasks)
{

	if (!m_pSystemDust)
	{
		m_pSystemDust = new CPSystem_Dust(m_pGraphic_Device);
		m_pSystemDust->Initialize();
	}
}

HRESULT CLaser::Initialize_Prototype()
{
	m_vecLayerMasks.clear();
	m_vecLayerMasks.push_back(wstring(L"FIX"));
	m_vecLayerMasks.push_back(wstring(L"SPIKE"));
	m_vecLayerMasks.push_back(wstring(L"KINETIC"));
	m_vecLayerMasks.push_back(wstring(L"Player"));
	m_vecLayerMasks.push_back(wstring(L"Topdee"));
	m_vecLayerMasks.push_back(wstring(L"Boss"));

	return S_OK;
}

HRESULT CLaser::Initialize(void* pArg)
{
	FLaserDesc _desc = {};
	_desc = *static_cast<FLaserDesc*>(pArg);

	m_vCurDIr	= _desc.vDir;
	m_vInitPos	= _desc.vInitPos;

	if (FAILED(Ready_Components())) return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, _float3{ -0.5f, 0.f, 0.f });
	m_pPivotTransformCom->Set_State(STATE::POSITION, _float3{ +0.5f, 0.f, 0.f });
	m_pTransformCom->Scale(30.f * g_fDot, 8.f * g_fDot, 1.f);

	return S_OK;
}

void CLaser::Priority_Update(_float fTimeDelta)
{
	m_vecReflectInfos.clear();
	m_bReflect = true;
}

void CLaser::Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->IsKeyState('K', EKeyActionState::Stay)) {
		_float4x4 _m;
		_float3 _vAxis = { 0.f,0.f, 1.f };
		D3DXMatrixRotationAxis(&_m, &_vAxis, 0.05f);
		D3DXVec3TransformNormal(&m_vCurDIr, &m_vCurDIr, &_m);
	}
	if (m_pGameInstance->IsKeyState('J', EKeyActionState::Stay)) {
		_float4x4 _m;
		_float3 _vAxis = { 0.f,0.f, 1.f };
		D3DXMatrixRotationAxis(&_m, &_vAxis, -0.05f);
		D3DXVec3TransformNormal(&m_vCurDIr, &m_vCurDIr, &_m);
	}*/
	Shot();
}

void CLaser::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CLaser::Render()
{
	if (!IsActive()) return S_OK;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	float fBias = +0.000001f;
	m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fBias);

	for (auto [_vPos, _vDir] : m_vecReflectInfos) {
		m_pTransformCom->Set_State(STATE::POSITION, _vPos);
		m_pTransformCom->Rotation(_float3{0.f,0.f,-1.f }, -atan2(_vDir.y, _vDir.x));
		_float _length = D3DXVec3Length(&_vDir);

		m_pTransformCom->Scale(_length, g_fDot * 8.f, 0.f);
		if (FAILED(m_pPivotTransformCom->Bind_Resource()))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_Texture(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render())) 
			return E_FAIL;
	}

	fBias = 0.f;
	m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fBias);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CLaser::Ready_Components()
{

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Laser"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc{};
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	TransformDesc.parent = m_pTransformCom;
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_PivotTransform2"), reinterpret_cast<CComponent**>(&m_pPivotTransformCom), &TransformDesc)))
		return E_FAIL;
	m_pPivotTransformCom->SetParent(&TransformDesc);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CLaser* CLaser::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	CLaser* pInstance = new CLaser(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CLaser");
		Safe_Release(pInstance);
	}

	return pInstance;


}

CGameObject* CLaser::Clone(void* pArg)
{

	CLaser* pInstance = new CLaser(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CLaser");
		Safe_Release(pInstance);
	}

	return pInstance;


}

void CLaser::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPivotTransformCom);
	Safe_Release(m_pVIBufferCom);
}

void CLaser::Shot()
{
	m_vecReflectInfos.push_back({ m_vInitPos, m_vCurDIr });

	while (Move());
}

//벡터의 마지막 요소의 이동 거리를 계산
bool CLaser::Move()
{
	auto [_vPos, _vDir] = m_vecReflectInfos.back();

	_float2 _vRayDir = { _vDir.x, _vDir.y };
	D3DXVec2Normalize(&_vRayDir, &_vRayDir);
	_float2 _vRayPos = _float2{ _vPos.x , _vPos.y } + 0.01f * _vRayDir;


	FRayCastHit _hit;
	_bool		_bReflect = true;
	if (m_pGameInstance->RayCast(_vRayPos, _vRayDir, 100.f, &_hit, m_vecLayerMasks)) {
		m_vecReflectInfos.back().second *= _hit.dist;
		//test 
		_bReflect = true;
		auto _pToodee = dynamic_cast<CToodee*>(_hit.pObject);
		if (_pToodee) {
			_pToodee->Set_IsDead();
			_bReflect = false;
		}
		auto _pTopdee = dynamic_cast<CTopdee*>(_hit.pObject);
		if (_pTopdee) {
			_pTopdee->Set_IsDead();
			_bReflect = false;
		}
		auto _pKorimlio = dynamic_cast<CKorimlio*>(_hit.pObject);
		if (_pKorimlio) {
			
		}
	}
	else {
		m_vecReflectInfos.back().second *= 100.f;
		_bReflect = false;

		_float _fOffset = D3DX_PI * 2.f / 9;
		
		_float _dirAngle = atan2(_hit.normal.y, _hit.normal.x);
		m_pSystemDust->SetSystem(1, 0.3f, 6.f, 5.f, .5f, 0.f, _dirAngle - _fOffset, _dirAngle + _fOffset);
		_float3 _vPos = { 0.f,0.f,0.f };
		_vPos.x = _hit.contactPoint.x;
		_vPos.y = _hit.contactPoint.y;
		m_pSystemDust->Trigger(_vPos);
	}

	if (m_vecReflectInfos.size() > MAXREFLECTION) { 
		_bReflect = false;
		_float _fOffset = D3DX_PI * 2.f / 36 * 2;
		_float _dirAngle = atan2(_hit.normal.y, _hit.normal.x);
		
		m_pSystemDust->SetSystem(1, 0.3f, 6.f, 4.f, .5f, 0.f, _dirAngle - _fOffset, _dirAngle + _fOffset);
		_float3 _vPos = { 0.f,0.f,0.f };
		_vPos.x = _hit.contactPoint.x;
		_vPos.y = _hit.contactPoint.y;
		m_pSystemDust->Trigger(_vPos);
	}

	_float3 _vNormal = { _hit.normal.x, _hit.normal.y, 0.f };
	if (_bReflect && D3DXVec3Dot(&_vDir, &_vNormal)<0) {

		D3DXVec3Normalize(&_vDir, &_vDir);
		_float3 _vOut = _vDir - 2.f * D3DXVec3Dot(&_vDir, &_vNormal) * _vNormal;
		_float3 _vContact = { _hit.contactPoint.x, _hit.contactPoint.y, 0.f };

		D3DXVec3Normalize(&_vOut, &_vOut);
		m_vecReflectInfos.push_back({ _vContact , _vOut });
	}
	return _bReflect;
}
