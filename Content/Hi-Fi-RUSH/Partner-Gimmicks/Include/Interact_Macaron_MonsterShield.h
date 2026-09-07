#pragma once
#include "Interact_RhythmChallenge.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CInteract_Macaron_MonsterShield final : public CInteract_RhythmChallenge
{
	friend class CGameObject;
public:
	CInteract_Macaron_MonsterShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteract_Macaron_MonsterShield(const CInteract_Macaron_MonsterShield& Prototype);
	virtual ~CInteract_Macaron_MonsterShield() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);

	virtual HRESULT Late_Initialize() override;

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	
	virtual void Render_Properties() override;

public:
	virtual void	Free() override;

	CGameObject* Clone(void* pArg) override;
	HRESULT Add_InitComponents() override;
private:
	float			m_fAmount = 0.f;
};
NS_END