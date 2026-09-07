#pragma once
#include "BT_Task.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBT_Task_Wait : public CBT_Task
{
	CBT_Task_Wait(const CBT_Task_Wait& Prototype) = delete;
	CBT_Task_Wait& operator=(const CBT_Task_Wait& Prototype) = delete;
public:

	struct BT_Task_Wait_Desc : public BT_Task_Desc {
		_float  m_fDuration				= 0.f;
		_bool	m_bRestartTimer			= true;
		_bool	m_bForceEndCondition	= false;
	};
protected:
	CBT_Task_Wait();
	virtual ~CBT_Task_Wait() = default;
protected:
	virtual HRESULT Initialize(void* pArg)		override;
public:
	virtual ESTATE	Update(_float fTimeDelta)	override;
public:
	static CBT_Task_Wait* Create(void* pArg);
private:
	virtual bool	CheckSuccess();
	virtual bool	CheckFail();
	virtual void	Running(float fDeltaTime);
private:
	BT_Task_Desc			m_desc;
	UniqueTimer				m_upTimer		= nullptr;
	_bool					m_bRestartTimer = false;
	_bool					m_bForceEndCondition = false;
	void OnAbort(int iLowerPriority) override; //Pritority Param will be Ignored
};

NS_END
