#pragma once
#include "Block.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
struct FCollisionInfo;
NS_END

NS_BEGIN(Client)
class CTopdee;

class CBlock_Lightning final  : public CBlock
{
private:
	CBlock_Lightning(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBlock_Lightning(const CBlock_Lightning& Prototype);
	virtual ~CBlock_Lightning() = default;
public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;
	void Priority_Update(_float fTimeDelta)	override;
	void Update(_float fTimeDelta)			override;
	void Late_Update(_float fTimeDelta)		override;
	HRESULT Render()						override;

	void OnLightning() { m_bOnLightning = true; };
private:
	HRESULT	Ready_Components();
public:
	static CBlock_Lightning* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	void Free() override;

	// CBlock을(를) 통해 상속됨
	virtual Engine::CGameObject* Clone(void* pArg);
private:
	_bool m_bOnLightning = false;
	_bool m_bOnBlink = false;
	UniqueTimer m_pLightningTimer = { nullptr };

};
NS_END
