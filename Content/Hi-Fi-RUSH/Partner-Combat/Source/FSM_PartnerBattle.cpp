#include "GameInstance.h"
#include "FSM_PartnerBattle.h"
#include "Partner.h"

CPartnerBattle_FSM::CPartnerBattle_FSM(_uint iState)
	: CFSM(iState)
{
}

HRESULT CPartnerBattle_FSM::Initialize(void* pArg)
{
	CHKFAIL(__super::Initialize(pArg))

	using PB = EPartnerBattleState;

	AddTransitionFunc(ENUM_TO_UINT(PB::NONE), ENUM_TO_UINT(PB::NORMAL),
		[=]() { return m_desc.m_bIsOnAvailable && m_desc.m_bPartnerKeyPressed; }, 1.f);

	AddTransitionFunc(ENUM_TO_UINT(PB::NONE), ENUM_TO_UINT(PB::EVENT), 
		[=]() { return m_desc.m_bIsOnAvailable && m_desc.m_bPartnerKeyPressed && m_desc.m_bIsOnEvent; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PB::NONE), ENUM_TO_UINT(PB::PARRY), 
		[=]() { return m_desc.m_bIsOnAvailable && m_desc.m_bPartnerKeyPressed && m_desc.m_bIsOnParry; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PB::NONE), ENUM_TO_UINT(PB::SPECIAL),
		[=]() { return m_desc.m_bIsOnAvailable && m_desc.m_bPartnerKeyPressed && m_desc.m_bIsOnSpecial; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PB::NONE), ENUM_TO_UINT(PB::AIRJAM),
		[=]() { return m_desc.m_bIsOnAvailable && m_desc.m_bPartnerKeyPressed && m_desc.m_bIsOnAirJam; }, 0.f);
	AddTransitionFunc(ENUM_TO_UINT(PB::NONE), ENUM_TO_UINT(PB::GROUNDJAM),
		[=]() { 
			return 
				m_desc.m_bIsOnAvailable && m_desc.m_bPartnerKeyPressed && m_desc.m_bIsOnGroundJam; }, 0.f);

	AddBatchTransitionFunc(
		{ ENUM_TO_UINT(PB::NORMAL), ENUM_TO_UINT(PB::PARRY), ENUM_TO_UINT(PB::SPECIAL), ENUM_TO_UINT(PB::AIRJAM), ENUM_TO_UINT(PB::GROUNDJAM), ENUM_TO_UINT(PB::EVENT) },
		{ ENUM_TO_UINT(PB::NONE) },
		[=]() { return m_desc.m_bTaskEnd; }, 0.f);

	return S_OK;
}

unique_ptr<CPartnerBattle_FSM> CPartnerBattle_FSM::Create(_uint iState)
{
	unique_ptr<CPartnerBattle_FSM> upFSM = unique_ptr<CPartnerBattle_FSM>(new CPartnerBattle_FSM(iState));
	if (FAILED(upFSM->Initialize(nullptr))) {
		MSG_BOX("Fail to Create CPartnerBattle_FSM");
		return nullptr;
	}
	return upFSM;
}
