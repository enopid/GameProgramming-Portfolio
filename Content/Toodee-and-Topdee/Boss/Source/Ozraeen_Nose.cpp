#include "Ozraeen_Nose.h"
#include "Transform.h"
#include "Texture.h"
#include "DimensionSwitcher.h"
#include "VIBuffer_Rect.h"
#include "Boss.h"
#include "GameInstance.h"

COzraeen_Nose::COzraeen_Nose(LPDIRECT3DDEVICE9 pGraphic_Device) : COzraeen_Part(pGraphic_Device)
{
}

COzraeen_Nose::COzraeen_Nose(const COzraeen_Nose& Prototype) : COzraeen_Part(Prototype)
{
}

HRESULT COzraeen_Nose::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

	return S_OK;
}

HRESULT COzraeen_Nose::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) return E_FAIL;

	return S_OK;
}

void COzraeen_Nose::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void COzraeen_Nose::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void COzraeen_Nose::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT COzraeen_Nose::Render()
{
	CRenderStateGaurd _rsGuard(m_pGraphic_Device);
	CHKFAIL(__super::Render())
	//if (_textureidx > 8) return S_OK;

	InheritPositon_Begin();
	_uint _textureidx = GetTextureIdx();
	if (_textureidx <= 8) {
		if (FAILED(m_pTransformCom->Bind_Resource()))				return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_Texture(_textureidx)))	return E_FAIL;

		if (FAILED(m_pVIBufferCom->Bind_Buffers()))					return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))						return E_FAIL;
	}

	InheritPositon_End();

	return S_OK;
}

COzraeen_Nose* COzraeen_Nose::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

	COzraeen_Nose* pInstance = new COzraeen_Nose(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : COzraeen_Nose");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COzraeen_Nose::Clone(void* pArg)
{

	COzraeen_Nose* pInstance = new COzraeen_Nose(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : COzraeen_Nose");
		Safe_Release(pInstance);
	}

	return pInstance;


}

void COzraeen_Nose::Free()
{
	__super::Free();
}

_uint COzraeen_Nose::GetTextureIdx()
{
	auto _vPos = m_pBaseTransform->Get_State(STATE::POSITION);
	auto _fAngle = atan2(m_vOffset.z - _vPos.z, (m_bFlip) ? -(m_vOffset.x - _vPos.x) : (m_vOffset.x - _vPos.x));
	_float _fAngleOffset = (2.f * D3DX_PI / 16);
	_fAngle -= (D3DX_PI- D3DX_PI/12);
	_fAngle = wrap(_fAngle, 0.f, 2 * D3DX_PI);
	return min(_uint(_fAngle / _fAngleOffset), 16);
}

HRESULT COzraeen_Nose::Ready_Components()
{
	if (FAILED(__super::Ready_Components())) return E_FAIL;

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::BOSS_STAGE4), TEXT("Prototype_Component_Texture_Boss_Ozraeen_Nose"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	m_pTransformCom->Scale(100.f / g_iCubicDot, 100.f / g_iCubicDot, 1.f);

	return S_OK;
}
