#pragma once
#include "BT_Node.h"

NS_BEGIN(Engine)
class CBlackBoard;

class ENGINE_DLL CBT_Decorator : public CBT_Node
{
	CBT_Decorator(const CBT_Decorator& Prototype) = delete;
	CBT_Decorator& operator=(const CBT_Decorator& Prototype) = delete;
public:
	enum class EABORTMODE { 
		NONE,			//조건 미만족시도 하던 행동 지속
		SELF,			//조건 미만족시에 하던 행동 취소
		LOWERPRIORITY,	//조건 만족시 낮은 우선순위 중단
		BOTH			//Self + LOWERPRIORITY
	};

	struct BT_Decorator_Desc : public BT_NODE_DESC {
		const	CBlackBoard*						m_pBlackBoard			= nullptr;
		EABORTMODE									m_eAbortMode			= EABORTMODE::NONE;
		function<bool	(CBT_Decorator*)>			m_pFuncCheckCondition	= nullptr;
		function<ESTATE	(CBT_Decorator*)>			m_pFuncConditionFail	= nullptr;
		function<ESTATE	(CBT_Decorator*, ESTATE)>	m_pFuncModifyResult		= nullptr;
	};
protected:
	CBT_Decorator();
	virtual ~CBT_Decorator() = default;
protected:
	virtual HRESULT Initialize(void* pArg)		override;
public:
	virtual ESTATE	Update(_float fTimeDelta)	override;
	HRESULT			AddChildNode(CBT_Node * pChildNode);
	EABORTMODE		GetAbortType();
public:
	static CBT_Decorator* Create(void* pArg);
	virtual bool	CheckCondition();
protected:
	EABORTMODE		GetAbortMode();
	virtual ESTATE	OnConditionFail();
	virtual ESTATE	ModifyResult(ESTATE eState);
protected:
	unique_ptr<CBT_Node>	m_upChildNode = nullptr;
protected:
	BT_Decorator_Desc			m_desc;

	bool       m_bLastCondition = true;

	// CBT_Node을(를) 통해 상속됨
	void OnAbort(int iLowerPriority) override;
};
NS_END
