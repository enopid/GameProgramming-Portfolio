#pragma once
#include "Base.h"


NS_BEGIN(Engine)

class ENGINE_DLL CBT_Node abstract : public CBase
{
	CBT_Node(const CBT_Node& Prototype)				= delete;
	CBT_Node& operator=(const CBT_Node& Prototype)	= delete;
public:
	enum ENODETYPE {
		NT_TASK,
		NT_COMPOSIT,
		NT_DECORATOR,
		NT_SUBTREE,
		NT_END
	};
	enum class ESTATE {
		ES_SUCCESS,
		ES_FAIL,
		ES_RUNNING,
		ES_NONE
	};

	struct BT_NODE_DESC
	{
		wstring						m_sNodeTag	= L"";
		ENODETYPE					m_eNodeType = NT_END;
		list<function<void(CBT_Node*)>>			m_lstFuncExits;
		list<function<void(CBT_Node*)>>			m_lstFuncEnters;
		list<function<void(CBT_Node*, float)>>	m_lstFuncStay;
	};
	virtual ~CBT_Node()				= default;
protected:
	CBT_Node() {};
protected:
	HRESULT SetChild(CBT_Node* pChildNode, int iPriority = 0);
	virtual HRESULT Initialize(void* pArg)			PURE;
public:
	virtual ESTATE	Tick(_float fTimeDelta);
	virtual void	OnAbort(int iLowerPriority)		PURE;
	bool IsRunning() const;
	void	AddEnterFunc(function<void(CBT_Node*)> func);
	void	AddExitFunc(function<void(CBT_Node*)> func);
	void	AddStayFunc(function<void(CBT_Node*, float)> func);
	ENODETYPE	GetNodeType();
protected:
	void Enter();
	virtual ESTATE	Update(_float fTimeDelta)		PURE;
	void Exit();
protected:
	BT_NODE_DESC				m_nodeDesc;
	CBT_Node*					m_pParentNode	= nullptr;
	ESTATE						m_eState		= ESTATE::ES_NONE;
	int							m_iPriority		= -1;
};
NS_END

