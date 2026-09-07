#include "Turret.h"
#include "GameInstance.h"
#include "Transform.h"
#include "FirePellet.h"
#include "Laser.h"

#include "ParticleManager.h"


CTurret::CTurret(LPDIRECT3DDEVICE9 pGraphic_Deivce, ETURRETTYPE _turretType)
	: CNonBlock{ pGraphic_Deivce }, m_turretType(_turretType)
{
}

CTurret::CTurret(const CTurret& Prototype)
	: CNonBlock( Prototype ), m_turretType(Prototype.m_turretType)
{
}

HRESULT CTurret::Initialize_Prototype()
{
 	return S_OK;
}

HRESULT CTurret::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PairInfo pairInfo = *reinterpret_cast<PairInfo*>(pArg);

	
	m_iCurDIr = get<FObjectType2>(pairInfo.second).m_vDirection;

	_float3 _offset = { 0.f, 0.f, 0.f };
	if		(m_iCurDIr == 0) _offset = { +0.f, +1.f, 0.f };
	else if (m_iCurDIr == 1) _offset = { +0.f, -1.f, 0.f };
	else if (m_iCurDIr == 2) _offset = { -1.f, +0.f, 0.f };
	else if (m_iCurDIr == 3) _offset = { +1.f, +0.f, 0.f };

	m_pPivotTransformCom1->Set_State(STATE::POSITION, _offset * -.5f);
	m_pPivotTransformCom2->Set_State(STATE::POSITION, _offset * +.5f);
	
	auto _func = [=]() { m_pSystemDust->Trigger(m_pPivotTransformCom1->Get_WorldState(STATE::POSITION)); if (m_turretType == ETURRETTYPE::FIREPELLET) ShotFirePellet(); };
	m_pScaleTimer = m_pGameInstance->CreateTimerWithFrequency(SCALEFREQUENCY, true, _func, true);

	if (m_turretType == ETURRETTYPE::LASER) {
		if (FAILED(CreateLaser(pairInfo))) return E_FAIL;
	}
	else if (m_turretType == ETURRETTYPE::DIAGLASER) {
		if (FAILED(CreateLaser(pairInfo, true))) return E_FAIL;
	}
	else {
		if (FAILED(CreateFirePelletPool(pairInfo))) return E_FAIL;
	}

	if (!m_pSystemDust)
	{
		m_pSystemDust = new CPSystem_Dust(m_pGraphic_Device);
		m_pSystemDust->Initialize();
		_float _fOffset =  D3DX_PI * 2.f / 9;
		_float _dirAngle = D3DX_PI/2.f;
		if (m_iCurDIr == 0)			m_pSystemDust->SetSystem(6, 0.3f, 7.f, 5.f, .5f, 0.f, _dirAngle * 1 - _fOffset, _dirAngle * 1 + _fOffset);
		else if (m_iCurDIr == 1)	m_pSystemDust->SetSystem(6, 0.3f, 7.f, 5.f, .5f, 0.f, _dirAngle * 3 - _fOffset, _dirAngle * 3 + _fOffset);
		else if (m_iCurDIr == 2)	m_pSystemDust->SetSystem(6, 0.3f, 7.f, 5.f, .5f, 0.f, _dirAngle * 2 - _fOffset, _dirAngle * 2 + _fOffset);
		else if (m_iCurDIr == 3)	m_pSystemDust->SetSystem(6, 0.3f, 7.f, 5.f, .5f, 0.f, _dirAngle * 0 - _fOffset, _dirAngle * 0 + _fOffset);
	}

	return S_OK;
}
void CTurret::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CTurret::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}
void CTurret::AdjustTurretScale()
{
	if (m_pScaleTimer) {
		_float _ratio = m_pScaleTimer->GetElapsedRatio();

		_float _fScale = pow(2, sin(_ratio * D3DX_PI * 2));

		_float2 _scale = { 1.f, 1.f };
		_scale.x = (m_iCurDIr >= 2) ? _fScale : 1.f / _fScale;
		_scale.y = (m_iCurDIr <  2) ? _fScale : 1.f / _fScale;

		m_pPivotTransformCom1->Scale(16.f * g_fDot * _scale.x, 16.f * g_fDot * _scale.y, 16.f * g_fDot * 1.f);
	}
}

void CTurret::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}
HRESULT CTurret::Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);


	//Draw Laser
	/*if (m_turretType == ETURRETTYPE::LASER) {
		m_pGraphic_Device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		m_pGraphic_Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		m_pGraphic_Device->SetRenderState(D3DRS_STENCILREF, 1);
		m_pGraphic_Device->SetRenderState(D3DRS_STENCILMASK, 0xFF);
		m_pGraphic_Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFF);

		m_pGraphic_Device->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
		m_pGraphic_Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

		AdjustLaserScale();
		float fBias = +0.000001f;
		m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fBias);
		if (FAILED(m_pPivotTransformCom2->Bind_Resource()))
			return E_FAIL;
		if (FAILED(m_pLaserTexture->Bind_Texture(m_iCurDIr)))
			return E_FAIL;
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
		fBias = 0.f;
		m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fBias);



		m_pGraphic_Device->SetRenderState(D3DRS_COLORWRITEENABLE,
			D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
		m_pGraphic_Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		m_pGraphic_Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);


		AdjustLaserScale();
		m_pPivotTransformCom1->Scale(g_fDot * 200, g_fDot * 30, 1.f * g_fDot);
		fBias = +0.000001f;
		m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fBias);
		if (FAILED(m_pPivotTransformCom2->Bind_Resource()))
			return E_FAIL;
		if (FAILED(m_pLaserTexture->Bind_Texture(m_iCurDIr)))
			return E_FAIL;
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
		fBias = 0.f;

		m_pGraphic_Device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}*/

	//Draw Turret
	AdjustTurretScale();
	


	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pPivotTransformCom2->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_Texture(m_iCurDIr)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}
HRESULT	CTurret::Ready_Components()
{
	__super::Ready_Components();

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Turret"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.parent = m_pTransformCom;
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_PivotTransform1"), reinterpret_cast<CComponent**>(&m_pPivotTransformCom1), &TransformDesc)))
		return E_FAIL;
	
	m_pPivotTransformCom1->SetParent(&TransformDesc);

	TransformDesc.parent = m_pPivotTransformCom1;
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_PivotTransform2"), reinterpret_cast<CComponent**>(&m_pPivotTransformCom2), &TransformDesc)))
		return E_FAIL;
	m_pPivotTransformCom2->SetParent(&TransformDesc);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CTurret* CTurret::Create(LPDIRECT3DDEVICE9 pGraphic_Device, ETURRETTYPE _TURRETTYPE)
{
	CTurret* pInstance = new CTurret(pGraphic_Device, _TURRETTYPE);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTurret");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTurret::Clone(void* pArg)
{
	CTurret* pInstance = new CTurret(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTurret");
		Safe_Release(pInstance);
	}

	return pInstance;
}
PickingInfo* CTurret::Picking(RAY tRay)
{
	_float4x4 matWorldInv{};

	RAY tLocalRay = {
		tRay.vPos,
		tRay.vDir
	};

	D3DXMatrixInverse(&matWorldInv, nullptr, m_pTransformCom->Get_WorldMatrixPtr());

	D3DXVec3TransformCoord(&tLocalRay.vPos, &tLocalRay.vPos, &matWorldInv);
	D3DXVec3TransformNormal(&tLocalRay.vDir, &tLocalRay.vDir, &matWorldInv);
	D3DXVec3Normalize(&tLocalRay.vDir, &tLocalRay.vDir);

	PickingInfo* pPickingInfo = m_pVIBufferCom->Picking(tLocalRay);
	if (pPickingInfo)
	{
		pPickingInfo->m_vCenterPos = m_pTransformCom->Get_State(STATE::POSITION);
		pPickingInfo->m_pGameObject = this;
	}

	return pPickingInfo;
}
void CTurret::ShotFirePellet()
{
	for (_uint i = 0; i < MAXNUMFIREPELLETS; i++) {
		if (!m_arrpFirePellets[i]->IsActive()) {
			m_arrpFirePellets[i]->Shot();
			return;
		}
	}
}

void CTurret::ShotLaser()
{
}

HRESULT CTurret::CreateFirePelletPool(PairInfo pairInfo)
{
	auto iPrototypeLevelIndex	= ENUM_TO_UINT(LEVEL::STATIC);
	auto strPrototypeTag		= TEXT("Prototype_GameObject_FirePellet");
	auto iLayerLevelIndex		= pairInfo.first.m_iLayerLevelIndex;
	auto strLayerTag			= pairInfo.first.m_strLayerTag;

	CFirePellet::FFirePelletDesc _desc;
	_desc.vInitPos	= m_pTransformCom->Get_WorldState(STATE::POSITION);
	_desc.iDir		= m_iCurDIr;
	for (_uint i = 0; i < MAXNUMFIREPELLETS; i++) {
		CFirePellet* _pObject;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag,
			iLayerLevelIndex, strLayerTag, &_desc, reinterpret_cast<CGameObject**>(&_pObject))))
			return E_FAIL;
		m_arrpFirePellets[i] = _pObject;
	}

	return S_OK;
}

HRESULT CTurret::CreateLaser(PairInfo pairInfo, bool _bIsDiag)
{
	auto iPrototypeLevelIndex = ENUM_TO_UINT(LEVEL::STATIC);
	auto strPrototypeTag = TEXT("Prototype_GameObject_Laser");
	auto iLayerLevelIndex = pairInfo.first.m_iLayerLevelIndex;
	auto strLayerTag = pairInfo.first.m_strLayerTag;

	CLaser::FLaserDesc _desc;
	_desc.vInitPos = m_pPivotTransformCom1->Get_WorldState(STATE::POSITION);
	
	_float3 _vRayDir = { 1.f, 0.f, 0.f };
	if		(m_iCurDIr == 0) _vRayDir = { +0.f, +1.f, 0.f };
	else if (m_iCurDIr == 1) _vRayDir = { -0.f, -1.f, 0.f };
	else if (m_iCurDIr == 2) _vRayDir = { -1.f, +0.f, 0.f };
	else if (m_iCurDIr == 3) _vRayDir = { +1.f, -0.f, 0.f };

	if (_bIsDiag) {
		if		(m_iCurDIr == 0) _vRayDir = { -1.f, +1.f, 0.f };
		else if (m_iCurDIr == 1) _vRayDir = { -1.f, -1.f, 0.f };
		else if (m_iCurDIr == 2) _vRayDir = { -1.f, +1.f, 0.f };
		else if (m_iCurDIr == 3) _vRayDir = { -1.f, +1.f, 0.f };
	}
	D3DXVec3Normalize(&_vRayDir, &_vRayDir);
	_desc.vDir = _vRayDir;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag,
		iLayerLevelIndex, strLayerTag, &_desc, reinterpret_cast<CGameObject**>(&m_pLaser))))
		return E_FAIL;

	return S_OK;
}

void CTurret::Free()
{
	__super::Free();

	CParticleManager::GetInstance()->Clear(m_pSystemDust);
	Safe_Release(m_pSystemDust);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPivotTransformCom1);
	Safe_Release(m_pPivotTransformCom2);

	Safe_Release(m_pLaser);
	for (_uint i = 0; i < MAXNUMFIREPELLETS; i++) Safe_Release(m_arrpFirePellets[i]);
}