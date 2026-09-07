#include "Ozraeen_Mouth.h"
#include "Transform.h"
#include "Texture.h"
#include "DimensionSwitcher.h"
#include "VIBuffer_Rect.h"
#include "Boss.h"
#include "GameInstance.h"

COzraeen_Mouth::COzraeen_Mouth(LPDIRECT3DDEVICE9 pGraphic_Device) : COzraeen_Part(pGraphic_Device)
{
}

COzraeen_Mouth::COzraeen_Mouth(const COzraeen_Mouth& Prototype) : COzraeen_Part(Prototype)
{
}

HRESULT COzraeen_Mouth::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT COzraeen_Mouth::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;
	auto [_idx, _bIsUp] = *static_cast<pair<_uint, bool>*>(pArg);

	if (_idx == 0 || _idx == 4) {
		m_iCurTextureIdx = 2;
		if (_idx == 4) Flip();
	}
	else {
		m_iCurTextureIdx = 0;
	}

	if (_bIsUp) {
		m_iCurTextureIdx++;
		m_pTransformCom->Set_State(STATE::POSITION, _float3{ 0.f, 0.f, 0.1f});
	}
	else {
		
		m_pTransformCom->Set_State(STATE::POSITION, _float3{ 0.f, (5.f - abs(_int(_idx) - 2)) * -1.f / g_iCubicDot, 0.f });
	}


	return S_OK;
}

void COzraeen_Mouth::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void COzraeen_Mouth::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void COzraeen_Mouth::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT COzraeen_Mouth::Render()
{
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);
	CHKFAIL(__super::Render())
	InheritPositon_Begin();

	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;

 	if (FAILED(m_pTextureCom->Bind_Texture(m_iCurTextureIdx)))	return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))						return E_FAIL;

	InheritPositon_End();

	return S_OK;
}

COzraeen_Mouth* COzraeen_Mouth::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	COzraeen_Mouth* pInstance = new COzraeen_Mouth(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : COzraeen_Mouth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COzraeen_Mouth::Clone(void* pArg)
{

	COzraeen_Mouth* pInstance = new COzraeen_Mouth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : COzraeen_Mouth");
		Safe_Release(pInstance);
	}

	return pInstance;


}

void COzraeen_Mouth::Free()
{
	__super::Free();
}

HRESULT COzraeen_Mouth::Ready_Components()
{
	if (FAILED(__super::Ready_Components())) return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE4), TEXT("Prototype_Component_Texture_Boss_Ozraeen_Mouth"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pTransformCom->Scale(50.f / g_iCubicDot, 50.f / g_iCubicDot, 1.f);

	return S_OK;
}
