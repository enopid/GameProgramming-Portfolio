#include "BT_Decorator.h"
#include "cgraph.h"
#include "gvc.h"

CBT_Decorator::CBT_Decorator()
	: CBT_Node()
{
	m_LinkId = GenerateEditorID();
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
	if (!CheckCondition()) return OnConditionFail();

	auto _prevResult = m_upChildNode->Tick(fTimeDelta);
	auto result		= ModifyResult(_prevResult);
	m_eState = result;
	if (m_eState != ESTATE::ES_RUNNING && _prevResult== ESTATE::ES_RUNNING)
		m_upChildNode->OnAbort(0);
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
			if (m_upChildNode->IsRunning()) m_bNextCondition = true;
			break;
		case Engine::CBT_Decorator::EABORTMODE::SELF:
			if (!m_bNextCondition)			OnAbort(0);
			break;
		case Engine::CBT_Decorator::EABORTMODE::LOWERPRIORITY:
			if (m_bNextCondition)			m_pParentNode->OnAbort(m_iPriority+1);
			break;
		case Engine::CBT_Decorator::EABORTMODE::BOTH:
			if (!m_bNextCondition)			OnAbort(0);
			if (m_bNextCondition)			m_pParentNode->OnAbort(m_iPriority + 1);
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

void CBT_Decorator::SetGVNodePos()
{
	__super::SetGVNodePos();
	m_upChildNode->SetGVNodePos();
}

RECT CBT_Decorator::DrawNode(Agraph_t* pGraph)
{
	ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.f, 1.f));
	ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.f, -1.f));

	if (m_eNodeState == CBT_Node::ESTATE::ES_RUNNING) {
		ed::PushStyleColor(
			ed::StyleColor_NodeBg,
			ImColor(208, 215, 15)
		);
	}
	else if (m_eNodeState == CBT_Node::ESTATE::ES_FAIL) {
		ed::PushStyleColor(
			ed::StyleColor_NodeBg,
			ImColor(240, 15, 15)
		);
	}
	else if (m_eNodeState == CBT_Node::ESTATE::ES_SUCCESS) {
		ed::PushStyleColor(
			ed::StyleColor_NodeBg,
			ImColor(15, 215, 15)
		);
	}
	else {
		ed::PushStyleColor(
			ed::StyleColor_NodeBg,
			ImColor(73, 72, 73)
		);
	}
	m_eNodeState = m_eState;

	string _nodeName = ws2s(m_nodeDesc.m_sNodeTag);
	ImVec2 _textSize = ImGui::CalcTextSize(_nodeName.c_str());
	float bodyWidth = max(_textSize.x + BODY_PADDINGX * 2.f, 40.f);
	float bodyHeight = 20.f;
	float pinWidth = bodyWidth - PIN_PADDINGX * 2.f;

	ed::BeginNode(m_NodeID);

	//toppin
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + PIN_PADDINGX);
	ed::BeginPin(m_InputPinID, ed::PinKind::Input);
	ImGui::Dummy(ImVec2(pinWidth, PIN_HEIGHT));
	ed::EndPin();

	//body
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + BODY_PADDINGX);
	ImGui::Text("%s", ws2s(m_nodeDesc.m_sNodeTag).c_str());
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(BODY_PADDINGX, 0.f));

	//endpin
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + PIN_PADDINGX);
	ed::BeginPin(m_OutputPinID, ed::PinKind::Output);
	ImGui::Dummy(ImVec2(pinWidth, PIN_HEIGHT));
	ed::EndPin();

	ed::EndNode();
	ed::PopStyleVar(2);
	ed::PopStyleColor(1);

	m_upChildNode->DrawNode(pGraph);
	ed::Link(
		m_LinkId,
		m_OutputPinID,
		m_upChildNode->GetInputPinID()
	);
	if (m_upChildNode->IsRunning()) {
		ed::Flow(m_LinkId);
		ed::SelectNode(m_upChildNode->GetNodeID(), true);
	}
	

	std::string gvID = std::to_string(m_NodeID.Get());
	m_pGVNode = agnode(pGraph, gvID.data(), 1);
	agedge(pGraph, m_pGVNode, m_upChildNode->GetGVNode(), nullptr, 1);
	SetGVNodeSize();

	return m_NodeRect;
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
