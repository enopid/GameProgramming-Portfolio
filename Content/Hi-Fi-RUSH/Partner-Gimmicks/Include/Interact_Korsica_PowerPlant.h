#pragma once
#include "Interact_RhythmChallenge.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CMeshRenderer;
NS_END
NS_BEGIN(Client)
class CInteract_Korsica_PowerPlant final : public CInteract_RhythmChallenge
{
	friend class CGameObject;
public:
	CInteract_Korsica_PowerPlant(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteract_Korsica_PowerPlant(const CInteract_Korsica_PowerPlant& Prototype);
	virtual ~CInteract_Korsica_PowerPlant() = default;
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
	WPShader	m_wpShader;
	weak_ptr<CMeshRenderer>	m_wpMeshRenderer[3];
	_float3 m_vColor = {};
};
NS_END