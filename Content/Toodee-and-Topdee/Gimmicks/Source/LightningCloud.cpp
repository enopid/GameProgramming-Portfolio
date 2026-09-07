#include "LightningCloud.h"
#include "Block_Lightning.h"
#include "Toodee.h"
#include "Topdee.h"
#include "GameInstance.h"

#include "PSystem_Rain.h"
#include "ParticleManager.h"

CLightningCloud::CLightningCloud(LPDIRECT3DDEVICE9 pGraphic_Deivce)
	: CNonBlock { pGraphic_Deivce }
{}
CLightningCloud::CLightningCloud(const CLightningCloud& Prototype)
	: CNonBlock( Prototype ),
	m_vecLayerMasks(Prototype.m_vecLayerMasks)
{}
HRESULT CLightningCloud::Initialize_Prototype()
{
	m_vecLayerMasks.clear();
	m_vecLayerMasks.push_back(wstring(L"FIX"));
	m_vecLayerMasks.push_back(wstring(L"SPIKE"));
	m_vecLayerMasks.push_back(wstring(L"KINETIC"));
	m_vecLayerMasks.push_back(wstring(L"Player"));
	m_vecLayerMasks.push_back(wstring(L"Topdee"));
	m_vecLayerMasks.push_back(wstring(L"Wall"));

	return S_OK;
}
HRESULT CLightningCloud::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_float3 _vPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vPos.z = ZOFFSET + 0.5f;
	m_pTransformCom->Set_State(STATE::POSITION, _vPos);

	m_fTopRainLength[0] = m_fTopRainLength[1] = m_fTopRainLength[2] = abs(ZOFFSET);

	m_pCloudTextureCom->SetAnimationDuration(CLOUDANIMDURATION);
	for (_uint i = 0; i < 3; i++){
		m_pLightningTextureCom[i]->SetAnimationDuration(LIGHTNINGANIMDURATION);
		m_pTopRainSplashTextureCom[i]->SetAnimationDuration(LIGHTNINGANIMDURATION);
		m_pTooRainSplashTextureCom[i]->SetAnimationDuration(LIGHTNINGANIMDURATION);
	}

	for (_uint i = 0; i < 3; i++)
	{
		if (!m_pSystemRain[i])
		{
			m_pSystemRain[i] = new CPSystem_Rain(m_pGraphic_Device);
			m_pSystemRain[i]->Initialize();
		}
	}

	return S_OK;
}
void CLightningCloud::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CLightningCloud::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	auto [_eState, _fRatio] = m_pGameInstance->Get_DimensionInfo();
	m_fRatio = _fRatio;
	m_eState = _eState;

	Calculate_RainLength();
	Calculate_RainDropPos();

	m_iTransparency = MAXTRANSPARANCEY * m_fRatio;

	m_pCloudTransformCom->Set_State(STATE::POSITION, _float3{ 0.f,(1.f - m_fRatio) * 4,0.f });

	for (_uint i = 0; i < 3; i++)
	{
		_float RainLength = (m_fRatio * (m_fTopRainLength[i] - m_fTooRainLength[i]) + m_fTooRainLength[i]);
		m_pLightningTransformCom[i]->Rotation({ -1.f,0.f,0.f }, m_fRatio * D3DX_PI / 2.f);
		m_pLightningTransformCom[i]->Scale(
			16.f * g_fDot,
			16.f * g_fDot * RainLength,
			1.f);
		m_pSystemRain[i]->SetParentTansform(m_pLightningTransformCom[i]);
	}
	
}
void CLightningCloud::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}
HRESULT CLightningCloud::Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0x80);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	__super::Render();

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	CHKFAIL(RenderCloud())
	CHKFAIL(RenderShadow())
	CHKFAIL(RenderLightning())
	CHKFAIL(RenderRainSplash())

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	return S_OK;
}
HRESULT CLightningCloud::RenderCloud()
{
	if (FAILED(m_pCloudTextureTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pCloudTextureCom->Bind_AnimTexture()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}
HRESULT CLightningCloud::RenderShadow()
{
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE,	TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF,			0x10);
	// Texture Calc
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	// Alpha Blend (Lower Transparency)
	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(m_iTransparency, 0, 0, 0));

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	if (FAILED(m_pShadowTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pCloudTextureCom->Bind_AnimTexture()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}
HRESULT CLightningCloud::RenderLightning()
{
	for (_uint i = 0; i < 3; i++)
	{
		CTransformGaurd _transGaurd(m_pLightningPivotTransformCom[i]);
		for (_uint j = 0; j < 1; j++)
		{
			if (FAILED(m_pLightningPivotTransformCom[i]->Bind_Resource()))
				return E_FAIL;

			if (FAILED(m_pLightningTextureCom[i]->Bind_AnimTexture()))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;

			m_pLightningPivotTransformCom[i]->Set_State(STATE::POSITION, m_pLightningPivotTransformCom[i]->Get_State(STATE::POSITION) + _float3{0.f, -1.f, 0.f});
		}
	}
	return S_OK;
}
HRESULT CLightningCloud::RenderRainSplash()
{
	for (_uint i = 0; i < 3; i++)
	{
		if (FAILED(m_pRainPivotTransformCom[i]->Bind_Resource()))
			return E_FAIL;

		if (m_eState == EDimensionState::TOPDIM || m_eState == EDimensionState::TOTOPDIM) {
			CHKFAIL(m_pTopRainSplashTextureCom[i]->Bind_AnimTexture())
		}
		else {
			CHKFAIL(m_pTooRainSplashTextureCom[i]->Bind_AnimTexture())
		}

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT	CLightningCloud::Ready_Components()
{
	__super::Ready_Components();

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CHKFAIL(SetTexture());
	CHKFAIL(SetTransform());

	/* Com_Collider */
	CCollider::FColliderDesc ColliderDesc{};
	ColliderDesc.bInitialActive = true;
	ColliderDesc.bIsTrigger = false;
	ColliderDesc.func = [=](FCollisionInfo _fCollisionInfo) { OnCollision(_fCollisionInfo); };
	ColliderDesc.iLayerLevelIndex = 0;
	ColliderDesc.pObject = this;
	ColliderDesc.pTransform = m_pShadowTransformCom;
	ColliderDesc.strCollisionLayerTag = L"Lightning";
	
	FRectColliderInfo _collisionInfo;
	_collisionInfo.fHeight = 1.f;
	_collisionInfo.fWidth = 3.f;
	_collisionInfo.vCenter = _float2{ 0,0 };
	ColliderDesc.colliderInfo = _collisionInfo;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}
HRESULT CLightningCloud::SetTexture()
{
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_LightningCloud"),
		TEXT("Com_CloudTexture"), reinterpret_cast<CComponent**>(&m_pCloudTextureCom))))
		return E_FAIL;

	for (_uint i = 0; i < 3; i++){
		if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_Lightning"),
			wstring(L"Com_LightningTexture") + to_wstring(i), reinterpret_cast<CComponent**>(&m_pLightningTextureCom[i]))))
			return E_FAIL;
		m_pLightningTextureCom[i]->Bind_Texture(rand() % 8);
	}


	for (_uint i = 0; i < 3; i++) {
		if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_TopRainSplash"),
			wstring(L"Com_TopRainSplashTexture") + to_wstring(i), reinterpret_cast<CComponent**>(&m_pTopRainSplashTextureCom[i]))))
			return E_FAIL;
		m_pTopRainSplashTextureCom[i]->Bind_Texture(rand() % 6);
	}
	for (_uint i = 0; i < 3; i++) {
		if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_TooRainSplash"),
			wstring(L"Com_TooRainSplashTexture") + to_wstring(i), reinterpret_cast<CComponent**>(&m_pTooRainSplashTextureCom[i]))))
			return E_FAIL;
		m_pTooRainSplashTextureCom[i]->Bind_Texture(rand() % 6);
	}

	return S_OK;
}
HRESULT CLightningCloud::SetTransform()
{
	CTransform::TRANSFORM_DESC		TransformDesc{0.f,0.f,nullptr};
	TransformDesc.parent = m_pTransformCom;
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_CloudTransform"), reinterpret_cast<CComponent**>(&m_pCloudTransformCom), &TransformDesc)))
		return E_FAIL;
	m_pCloudTransformCom->SetParent(&TransformDesc);

	TransformDesc.parent = m_pCloudTransformCom;
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_CloudTexTransform"), reinterpret_cast<CComponent**>(&m_pCloudTextureTransformCom), &TransformDesc)))
		return E_FAIL;
	m_pCloudTextureTransformCom->SetParent(&TransformDesc);
	m_pCloudTextureTransformCom->Scale(58 * g_fDot, 22 * g_fDot, 1.f);


	TransformDesc.parent = m_pTransformCom;
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_ShadowTransform"), reinterpret_cast<CComponent**>(&m_pShadowTransformCom), &TransformDesc)))
		return E_FAIL;
	m_pShadowTransformCom->SetParent(&TransformDesc);
	m_pShadowTransformCom->Set_State(STATE::POSITION, _float3{ 0.f,0.f, -ZOFFSET });
	m_pShadowTransformCom->Scale(58 * g_fDot, 22 * g_fDot, 1.f);


	for (_uint i = 0; i < 3; i++) {
		TransformDesc.parent = m_pCloudTransformCom;
		if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
			wstring(L"Com_LightningTransform") + to_wstring(i), reinterpret_cast<CComponent**>(&m_pLightningTransformCom[i]), &TransformDesc)))
			return E_FAIL;
		m_pLightningTransformCom[i]->SetParent(&TransformDesc);
		m_pLightningTransformCom[i]->Set_State(STATE::POSITION, _float3{ i * 1.f -1.f, 0.f, RAINOFFSET });

		TransformDesc.parent = m_pLightningTransformCom[i];
		if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
			wstring(L"Com_LightningPivotTransform") + to_wstring(i), reinterpret_cast<CComponent**>(&m_pLightningPivotTransformCom[i]), &TransformDesc)))
			return E_FAIL;
		m_pLightningPivotTransformCom[i]->SetParent(&TransformDesc);
		m_pLightningPivotTransformCom[i]->Set_State(STATE::POSITION, _float3{ 0.f, -0.5f, 0.f });


		TransformDesc.parent = nullptr;
		if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
			wstring(L"Com_RainPivotTransform") + to_wstring(i), reinterpret_cast<CComponent**>(&m_pRainPivotTransformCom[i]), &TransformDesc)))
			return E_FAIL;
	}
	return S_OK;
}
void CLightningCloud::Calculate_RainLength()
{
	for (_uint i = 0; i < 3; i++){
		_float2 _vRayDir = { 0.f, -1.f };
		auto _vPos = m_pLightningTransformCom[i]->Get_WorldState(STATE::POSITION);
		_float2 _vRayPos = _float2{ _vPos.x , _vPos.y };

		FRayCastHit _hit;
		if (m_pGameInstance->RayCast(_vRayPos, _vRayDir, 100.f, &_hit, m_vecLayerMasks)) {
			m_fTooRainLength[i] = _hit.dist - 0.1f;

			CToodee* pToodee = dynamic_cast<CToodee*>(_hit.pObject);
			if (pToodee) pToodee->Set_IsDead();
			auto _lightningBlock = dynamic_cast<CBlock_Lightning*>(_hit.pObject);
			if (_lightningBlock && m_eState!=EDimensionState::TOPDIM) _lightningBlock->OnLightning();
		}
		else {
			m_fTooRainLength[i] = 18.f;
		}
	}
}
void CLightningCloud::Calculate_RainDropPos()
{
	for (_uint i = 0; i < 3; i++)
	{
		auto _vPos = m_pLightningTransformCom[i]->Get_WorldState(STATE::POSITION);
		_vPos.y -= (m_fTooRainLength[i] - .5f) * (1 - m_fRatio);
		_vPos.z = + .49f;
		m_pRainPivotTransformCom[i]->Set_State(STATE::POSITION, _vPos);

	}
}
HRESULT CLightningCloud::Reset()
{
	
	return S_OK;
}
void CLightningCloud::OnCollision(FCollisionInfo _fCollisionInfo)
{
	if (_fCollisionInfo.strCollisionLayerTag == L"Topdee") {
		auto _pTopdee = static_cast<CTopdee*> (_fCollisionInfo.pObject);
		if (!_pTopdee->HoldingLightningBlock()) _pTopdee->Set_IsDead();
	}
	if (_fCollisionInfo.strCollisionLayerTag == L"KINETIC") {
		auto _lightningBlock = dynamic_cast<CBlock_Lightning*>(_fCollisionInfo.pObject);
		if (_lightningBlock && m_eState == EDimensionState::TOPDIM) _lightningBlock->OnLightning();
	}
}
CLightningCloud* CLightningCloud::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLightningCloud* pInstance = new CLightningCloud(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CLightningCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}
PickingInfo* CLightningCloud::Picking(RAY tRay)
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
CGameObject* CLightningCloud::Clone(void* pArg)
{
	CLightningCloud* pInstance = new CLightningCloud(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CLightningCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CLightningCloud::Free()
{
	__super::Free();

	for (_uint i = 0; i < 3; i++)
	{
		CParticleManager::GetInstance()->Clear(m_pSystemRain[i]);
		Safe_Release(m_pSystemRain[i]);
	}

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pCloudTextureCom);
	for (_uint i = 0; i < 3; i++) Safe_Release(m_pLightningTextureCom[i]);
	for (_uint i = 0; i < 3; i++) Safe_Release(m_pTopRainSplashTextureCom[i]);
	for (_uint i = 0; i < 3; i++) Safe_Release(m_pTooRainSplashTextureCom[i]);

	Safe_Release(m_pCloudTransformCom);
	Safe_Release(m_pCloudTextureTransformCom);
	Safe_Release(m_pShadowTransformCom);
	for (_uint i = 0; i < 3; i++) Safe_Release(m_pLightningPivotTransformCom[i]);
	for (_uint i = 0; i < 3; i++) Safe_Release(m_pLightningTransformCom[i]);
	for (_uint i = 0; i < 3; i++) Safe_Release(m_pRainPivotTransformCom[i]);
}