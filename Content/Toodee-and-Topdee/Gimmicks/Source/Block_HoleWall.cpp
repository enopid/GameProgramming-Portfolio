#include "Block_HoleWall.h"
#include "Transform.h"
#include "GameInstance.h"

CBlock_HoleWall::CBlock_HoleWall(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlock( pGraphic_Device )
{
}

CBlock_HoleWall::CBlock_HoleWall(const CBlock_HoleWall& Prototype)
	: CBlock(Prototype)
{
	m_iBlockFlag = EBLOCKFLAG::BF_FIX;
	m_iTextureIdx = 0;
}

HRESULT CBlock_HoleWall::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBlock_HoleWall::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg == nullptr) _float3 _vPos = *reinterpret_cast<_float3*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, *reinterpret_cast<_float3*>(pArg));
	m_pColliderCom->SetActive(false);

	return S_OK;
}

void CBlock_HoleWall::Priority_Update(_float fTimeDelta)
{
}

void CBlock_HoleWall::Update(_float fTimeDelta)
{
}

void CBlock_HoleWall::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CBlock_HoleWall::Render()
{
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	CHKFAIL(__super::Render());

	return S_OK;
}

HRESULT CBlock_HoleWall::Ready_Components()
{
	__super::Ready_Components();

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_HoleWall"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CBlock_HoleWall* CBlock_HoleWall::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlock_HoleWall* pInstance = new CBlock_HoleWall(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBlock_Hole");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlock_HoleWall::Free()
{
	__super::Free();
}

Engine::CGameObject* CBlock_HoleWall::Clone(void* pArg)
{
	CBlock_HoleWall* pInstance = new CBlock_HoleWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBlock_Hole");
		Safe_Release(pInstance);
	}

	return pInstance;


}
