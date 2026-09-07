#include "BT_Decorator_Cooldown.h"

CBT_Decorator_Cooldown::CBT_Decorator_Cooldown() : CBT_Decorator()
{
}

HRESULT CBT_Decorator_Cooldown::Initialize(void* pArg)
{
	CHKFAIL(__super::Initialize(pArg))
	return S_OK;
}

CBT_Node::ESTATE CBT_Decorator_Cooldown::Update(_float fTimeDelta)
{
	return __super::Update(fTimeDelta);
}

bool CBT_Decorator_Cooldown::CheckCondition()
{
	return __super::CheckCondition();
}

CBT_Node::ESTATE CBT_Decorator_Cooldown::OnConditionFail()
{
	return __super::OnConditionFail();
}

CBT_Node::ESTATE CBT_Decorator_Cooldown::ModifyResult(ESTATE eState)
{
	return __super::ModifyResult(eState);
}

CBT_Decorator_Cooldown* CBT_Decorator_Cooldown::Create(void* pArg)
{
	CBT_Decorator_Cooldown* pDecorator = new CBT_Decorator_Cooldown();
	if (FAILED(pDecorator->Initialize(pArg))) {
		MSG_BOX("Fail to Create Decorator Cooldown");
		return nullptr;
	}
	return pDecorator;
}
