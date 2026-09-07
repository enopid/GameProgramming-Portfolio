#pragma once
#include "BT_Composit.h"

NS_BEGIN(Engine)

class ENGINE_DLL  CBT_Sequence final : public CBT_Composit
{
	CBT_Sequence(const CBT_Sequence& Prototype)				= delete;
	CBT_Sequence& operator=(const CBT_Sequence& Prototype)	= delete;
public:
	struct BT_Sequence_Desc : public BT_NODE_DESC {
	};
protected:
	CBT_Sequence();
	virtual ~CBT_Sequence() = default;
protected:
	virtual HRESULT Initialize(void* pArg)		override;
public:
	virtual ESTATE	Update(_float fTimeDelta)	override;
public:
	static CBT_Sequence* Create(void* pArg);
private:
	BT_Sequence_Desc			m_desc;
	void OnAbort(int iLowerPriority) override; 
};

NS_END
