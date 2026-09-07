#include "BT_Decorator.h"

CBT_Decorator::CBT_Decorator()
{
}

HRESULT CBT_Decorator::Initialize(void* pArg)
{
	CHKFAIL(__super::Initialize(pArg))
	if (pArg) {
		m_desc = *static_cast<BT_Decorator_Desc*>(pArg);
		m_desc.m_eNodeType		= CBT_Node::NT_DECORATOR;
		m_nodeDesc.m_eNodeType	= m_desc.m_eNodeType;
	}
	else {
		return E_FAIL;
	}

	m_nodeDesc.m_lstFuncEnters.push_back([=](CBT_Node*) { m_bLastCondition = true; });
	return S_OK;
}

CBT_Node::ESTATE CBT_Decorator::Update(_float fTimeDelta)
{
	if (!CheckCondition()) {
		return OnConditionFail();
	}

	auto result = m_upChildNode->Tick(fTimeDelta);
	result		= ModifyResult(result);
	m_eState = result;
	return result;
}

bool CBT_Decorator::CheckCondition()
{
	auto m_bNextCondition = true;
	if (m_desc.m_pFuncCheckCondition) 
		m_bNextCondition = m_desc.m_pFuncCheckCondition(this);

	if (m_bNextCondition != m_bLastCondition) {
		switch (m_desc.m_eAbortMode)
		{
		case Engine::CBT_Decorator::EABORTMODE::NONE:
			if (m_upChildNode->IsRunning()) {
				m_bNextCondition = true;
			}
			break;
		case Engine::CBT_Decorator::EABORTMODE::SELF:
			if (!m_bNextCondition)	OnAbort(0);
			break;
		case Engine::CBT_Decorator::EABORTMODE::LOWERPRIORITY:
			if (m_bNextCondition)	m_pParentNode->OnAbort(m_iPriority+1);
			break;
		case Engine::CBT_Decorator::EABORTMODE::BOTH:
			if (!m_bNextCondition)	OnAbort(0);
			if (m_bNextCondition)	m_pParentNode->OnAbort(m_iPriority + 1);
			break;
		default:
			break;
		}
	}
	m_bLastCondition = m_bNextCondition;
	return m_bLastCondition;
}

CBT_Decorator::EABORTMODE CBT_Decorator::GetAbortMode()
{
	return m_desc.m_eAbortMode;
}

CBT_Node::ESTATE CBT_Decorator::OnConditionFail()
{
	if (m_desc.m_pFuncConditionFail) {
		m_eState =  m_desc.m_pFuncConditionFail(this);
		return m_eState;
	}
	return ESTATE::ES_FAIL;
}

CBT_Node::ESTATE CBT_Decorator::ModifyResult(ESTATE eState)
{
	if (m_desc.m_pFuncModifyResult) return m_desc.m_pFuncModifyResult(this, eState);
	return eState;
}

void CBT_Decorator::OnAbort(int iLowerPriority)
{
	if (IsRunning()) {
		Exit();
		m_upChildNode->OnAbort(0);
	}
}

HRESULT CBT_Decorator::AddChildNode(CBT_Node* pChildNode)
{
	
	if (m_upChildNode) {
		wstring sMSG = L"Fail to Add Child : Already has child node";
		MSG_BOXL(sMSG.c_str());
		return E_FAIL;
	}
	if (FAILED(SetChild(pChildNode))) {
		wstring sMSG = L"Fail to Add Child : Child node already has parent";
		MSG_BOXL(sMSG.c_str());
		return E_FAIL;
	}
	m_upChildNode = unique_ptr<CBT_Node>(pChildNode);
	
	return S_OK;
}

CBT_Decorator::EABORTMODE CBT_Decorator::GetAbortType()
{
	return m_desc.m_eAbortMode;
}

CBT_Decorator* CBT_Decorator::Create(void* pArg)
{
	CBT_Decorator* pDecorator = new CBT_Decorator();
	if (FAILED(pDecorator->Initialize(pArg))) {
		MSG_BOX("Fail to Create Decorator");
		return nullptr;
	}
	return pDecorator;
}
