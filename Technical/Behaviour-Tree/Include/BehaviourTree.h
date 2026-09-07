#pragma once

#include "Base.h"
#include "BT_Node.h"


//노드
//		데코레이터
//		컴포짓이나 테스크
//1. 데코레이터
//		실행 조건 식별
//2. 컴포짓
//		액션의 조합
//3. 테스크
//		액션

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
protected:
	unique_ptr<CBlackBoard>		m_pBlackBoard	= nullptr;
	unique_ptr<CBT_Node>		m_pRootNode		= nullptr;
};
NS_END