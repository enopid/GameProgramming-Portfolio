#include "Block_Lightning.h"
#include "Topdee.h"
#include "Transform.h"
#include "Collider.h"
#include "GameInstance.h"

CBlock_Lightning::CBlock_Lightning(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlock( pGraphic_Device )
{
}

CBlock_Lightning::CBlock_Lightning(const CBlock_Lightning& Prototype)
	: CBlock(Prototype)
{
	m_iBlockFlag = EBLOCKFLAG::BF_LIFT;
}

HRESULT CBlock_Lightning::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBlock_Lightning::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;
	m_pLightningTimer = m_pGameInstance->CreateTimerWithDuration(.5f, true, [=]() {
		m_bOnBlink ^= true;
		_float _duration = (rand() % 1000) / 1000.f;
		_duration = 0.1f * _duration + 0.01f;
		m_pLightningTimer->SetDuration(_duration);
		}, false);
	return S_OK;
}

void CBlock_Lightning::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
	m_pLightningTimer->SetActive(m_bOnLightning);
	m_bOnLightning = false;
}

void CBlock_Lightning::Update(_float fTimeDelta)
{
	if (m_pLightningTimer->IsActive() && m_bOnBlink) {
		m_iTextureIdx = 1;
	}
	else {
		m_iTextureIdx = 0;
	}
	__super::Update(fTimeDelta);
}

void CBlock_Lightning::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	
}

HRESULT CBlock_Lightning::Render()
{
	__super::Render();
	return S_OK;
}

HRESULT CBlock_Lightning::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_LightningBlock"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CBlock_Lightning* CBlock_Lightning::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlock_Lightning* pInstance = new CBlock_Lightning(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBlock_Lightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlock_Lightning::Free()
{
	__super::Free();
}

Engine::CGameObject* CBlock_Lightning::Clone(void* pArg)
{
	CBlock_Lightning* pInstance = new CBlock_Lightning(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBlock_Lightning");
		Safe_Release(pInstance);
	}

	return pInstance;


}
