#include "BT_Node.h"

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
	m_eState = eState;
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
