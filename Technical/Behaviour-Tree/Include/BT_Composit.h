#pragma once
#include "BT_Node.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBT_Composit abstract : public CBT_Node
{
	CBT_Composit(const CBT_Composit& Prototype)				= delete;
	CBT_Composit& operator=(const CBT_Composit& Prototype)	= delete;
protected:
	CBT_Composit();
	virtual ~CBT_Composit() = default;
public:
	virtual HRESULT	AddChildNode(CBT_Node* pChildNode);
protected:
	vector<unique_ptr<CBT_Node>>	m_vecChildNodes;
	_int       m_iCurrentChildIdx = 0;
};

NS_END
