#pragma once

#include "Client_Defines.h"
#include "NonBlock.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
struct FCollisionInfo;
NS_END

NS_BEGIN(Client)

class CHole final : public CNonBlock
{
	inline static _float HOLEFALLTHRESHOLD = .1f;
private:
	explicit CHole(LPDIRECT3DDEVICE9 pGraphic_Deivce);
	explicit CHole(const CHole& Prototype);
	virtual ~CHole() = default;

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;
	void Priority_Update(_float fTimeDelta)	override;
	void Update(_float fTimeDelta)			override;
	void Late_Update(_float fTimeDelta)		override;
	HRESULT Render()						override;

	HRESULT Reset() override;
private:
	HRESULT	Ready_Components();
	void OnCollision(FCollisionInfo _fCollisionInfo);

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

public:
	static CHole* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual Engine::CGameObject* Clone(void* pArg);
	void Free() override;

	virtual PickingInfo* Picking(RAY tRay);
};

NS_END