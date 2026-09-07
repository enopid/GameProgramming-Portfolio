#pragma once
#include "BT_Composit.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBT_RandomSelector final : public CBT_Composit
{
	CBT_RandomSelector(const CBT_RandomSelector& Prototype)				= delete;
	CBT_RandomSelector& operator=(const CBT_RandomSelector& Prototype)	= delete;
public:
	struct BT_Selector_Desc : public BT_NODE_DESC {
	};
protected:
	CBT_RandomSelector();
	virtual ~CBT_RandomSelector() = default;
protected:
	virtual HRESULT Initialize(void* pArg)		override;
public:
	virtual ESTATE	Update(_float fTimeDelta)	override;
public:
	static CBT_RandomSelector* Create(void* pArg);
	virtual HRESULT	AddChildNode(CBT_Node* pChildNode) override;
	HRESULT	AddChildNode(CBT_Node* pChildNode, _float fWeight);
	_uint	GetNextChildIdx();
private:
	BT_Selector_Desc	m_desc;
	void OnAbort(int iLowerPriority) override;
	vector<_float>		m_vecWeights;

};

NS_END
