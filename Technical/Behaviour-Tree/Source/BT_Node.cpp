#include "BT_Node.h"

CBT_Node::CBT_Node()
{
	m_NodeID		= GenerateEditorID();
	m_InputPinID	= GenerateEditorID();
	m_OutputPinID	= GenerateEditorID();
}

HRESULT CBT_Node::SetChild(CBT_Node* pChildNode, int iPriority)
{
	if (pChildNode->m_pParentNode) return E_FAIL;
	pChildNode->m_pParentNode = this;
	pChildNode->m_iPriority = iPriority;
	return S_OK;
}

HRESULT CBT_Node::Initialize(void* pArg)
{
	if (pArg)	m_nodeDesc = *static_cast<BT_NODE_DESC*>(pArg);
	else		return E_FAIL;
	return S_OK;
}

CBT_Node::ESTATE CBT_Node::Tick(_float fTimeDelta)
{
	if (m_eState == ESTATE::ES_NONE)	Enter();
	auto eState = Update(fTimeDelta);
	for (auto& _func : m_nodeDesc.m_lstFuncStay) _func(this, fTimeDelta);
	m_eState = m_eNodeState = eState;
	if (m_eState != ESTATE::ES_RUNNING) Exit();
	return eState;
}

bool CBT_Node::IsRunning() const
{
	return m_eState == ESTATE::ES_RUNNING;
}

void CBT_Node::AddEnterFunc(function<void(CBT_Node*)> func)
{
	m_nodeDesc.m_lstFuncEnters.push_back(func);
}

void CBT_Node::AddExitFunc(function<void(CBT_Node*)> func)
{
	m_nodeDesc.m_lstFuncExits.push_back(func);
}

void CBT_Node::AddStayFunc(function<void(CBT_Node*, float)> func)
{
	m_nodeDesc.m_lstFuncStay.push_back(func);
}

CBT_Node::ENODETYPE CBT_Node::GetNodeType()
{
	return m_nodeDesc.m_eNodeType;
}

void CBT_Node::Enter() {
	for (auto& _func : m_nodeDesc.m_lstFuncEnters) _func(this);
	m_eState = ESTATE::ES_RUNNING;
}

void CBT_Node::Exit() {
	for (auto& _func : m_nodeDesc.m_lstFuncExits) _func(this);
	m_eState = ESTATE::ES_NONE;
}

ed::NodeId CBT_Node::GetNodeID()
{
	return m_NodeID;
}		   

ed::PinId CBT_Node::GetInputPinID()
{
	return m_InputPinID;
}

ed::PinId CBT_Node::GetOutputPinIDs()
{
	return m_OutputPinID;
}

RECT CBT_Node::GetNodeRect()
{
	RECT _rect;
	ImVec2 pos	= ed::GetNodePosition(m_NodeID);
	ImVec2 size = ed::GetNodeSize(m_NodeID);
	_rect.left	= pos.x;
	_rect.top	= pos.y;
	_rect.right	= pos.x + size.x;
	_rect.bottom= pos.y + size.y;
	
	return _rect;
}

RECT CBT_Node::UnionRect(const RECT& _rectA, const RECT& _rectB, int iPadX, int iPadY)
{
	return RECT();
}


void CBT_Node::SetGVNodeSize()
{
	constexpr float DPI = 96.f;

	ImVec2 size = ed::GetNodeSize(m_NodeID);

	const float widthInch	= size.x / DPI;
	const float heightInch	= size.y / DPI;

	agsafeset(
		m_pGVNode,
		const_cast<char*>("width"),
		const_cast<char*>(std::to_string(widthInch).c_str()),
		const_cast<char*>("")
	);

	agsafeset(
		m_pGVNode,
		const_cast<char*>("height"),
		const_cast<char*>(std::to_string(heightInch).c_str()),
		const_cast<char*>("")
	);

	agsafeset(
		m_pGVNode,
		const_cast<char*>("fixedsize"),
		const_cast<char*>("true"),
		const_cast<char*>("")
	);
}

void CBT_Node::SetGVNodePos()
{
	constexpr float POINT_TO_PIXEL = 96.f / 72.f;

	auto coord = ND_coord(m_pGVNode);

	float centerX =
		static_cast<float>(coord.x) * POINT_TO_PIXEL;

	float centerY =
		static_cast<float>(coord.y) * POINT_TO_PIXEL;
	centerY = -centerY;
	// Graphviz는 중심 기준
	// node-editor는 좌상단 기준
	ImVec2 size = ed::GetNodeSize(m_NodeID);

	ed::SetNodePosition(m_NodeID, ImVec2(
		centerX - size.x * 0.5f,
		centerY - size.y * 0.5f
	));
}
