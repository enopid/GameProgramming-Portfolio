#pragma once
#include "Interact_RhythmChallenge.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CEffectRoot;
NS_END

NS_BEGIN(Client)
class CInteract_Korsica_Firegate final : public CInteract_RhythmChallenge
{
	friend class CGameObject;
public:
	CInteract_Korsica_Firegate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteract_Korsica_Firegate(const CInteract_Korsica_Firegate& Prototype);
	virtual ~CInteract_Korsica_Firegate() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);

	virtual HRESULT Late_Initialize() override;

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	virtual void Render_Properties() override;

	void Add_Listener();
	_float3 Get_CurrentPos();
	_bool IsOutofRange();
	
public:
	virtual void	Free() override;

	CGameObject* Clone(void* pArg) override;
	HRESULT Add_InitComponents() override;

private:
	_bool m_bIsReboot{};
	_bool m_bRebootComplete{};

	_float m_fSoundCullingRange{};

	class CUC_UIEffect* m_pUCEffect = { nullptr };

	WPCollidor			m_wpCollidor_FireHit;
	WPCollidor			m_wpCollidor_FireBody;

	CEffectRoot*		m_pEffect	= nullptr;
};
NS_END