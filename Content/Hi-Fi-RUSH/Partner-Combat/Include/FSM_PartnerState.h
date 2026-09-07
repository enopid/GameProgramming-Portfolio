#pragma once


#include "FSM.h"
#include "Client_Defines.h" 


NS_BEGIN(Client)

class CPartnerState_FSM final : public CFSM
{
public:
	struct FPartnerStateDesc {
		_bool m_bIsOnAvailable		= false;
		_bool m_bIsOnCombat			= false;
		_bool m_bIsOnBattle			= false;
		_bool m_bIsOnNormal			= false;
		_bool m_bIsDone				= false;
		_bool m_bSelected			= false;

		_bool m_bPartnerKeyPressed	= false;
		_bool m_bIsOnCoolDown		= true;
	};

public:
	virtual ~CPartnerState_FSM() = default;

	FPartnerStateDesc* Get_Context() { return &m_desc; }
private:
	CPartnerState_FSM(_uint iState);
	CPartnerState_FSM(const CPartnerState_FSM& Prototype) = delete;
	CPartnerState_FSM& operator=(const CPartnerState_FSM& Prototype) = delete;
private:
	HRESULT Initialize(void* pArg) override;
public:
	static unique_ptr<CPartnerState_FSM> Create(_uint iState);
private:
	FPartnerStateDesc m_desc = {};
};

NS_END

