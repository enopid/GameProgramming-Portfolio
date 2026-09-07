#include "BT_Task_Wait.h"
#include "GameInstance.h"

CBT_Task_Wait::CBT_Task_Wait()
{
}

HRESULT CBT_Task_Wait::Initialize(void* pArg)
{
    CHKFAIL(__super::Initialize(pArg))
    auto _fDuration = static_cast<BT_Task_Wait_Desc*>(pArg)->m_fDuration;
    m_bRestartTimer = static_cast<BT_Task_Wait_Desc*>(pArg)->m_bRestartTimer;
    m_bForceEndCondition = static_cast<BT_Task_Wait_Desc*>(pArg)->m_bForceEndCondition;
    m_upTimer = CGameInstance::GetInstance()->CreateTimerWithDuration(_fDuration, false, nullptr, false);

    if (m_bRestartTimer) {
        m_nodeDesc.m_lstFuncEnters.push_back([=](CBT_Node*) { m_upTimer->Restart(); });
        m_nodeDesc.m_lstFuncExits.push_back ([=](CBT_Node*) { m_upTimer->Reset(); });
    }
    else {
        m_nodeDesc.m_lstFuncEnters.push_back([=](CBT_Node*) { m_upTimer->SetActive(true); });
        m_nodeDesc.m_lstFuncExits.push_back ([=](CBT_Node*) { m_upTimer->SetActive(false); });
    }
    return S_OK;
}

CBT_Node::ESTATE CBT_Task_Wait::Update(_float fTimeDelta)
{
    if (CheckSuccess()) return ESTATE::ES_SUCCESS;
    if (CheckFail())    return ESTATE::ES_FAIL;
    Running(fTimeDelta);
    return ESTATE::ES_RUNNING;
}

CBT_Task_Wait* CBT_Task_Wait::Create(void* pArg)
{
    CBT_Task_Wait* pTask = new CBT_Task_Wait();
    if (FAILED(pTask->Initialize(pArg))) {
        MSG_BOX("Fail to Create Task");
        return nullptr;
    }
    return pTask;
}

bool CBT_Task_Wait::CheckSuccess()
{
    //시간이 다되거나 조건만족
    //시간이 다되고   조건만족
    // 
    bool bIsSuccess = false;
    if (m_upTimer->IsActive()) {
        if (m_bForceEndCondition && m_desc.m_pFuncConditionSuccess) bIsSuccess = m_desc.m_pFuncConditionSuccess(this);
    }
    else {
        bIsSuccess = true;
        if (m_desc.m_pFuncConditionSuccess) bIsSuccess = m_desc.m_pFuncConditionSuccess(this);
    }

    if (bIsSuccess) Exit();
    return bIsSuccess;
}

bool CBT_Task_Wait::CheckFail()
{
    bool bIsFail = false;
    if (m_desc.m_pFuncConditionFail) bIsFail =  m_desc.m_pFuncConditionFail(this);
    if (bIsFail) Exit();
    return bIsFail;
}

void CBT_Task_Wait::Running(float fDeltaTime)
{
    if (m_desc.m_pFuncRunning)  m_desc.m_pFuncRunning(this, fDeltaTime);
}

void CBT_Task_Wait::OnAbort(int iLowerPriority)
{
    Exit();
}
