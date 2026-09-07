#pragma once
#include "Base.h"
#include "imgui_node_editor.h"
#include "cgraph.h"
#include "gvc.h"

NS_BEGIN(Engine)

namespace ed = ax::NodeEditor;

class ENGINE_DLL CBT_Node abstract : public CBase
{
protected:
	static constexpr float PIN_HEIGHT	= 2.f;
	static constexpr float PIN_PADDINGX	= 8.f;
	static constexpr float BODY_PADDINGX= 4.f;
	static constexpr float PADDINGY		= 1.f;
private:
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
	CBT_Node();
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
public:
	virtual RECT DrawNode(Agraph_t* pGraph) = 0;
	ed::NodeId			GetNodeID();
	ed::PinId			GetInputPinID();
	ed::PinId			GetOutputPinIDs();
	Agnode_t*			GetGVNode() { return m_pGVNode; };
	RECT				GetNodeRect();
	RECT				UnionRect(const RECT& _rectA, const RECT& _rectB, int iPadX = 10, int iPadY = 10);
	void SetGVNodeSize  ();
	virtual void SetGVNodePos	();
protected:
	BT_NODE_DESC				m_nodeDesc;
	CBT_Node*					m_pParentNode	= nullptr;
	ESTATE						m_eState		= ESTATE::ES_NONE;
	int							m_iPriority		= -1;
protected:
	static inline uintptr_t GenerateEditorID()
	{
		static uintptr_t s_NextID = 1;
		return s_NextID++;
	}

	ESTATE				m_eNodeState = ESTATE::ES_NONE;
	ed::NodeId			m_NodeID;
	ed::PinId			m_InputPinID;
	ed::PinId			m_OutputPinID;
	RECT				m_NodeRect;
	Agnode_t*			m_pGVNode;
};
NS_END

