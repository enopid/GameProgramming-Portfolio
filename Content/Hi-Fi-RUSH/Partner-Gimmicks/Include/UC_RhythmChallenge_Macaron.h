#pragma once
#include "Client_Defines.h"
#include "UIController.h"
#include "Time_Manager_API.h"

NS_BEGIN(Client)

class CUC_RhythmChallenge_Macaron : public CUIController
{
private:
	explicit CUC_RhythmChallenge_Macaron();
	virtual ~CUC_RhythmChallenge_Macaron() = default;

public:
	HRESULT Initialize(void* pDesc, _wstring sLVLTag, _wstring sUCName) override;
	void Update(float fDeltaTime) override;
	void Late_Initialize() override;

public:
	static CUC_RhythmChallenge_Macaron* Create(void* pArg, _wstring sLVLTag, _wstring sUCName);

	void Enter() override;
	void Exit() override;
	void Free() override;
public:
	void SetPos(_float2 _vPos);
	void OnChallenge();
	void Success();
	void Fail();
private:
	class CUI_Image*	m_pRingImg			= { nullptr };
	UniqueTimer			m_upSuccessTimer	= nullptr;
};

NS_END