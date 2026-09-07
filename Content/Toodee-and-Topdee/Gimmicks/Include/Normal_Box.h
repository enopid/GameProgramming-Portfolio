#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
class CCollider;
NS_END

NS_BEGIN(Client)

class CNormal_Box final : public CGameObject
{
private:
	explicit CNormal_Box(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CNormal_Box(const CNormal_Box& Prototype);
	virtual ~CNormal_Box() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Cube* m_pVIBufferCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	HRESULT	Ready_Components();
public:
	static CNormal_Box* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg)override;
	virtual void Free() override;

	virtual PickingInfo* Picking(RAY tRay);
private:
	_uint m_iTextureIdx = { 0 };

};

NS_END