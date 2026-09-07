#pragma once
#include "Client_Defines.h"
#include "InteractObject.h"
NS_BEGIN(Engine)
class CMeshRenderer;
class CEffectRoot;
NS_END
NS_BEGIN(Client)
class CInteract_Peppermint_Switch : public CInteractObject
{
public:
	CInteract_Peppermint_Switch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteract_Peppermint_Switch(const CInteract_Peppermint_Switch& Prototype);
	virtual ~CInteract_Peppermint_Switch() = default;
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
	weak_ptr<CMeshRenderer>m_wpMeshRenderer;
	_float3 m_vColor = {};
	class CEffectRoot* m_pSwitchOnEffect = nullptr;
};

NS_END
