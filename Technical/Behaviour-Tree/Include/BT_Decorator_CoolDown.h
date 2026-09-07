#pragma once
#include "BT_Decorator.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CBlackBoard;

class ENGINE_DLL CBT_Decorator_Cooldown : public CBT_Decorator
{
	CBT_Decorator_Cooldown(const CBT_Decorator_Cooldown& Prototype) = delete;
	CBT_Decorator_Cooldown& operator=(const CBT_Decorator_Cooldown& Prototype) = delete;
public:
	struct BT_Decorator_Cooldown_Desc : public BT_Decorator_Desc {
		_float	m_fTime = 1.f;
	};
protected:
	CBT_Decorator_Cooldown();
	virtual ~CBT_Decorator_Cooldown() = default;
protected:
	virtual HRESULT Initialize(void* pArg)		override;
public:
	virtual ESTATE	Update(_float fTimeDelta)	override;
public:
	static CBT_Decorator_Cooldown* Create(void* pArg);
	virtual bool	CheckCondition();
private:
	virtual ESTATE	OnConditionFail();
	virtual ESTATE	ModifyResult(ESTATE eState);
protected:
	unique_ptr<CBT_Node>	m_upChildNode = nullptr;
private:
	BT_Decorator_Desc			m_desc;

	bool       m_bLastCondition = true;

private:
	UniqueTimer		m_upTimer = nullptr;
};
NS_END
