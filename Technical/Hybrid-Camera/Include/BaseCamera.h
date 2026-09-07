#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)


class ENGINE_DLL CBaseCamera abstract : public CGameObject
{
protected:
	CBaseCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBaseCamera(const CBaseCamera& Prototype);
	virtual ~CBaseCamera() = default;
public:
	virtual HRESULT Initialize_Prototype() override; /* 원형을 위한 초기화(서버, 파일) */
	virtual HRESULT Initialize(void* pArg) override; /* 사본객체를 위한 초기화 */
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;

protected:
	virtual void SetViewMatrix() PURE;
	virtual void SetProjMatrix() PURE;
	virtual void SetCamera();
protected:
	_float4x4 m_pViewMatrix;
	_float4x4 m_pProjMatrix;

	_float	m_fNearPlane	= 1.f;
	_float	m_fFarPlane		= 2000.f;

	class CTransform* m_pTransformCom = {nullptr};
};

NS_END

