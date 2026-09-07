#include "Ozraeen_Head.h"
#include "Transform.h"
#include "Texture.h"
#include "DimensionSwitcher.h"
#include "VIBuffer_Rect.h"
#include "Boss.h"
#include "GameInstance.h"

COzraeen_Head::COzraeen_Head(LPDIRECT3DDEVICE9 pGraphic_Device) : COzraeen_Part(pGraphic_Device)
{
}

COzraeen_Head::COzraeen_Head(const COzraeen_Head& Prototype) : COzraeen_Part(Prototype)
{
}

HRESULT COzraeen_Head::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT COzraeen_Head::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;

	return S_OK;
}

void COzraeen_Head::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void COzraeen_Head::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void COzraeen_Head::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT COzraeen_Head::Render()
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

COzraeen_Head* COzraeen_Head::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	COzraeen_Head* pInstance = new COzraeen_Head(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : COzraeen_Head");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COzraeen_Head::Clone(void* pArg)
{

	COzraeen_Head* pInstance = new COzraeen_Head(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : COzraeen_Head");
		Safe_Release(pInstance);
	}

	return pInstance;


}

void COzraeen_Head::Free()
{
	__super::Free();
}

_uint COzraeen_Head::GetTextureIdx()
{
	auto _vPos = m_pBaseTransform->Get_State(STATE::POSITION);
	auto _fAngle = atan2(m_vOffset.z - _vPos.z, (m_bFlip) ? -(m_vOffset.x - _vPos.x) : (m_vOffset.x - _vPos.x));
	_float _fAngleOffset = (2.f * D3DX_PI / 13);
	_fAngle -= D3DX_PI * 0.5f + _fAngleOffset * 1.5f;

	_fAngle = wrap(_fAngle, 0.f, 2 * D3DX_PI);

	return min(_uint(_fAngle / _fAngleOffset), 12);
}

HRESULT COzraeen_Head::Ready_Components()
{
	if (FAILED(__super::Ready_Components())) return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE4), TEXT("Prototype_Component_Texture_Boss_Ozraeen_Head"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pTransformCom->Scale(100.f / g_iCubicDot, 100.f / g_iCubicDot, 1.f);

	return S_OK;
}
