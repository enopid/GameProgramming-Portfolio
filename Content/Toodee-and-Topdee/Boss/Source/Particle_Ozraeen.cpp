#include "Particle_Ozraeen.h"
#include"GameInstance.h"


CParticle_Ozraeen::CParticle_Ozraeen(LPDIRECT3DDEVICE9 pGraphic_Device) : CParticle{ pGraphic_Device }
{
}

CParticle_Ozraeen::CParticle_Ozraeen(const CParticle_Ozraeen& Prototype) : CParticle(Prototype)
{
}
HRESULT CParticle_Ozraeen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Ozraeen::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_pParticleLifeTimeTimer = m_pGameInstance->CreateTimerWithDuration(1.f, false, [this]() {m_ParticleAttr._isAlive = false; }, false);

	return S_OK;
}

void CParticle_Ozraeen::Priority_Update(_float fTimeDelta)
{

}

void CParticle_Ozraeen::Update(_float fTimeDelta)
{

	__super::Update(fTimeDelta);

		
}

void CParticle_Ozraeen::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CParticle_Ozraeen::Render()
{
	if (!m_ParticleAttr._isAlive) return S_OK;

	CTransformGaurd _trGuard(m_pTransformCom);
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);

	if (m_bBlink) {
		m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	}

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0x10);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_AnimTexture()))				return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))						return E_FAIL;

	auto _vScale	= m_pTransformCom->Get_Scaled();
	float _InScale = (64 * _vScale.x - 6 * 3) / (64.f - 6);
	_InScale = max(_InScale, 0.1f);
	m_pTransformCom->Scale(_InScale, _InScale, _vScale.z);

	_float _fBias = -0.000001f;
	m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&_fBias);

	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;

	if (FAILED(m_pOLTextureCom->Bind_AnimTexture()))	return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))						return E_FAIL;

	return S_OK;

}

HRESULT CParticle_Ozraeen::Ready_Components()
{
	__super::Ready_Components();

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE4), TEXT("Prototype_Component_Texture_Boss_Ozraeen_Blob"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE4), TEXT("Prototype_Component_Texture_Boss_Ozraeen_Blob_OL"),
		TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pOLTextureCom))))
		return E_FAIL;

	return S_OK;
}



CParticle_Ozraeen* CParticle_Ozraeen::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CParticle_Ozraeen* pInstance = new CParticle_Ozraeen(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Particle_Dust");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CParticle_Ozraeen::Clone(void* pArg)
{
	CParticle_Ozraeen* pInstance = new CParticle_Ozraeen(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPSystem_Ozraeen");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CParticle_Ozraeen::Free()
{
	__super::Free();

	Safe_Release(m_pOLTextureCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}

void CParticle_Ozraeen::Respawn(_float3 vPos, _float3 vDir, _float fSpeed, _float fSize)
{
	__super::Respawn(vPos, vDir, fSpeed, fSize);
}
