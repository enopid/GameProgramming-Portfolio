#include "Hole.h"
#include "GameInstance.h"
#include "Collider.h"
#include "Block.h"

CHole::CHole(LPDIRECT3DDEVICE9 pGraphic_Deivce)
	: CNonBlock { pGraphic_Deivce }
{}
CHole::CHole(const CHole& Prototype)
	: CNonBlock( Prototype )
{}
HRESULT CHole::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CHole::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + _float3{ 0.f,0.f,+1.0f });
	m_vInitPos = m_pTransformCom->Get_State(STATE::POSITION);
	
	return S_OK;
}
void CHole::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CHole::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	
}
void CHole::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}
HRESULT CHole::Render()
{
	if (!m_pColliderCom->IsActive()) return S_OK;

	CRenderStateGaurd rsGuard(m_pGraphic_Device);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTEXOPCAPS_SELECTARG2);
	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DXCOLOR(0.f, 0.f, 0.f, 1.f));
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

	__super::Render();

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if(FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}
HRESULT CHole::Reset()
{
	//CHKFAIL(__super::Reset())


	m_pColliderCom->SetActive(true);
	return S_OK;
}
HRESULT	CHole::Ready_Components()
{
	__super::Ready_Components();

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	//Test
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Box"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_Collider */
	CCollider::FColliderDesc ColliderDesc{};
	ColliderDesc.bInitialActive = true;
	ColliderDesc.bIsTrigger = false;
	ColliderDesc.func = [=](FCollisionInfo _fCollisionInfo) { OnCollision(_fCollisionInfo); };
	ColliderDesc.iLayerLevelIndex = 0;
	ColliderDesc.pObject = this;
	ColliderDesc.pTransform = dynamic_cast<CTransform*>(Get_Component(TEXT("Com_Transform")));
	ColliderDesc.strCollisionLayerTag = L"HOLE";
	
	FRectColliderInfo _collisionInfo;
	_collisionInfo.fHeight = 1.f;
	_collisionInfo.fWidth = 1.f;
	_collisionInfo.vCenter = _float2{ 0,0 };
	ColliderDesc.colliderInfo = _collisionInfo;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}
void CHole::OnCollision(FCollisionInfo _fCollisionInfo)
{
	auto [_state, _fRatio] = m_pGameInstance->Get_DimensionInfo();
	if (_state != EDimensionState::TOPDIM) return;
	if (_fCollisionInfo.strCollisionLayerTag == L"KINETIC") {
		auto _pTransform = static_cast<CTransform*>(_fCollisionInfo.pObject->Get_Component(L"Com_Transform"));
		auto _vDist = (_pTransform->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION));
		_vDist.z = 0;
		
		if (D3DXVec3Length(&_vDist) < HOLEFALLTHRESHOLD) {
			auto pBlock = static_cast<CBlock*>(_fCollisionInfo.pObject);
			pBlock->HoleFall();
			m_pColliderCom->SetActive(false);
		}
		
	}
}

CHole* CHole::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHole* pInstance = new CHole(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CHole");
		Safe_Release(pInstance);
	}

	return pInstance;
}
PickingInfo* CHole::Picking(RAY tRay)
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
CGameObject* CHole::Clone(void* pArg)
{
	CHole* pInstance = new CHole(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CHole");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CHole::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}