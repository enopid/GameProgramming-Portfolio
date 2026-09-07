#include "Ozraeen_Body.h"
#include "Transform.h"
#include "Texture.h"
#include "DimensionSwitcher.h"
#include "VIBuffer_Rect.h"
#include "Boss.h"
#include "GameInstance.h"

COzraeen_Body::COzraeen_Body(LPDIRECT3DDEVICE9 pGraphic_Device) : COzraeen_Part(pGraphic_Device)
{
}

COzraeen_Body::COzraeen_Body(const COzraeen_Body& Prototype) : COzraeen_Part(Prototype)
{
}

HRESULT COzraeen_Body::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT COzraeen_Body::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;

	m_pTransformCom->Scale(64.f * g_fDot, 64.f * g_fDot, 1.f);

	m_pOLTextureCom->SetAnimationDuration(1.f);
	m_pTextureCom->SetAnimationDuration(1.f);
	return S_OK;
}

void COzraeen_Body::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void COzraeen_Body::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void COzraeen_Body::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT COzraeen_Body::Render()
{
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);
	CHKFAIL(__super::Render());
	InheritPositon_Begin();

	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_AnimTexture()))				return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))						return E_FAIL;

	_float _fBias = -0.000001f;
	m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&_fBias);
	if (FAILED(m_pOLTextureCom->Bind_AnimTexture()))	return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))						return E_FAIL;

	InheritPositon_End();

	return S_OK;
}

COzraeen_Body* COzraeen_Body::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	COzraeen_Body* pInstance = new COzraeen_Body(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : COzraeen_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COzraeen_Body::Clone(void* pArg)
{

	COzraeen_Body* pInstance = new COzraeen_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : COzraeen_Body");
		Safe_Release(pInstance);
	}

	return pInstance;


}

void COzraeen_Body::Free()
{
	__super::Free();
	Safe_Release(m_pOLTextureCom);
}

HRESULT COzraeen_Body::Ready_Components()
{
	if (FAILED(__super::Ready_Components())) return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE4), TEXT("Prototype_Component_Texture_Boss_Ozraeen_Blob"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE4), TEXT("Prototype_Component_Texture_Boss_Ozraeen_Blob_OL"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pOLTextureCom))))
		return E_FAIL;


	return S_OK;
}
