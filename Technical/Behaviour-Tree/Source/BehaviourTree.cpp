#include "BehaviourTree.h"
#include "BT_Node.h"
#include "BlackBoard.h"

CBehaviourTree::CBehaviourTree()
{
}

HRESULT CBehaviourTree::Initialize(void* pArg)
{
	m_pBlackBoard = unique_ptr<CBlackBoard>(CBlackBoard::Create());
	return S_OK;
}

CBT_Node::ESTATE CBehaviourTree::Tick(_float fTimeDelta)
{
	return m_pRootNode->Tick(fTimeDelta);
}

CBlackBoard* CBehaviourTree::GetBlackBoard()
{
	return m_pBlackBoard.get();
}

CBehaviourTree* CBehaviourTree::Create(void* pArg)
{
    CBehaviourTree* pTree = new CBehaviourTree();
    if (FAILED(pTree->Initialize(pArg))) {
        MSG_BOX("Fail to Create Task");
        return nullptr;
    }
    return pTree;
}

HRESULT CBehaviourTree::AddRootNode(CBT_Node* pRootNode)
{
    m_pRootNode = unique_ptr<CBT_Node>(pRootNode);
    return S_OK;
}
