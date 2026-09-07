#pragma once
#include "Interact_RhythmChallenge.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CMeshRenderer;
NS_END
NS_BEGIN(Client)
class CInteract_Macaron_Platform final : public CInteract_RhythmChallenge
{
	friend class CGameObject;
public:
	CInteract_Macaron_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteract_Macaron_Platform(const CInteract_Macaron_Platform& Prototype);
	virtual ~CInteract_Macaron_Platform() = default;
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

	class CInteract_Electric_CirclePlatform* m_pMacaron_Platform = nullptr;

	weak_ptr<CMeshRenderer>m_wpMeshRenderer;
	_float3 m_vColor = {};
};
NS_END