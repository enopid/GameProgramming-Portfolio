#include "GameInstance.h"
#include "FSM_PartnerState.h"
#include "Partner.h"

CPartnerState_FSM::CPartnerState_FSM(_uint iState)
	: CFSM(iState)
{
}

HRESULT CPartnerState_FSM::Initialize(void* pArg)
{
	CHKFAIL(__super::Initialize(pArg))

	using PS = EPartnerState;
	AddTransitionFunc(ENUM_TO_UINT(PS::NONE), ENUM_TO_UINT(PS::NORMAL), [=]() { return m_desc.m_bIsOnNormal; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PS::NORMAL), ENUM_TO_UINT(PS::NONE), [=]() { return !m_desc.m_bIsOnNormal;}, 0.f);
	
	AddTransitionFunc(ENUM_TO_UINT(PS::NONE), ENUM_TO_UINT(PS::GIMMIC), [=]() { 
		return m_desc.m_bIsOnAvailable && m_desc.m_bIsOnCombat && !m_desc.m_bIsOnBattle && m_desc.m_bPartnerKeyPressed && m_desc.m_bSelected; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PS::GIMMIC), ENUM_TO_UINT(PS::NONE), [=]() { return m_desc.m_bIsDone;	  }, 0.f);

	AddTransitionFunc(ENUM_TO_UINT(PS::NONE), ENUM_TO_UINT(PS::BATTLE), [=]() { return 
		m_desc.m_bIsOnAvailable && m_desc.m_bIsOnCombat && m_desc.m_bIsOnBattle && m_desc.m_bPartnerKeyPressed && !m_desc.m_bIsOnCoolDown && m_desc.m_bSelected; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PS::BATTLE), ENUM_TO_UINT(PS::NONE), [=]() { return m_desc.m_bIsDone;	  }, 0.f);
	
	return S_OK;
}

unique_ptr<CPartnerState_FSM> CPartnerState_FSM::Create(_uint iState)
{
	unique_ptr<CPartnerState_FSM> upFSM = unique_ptr<CPartnerState_FSM>(new CPartnerState_FSM(iState));
	if (FAILED(upFSM->Initialize(nullptr))) {
		MSG_BOX("Fail to Create CPartnerState_FSM");
		return nullptr;
	}
	return upFSM;
}
