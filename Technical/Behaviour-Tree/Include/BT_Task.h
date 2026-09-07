#pragma once
#include "BT_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBT_Task : public CBT_Node
{
	CBT_Task(const CBT_Task& Prototype) = delete;
	CBT_Task& operator=(const CBT_Task& Prototype) = delete;
public:

	struct BT_Task_Desc : public BT_NODE_DESC {
		function<bool(CBT_Task*)>			m_pFuncConditionSuccess = nullptr;
		function<bool(CBT_Task*)>			m_pFuncConditionFail	= nullptr;
		function<void(CBT_Task*, float)>	m_pFuncRunning			= nullptr;
	};
protected:
	CBT_Task();
	virtual ~CBT_Task() = default;
protected:
	virtual HRESULT Initialize(void* pArg)		override;
public:
	virtual ESTATE	Update(_float fTimeDelta)	override;
public:
	static CBT_Task* Create(void* pArg);
private:
	virtual bool	CheckSuccess();
	virtual bool	CheckFail();
	virtual void	Running(float fDeltaTime);
protected:
	BT_Task_Desc			m_desc;
	void OnAbort(int iLowerPriority) override;
	// CBT_Node을(를) 통해 상속됨
	RECT DrawNode(Agraph_t* pGraph) override;

	//Pritority Param will be Ignored
};

NS_END
