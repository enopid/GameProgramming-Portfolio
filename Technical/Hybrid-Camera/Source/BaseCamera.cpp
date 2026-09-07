#include "BaseCamera.h"
#include "Transform.h"

CBaseCamera::CBaseCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CBaseCamera::CBaseCamera(const CBaseCamera& Prototype)
	: CGameObject( Prototype )
{
}

HRESULT CBaseCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBaseCamera::Initialize(void* pArg)
{
	return S_OK;
}

void CBaseCamera::Priority_Update(_float fTimeDelta)
{
}

void CBaseCamera::Update(_float fTimeDelta)
{
}

void CBaseCamera::Late_Update(_float fTimeDelta)
{
}


void CBaseCamera::Free()
{
	__super::Free();
	Safe_Release(m_pTransformCom);
}

void CBaseCamera::SetCamera()
{
	SetViewMatrix();
	SetProjMatrix();
}
