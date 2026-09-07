#pragma once


#include "FSM.h"
#include "Client_Defines.h" 


NS_BEGIN(Client)

class CPartnerBattle_FSM final : public CFSM
{
public:
	struct FPartnerBattleStateDesc {
		_bool m_bIsOnAvailable		= false;
		_bool m_bPartnerKeyPressed	= false;

		_bool m_bIsOnSpecial		= false;
		_bool m_bIsOnGroundJam		= false;
		_bool m_bIsOnAirJam			= false;
		_bool m_bIsOnParry			= false;
		_bool m_bIsOnEvent			= false;

		_bool m_bTaskEnd			= false;
	};

public:
	virtual ~CPartnerBattle_FSM() = default;

	FPartnerBattleStateDesc* Get_Context() { return &m_desc; }
private:
	CPartnerBattle_FSM(_uint iState);
	CPartnerBattle_FSM(const CPartnerBattle_FSM& Prototype) = delete;
	CPartnerBattle_FSM& operator=(const CPartnerBattle_FSM& Prototype) = delete;
private:
	HRESULT Initialize(void* pArg) override;
public:
	static unique_ptr<CPartnerBattle_FSM> Create(_uint iState);
private:
	FPartnerBattleStateDesc m_desc = {};
};

NS_END

