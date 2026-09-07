#include "GameInstance.h"
#include "FSM_PartnerGimmic.h"
#include "Partner.h"

CPartnerGimmic_FSM::CPartnerGimmic_FSM(_uint iState)
	: CFSM(iState)
{
}

HRESULT CPartnerGimmic_FSM::Initialize(void* pArg)
{
	CHKFAIL(__super::Initialize(pArg))

	using PG = EPartnerGimmicState;
	AddTransitionFunc(ENUM_TO_UINT(PG::NONE), ENUM_TO_UINT(PG::ENTER), 
		[=]() { return m_desc.m_bIsOnAvailable && m_desc.m_bPartnerKeyPressed; }, 0.f);
	
	AddTransitionFunc(ENUM_TO_UINT(PG::ENTER), ENUM_TO_UINT(PG::IDLE),
		[=]() { return m_desc.m_bEnterEnd; }, 0.f);
	
	AddTransitionFunc(ENUM_TO_UINT(PG::IDLE), ENUM_TO_UINT(PG::LOCKON),
		[=]() { return m_desc.m_bIsOnTarget; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PG::IDLE), ENUM_TO_UINT(PG::EXIT),
		[=]() { return !m_desc.m_bPartnerKeyPressed || m_desc.m_bIsFar; }, 0.f);

	AddTransitionFunc(ENUM_TO_UINT(PG::LOCKON), ENUM_TO_UINT(PG::IDLE),
		[=]() { return !m_desc.m_bIsOnTarget; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PG::LOCKON), ENUM_TO_UINT(PG::SHOT),
		[=]() { return !m_desc.m_bPartnerKeyPressed && m_desc.m_bIsOnTarget; }, 0.f);


	AddTransitionFunc(ENUM_TO_UINT(PG::SHOT), ENUM_TO_UINT(PG::SUCCESS),
		[=]() { return m_desc.m_bIsSuccess; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PG::SHOT), ENUM_TO_UINT(PG::FAIL),
		[=]() { return m_desc.m_bIsFail; }, 0.f);


	AddTransitionFunc(ENUM_TO_UINT(PG::SUCCESS), ENUM_TO_UINT(PG::EXIT),
		[=]() { return m_desc.m_bTaskEnd; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PG::FAIL), ENUM_TO_UINT(PG::EXIT),
		[=]() { return m_desc.m_bTaskEnd; }, 0.f);

	AddTransitionFunc(ENUM_TO_UINT(PG::EXIT), ENUM_TO_UINT(PG::NONE),
		[=]() { return m_desc.m_bExitEnd; }, 0.f);

	return S_OK;
}

unique_ptr<CPartnerGimmic_FSM> CPartnerGimmic_FSM::Create(_uint iState)
{
	unique_ptr<CPartnerGimmic_FSM> upFSM = unique_ptr<CPartnerGimmic_FSM>(new CPartnerGimmic_FSM(iState));
	if (FAILED(upFSM->Initialize(nullptr))) {
		MSG_BOX("Fail to Create CPartnerGimmic_FSM");
		return nullptr;
	}
	return upFSM;
}
