#pragma once

#include "Base.h"
#include "BT_Node.h"

NS_BEGIN(Engine)
class CBlackBoard;

class ENGINE_DLL CBehaviourTree : public CBase
{
protected:
	CBehaviourTree();
	CBehaviourTree(const CBehaviourTree& Prototype)				= delete;
	CBehaviourTree& operator=(const CBehaviourTree& Prototype)	= delete;
	virtual ~CBehaviourTree()									= default;
protected:
	virtual HRESULT Initialize(void* pArg);
public:
	virtual CBT_Node::ESTATE	Tick(_float fTimeDelta);
	CBlackBoard*				GetBlackBoard();
	static CBehaviourTree*		Create(void* pArg);
	HRESULT						AddRootNode(CBT_Node* pRootNode);
public:
	void RenderBT();
protected:
	unique_ptr<CBlackBoard>		m_pBlackBoard	= nullptr;
	unique_ptr<CBT_Node>		m_pRootNode		= nullptr;
};
NS_END