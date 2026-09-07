#pragma once
#include "InteractObject.h"
#include "Time_Manager_API.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CInteract_RhythmChallenge abstract : public CInteractObject
{
	friend class CGameObject;

public:
	CInteract_RhythmChallenge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteract_RhythmChallenge(const CInteract_RhythmChallenge& Prototype);
	virtual ~CInteract_RhythmChallenge() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);

	virtual HRESULT Late_Initialize() override;

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	virtual void Render_Properties() override;

	virtual void	Set_Animator(wstring sAnimatorPath) override;
	virtual void	Set_RhythmChallenge(class CRhythmChallenge* pRhythmChallenge);
public:
	EInteractResult	GetRhythmChallengeState();
	void Challenge();
	void Success();
	void Fail();

	void Move(class CCharacter* pPlayer, class CCharacter* pPartner);
public:
	virtual void	Free() override;

	HRESULT Add_InitComponents() override;
private:
	class CRhythmChallenge*		m_pRhythmChallenge	= nullptr;
	EInteractResult				m_eInteractResult	= EInteractResult::IR_END;

	weak_ptr<CTransform> m_wpPartnerTransform;
	weak_ptr<CTransform> m_wpPlayerTransform;
};
NS_END