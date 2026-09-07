#pragma once
#include "BT_Composit.h"

NS_BEGIN(Engine)

class ENGINE_DLL  CBT_Selector final : public CBT_Composit
{
	CBT_Selector(const CBT_Selector& Prototype)				= delete;
	CBT_Selector& operator=(const CBT_Selector& Prototype)	= delete;
public:
	struct BT_Selector_Desc : public BT_NODE_DESC {
	};
protected:
	CBT_Selector();
	virtual ~CBT_Selector() = default;
protected:
	virtual HRESULT Initialize(void* pArg)		override;
public:
	virtual ESTATE	Update(_float fTimeDelta)	override;
public:
	static CBT_Selector* Create(void* pArg);
private:
	BT_Selector_Desc			m_desc;
	void OnAbort(int iLowerPriority) override; 
};

NS_END
