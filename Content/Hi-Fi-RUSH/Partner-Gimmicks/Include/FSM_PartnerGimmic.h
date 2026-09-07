#pragma once


#include "FSM.h"
#include "Client_Defines.h" 


NS_BEGIN(Client)

class CPartnerGimmic_FSM final : public CFSM
{
public:
	struct FPartnerGimmicStateDesc {
		_bool m_bIsOnAvailable		= false; // 현재 가능 상태인지
		_bool m_bPartnerKeyPressed	= false; // 발사나 소환, 귀환 타이밍 결정
		_bool m_bIsFar				= false; // 발사나 소환, 귀환 타이밍 결정

		_bool m_bIsOnTarget			= false; // 타겟온인지

		_bool m_bIsFail				= false; //테스크 실패 체크
		_bool m_bIsSuccess			= false; //테스크 성공 체크


		_bool m_bEnterEnd			= false; //애니메이션 종료 타이밍1
		_bool m_bExitEnd			= false; //애니메이션 종료 타이밍2
		_bool m_bTaskEnd			= false; //성공이나 실패 테스크 종료
	};

public:
	virtual ~CPartnerGimmic_FSM() = default;

	FPartnerGimmicStateDesc* Get_Context() { return &m_desc; }
private:
	CPartnerGimmic_FSM(_uint iState);
	CPartnerGimmic_FSM(const CPartnerGimmic_FSM& Prototype) = delete;
	CPartnerGimmic_FSM& operator=(const CPartnerGimmic_FSM& Prototype) = delete;
private:
	HRESULT Initialize(void* pArg) override;
public:
	static unique_ptr<CPartnerGimmic_FSM> Create(_uint iState);
private:
	FPartnerGimmicStateDesc m_desc = {};
};

NS_END

