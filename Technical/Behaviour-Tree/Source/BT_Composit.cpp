#include "BT_Composit.h"

CBT_Composit::CBT_Composit()
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

	return S_OK;
}
