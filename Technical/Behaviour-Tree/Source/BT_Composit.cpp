#include "BT_Composit.h"
#include "cgraph.h"
#include "gvc.h"

CBT_Composit::CBT_Composit()
	: CBT_Node()
{
}

HRESULT CBT_Composit::AddChildNode(CBT_Node* pChildNode)
{

	if (FAILED(SetChild(pChildNode, m_vecChildNodes.size()))) {
		wstring sMSG = L"Fail to Add Child : Child node already has parent";
		MSG_BOXL(sMSG.c_str());
		return E_FAIL;
	}

	for (auto& upChildNode : m_vecChildNodes)
	{
		if (upChildNode.get() == pChildNode) {
			wstring sMSG = L"Fail to Add Child : Already has same child node";
			MSG_BOXL(sMSG.c_str());
			return E_FAIL;
		}
	}

	m_vecChildNodes.emplace_back(move(unique_ptr<CBT_Node>(pChildNode)));
	m_vecLinkIds.push_back(GenerateEditorID());

	return S_OK;
}

void CBT_Composit::SetGVNodePos()
{
	__super::SetGVNodePos();

	for (int i = 0; i < m_vecLinkIds.size(); i++) {
		m_vecChildNodes[i]->SetGVNodePos();
	}
}

RECT CBT_Composit::DrawNode(Agraph_t* pGraph)
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

	for (int i = 0; i < m_vecLinkIds.size(); i++) {
		m_vecChildNodes[i]->DrawNode(pGraph);
		ed::Link(
			m_vecLinkIds[i],
			m_OutputPinID,
			m_vecChildNodes[i]->GetInputPinID()
		);
		if (m_vecChildNodes[i]->IsRunning()) {
			ed::Flow(m_vecLinkIds[i]);
			ed::SelectNode(m_vecChildNodes[i]->GetNodeID(), true);
		}
	}

	std::string gvID = std::to_string(m_NodeID.Get());
	m_pGVNode = agnode(pGraph, gvID.data(), 1);
	SetGVNodeSize();
	for (int i = 0; i < m_vecLinkIds.size(); i++) {
		agedge(pGraph, m_pGVNode, m_vecChildNodes[i]->GetGVNode(), nullptr, 1);
	}

	return m_NodeRect;
}
