#include "BT_Task.h"

CBT_Task::CBT_Task()
{
}

HRESULT CBT_Task::Initialize(void* pArg)
{
    CHKFAIL(__super::Initialize(pArg))
    if (pArg) {
        m_desc = *static_cast<BT_Task_Desc*>(pArg);
        m_desc.m_eNodeType      = CBT_Node::NT_TASK;
        m_nodeDesc.m_eNodeType  = m_desc.m_eNodeType;
    }
    else {
        return E_FAIL;
    }


    return S_OK;
}

CBT_Node::ESTATE CBT_Task::Update(_float fTimeDelta)
{
    if (CheckSuccess()) return ESTATE::ES_SUCCESS;
    if (CheckFail())    return ESTATE::ES_FAIL;
    Running(fTimeDelta);
    return ESTATE::ES_RUNNING;
}

CBT_Task* CBT_Task::Create(void* pArg)
{
    CBT_Task* pTask = new CBT_Task();
    if (FAILED(pTask->Initialize(pArg))) {
        MSG_BOX("Fail to Create Task");
        return nullptr;
    }
    return pTask;
}

bool CBT_Task::CheckSuccess()
{
    bool bIsSuccess = true;
    if (m_desc.m_pFuncConditionSuccess) bIsSuccess =  m_desc.m_pFuncConditionSuccess(this);
    if (bIsSuccess) Exit();
    return bIsSuccess;
}

bool CBT_Task::CheckFail()
{
    bool bIsFail = false;
    if (m_desc.m_pFuncConditionFail) bIsFail =  m_desc.m_pFuncConditionFail(this);
    if (bIsFail) Exit();
    return bIsFail;
}

void CBT_Task::Running(float fDeltaTime)
{
    if (m_desc.m_pFuncRunning)  m_desc.m_pFuncRunning(this, fDeltaTime);
}

void CBT_Task::OnAbort(int iLowerPriority)
{
    Exit();
}
