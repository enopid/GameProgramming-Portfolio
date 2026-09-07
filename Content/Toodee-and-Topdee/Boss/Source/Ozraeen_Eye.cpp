#include "Ozraeen_Eye.h"
#include "Transform.h"
#include "Texture.h"
#include "DimensionSwitcher.h"
#include "VIBuffer_Rect.h"
#include "Boss.h"
#include "GameInstance.h"

COzraeen_Eye::COzraeen_Eye(LPDIRECT3DDEVICE9 pGraphic_Device) : COzraeen_Part(pGraphic_Device)
{
}

COzraeen_Eye::COzraeen_Eye(const COzraeen_Eye& Prototype) : COzraeen_Part(Prototype)
{
}

HRESULT COzraeen_Eye::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT COzraeen_Eye::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;
	m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + _float3{0.f, 0.f, -0.1f});
	return S_OK;
}

void COzraeen_Eye::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void COzraeen_Eye::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void COzraeen_Eye::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT COzraeen_Eye::Render()
{
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);
	CHKFAIL(__super::Render())
	InheritPositon_Begin();

	if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;

 	if (FAILED(m_pTextureCom->Bind_Texture(GetTextureIdx())))	return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))						return E_FAIL;

	InheritPositon_End();

	return S_OK;
}

COzraeen_Eye* COzraeen_Eye::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	COzraeen_Eye* pInstance = new COzraeen_Eye(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : COzraeen_Eye");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COzraeen_Eye::Clone(void* pArg)
{

	COzraeen_Eye* pInstance = new COzraeen_Eye(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : COzraeen_Eye");
		Safe_Release(pInstance);
	}

	return pInstance;


}

void COzraeen_Eye::Free()
{
	__super::Free();
}

_uint COzraeen_Eye::GetTextureIdx()
{
	auto _vPos = m_pBaseTransform->Get_State(STATE::POSITION);
	auto _fAngle = atan2(m_vOffset.z - _vPos.z, (m_bFlip) ? -(m_vOffset.x - _vPos.x) : (m_vOffset.x - _vPos.x));
	_float _fAngleOffset = (2.f * D3DX_PI / 13);
	_fAngle -= D3DX_PI * 0.5f + _fAngleOffset * 1.5f;

	_fAngle = wrap(_fAngle, 0.f, 2 * D3DX_PI);

	return min(_uint(_fAngle / _fAngleOffset), 12);
}

HRESULT COzraeen_Eye::Ready_Components()
{
	if (FAILED(__super::Ready_Components())) return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE4), TEXT("Prototype_Component_Texture_Boss_Ozraeen_Eye"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pTransformCom->Scale(100.f / g_iCubicDot, 100.f / g_iCubicDot, 1.f);

	return S_OK;
}
