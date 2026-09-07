#include "BT_Selector.h"
#include "BT_Decorator.h"

CBT_Selector::CBT_Selector() : CBT_Composit()
{
}

HRESULT CBT_Selector::Initialize(void* pArg)
{
    CHKFAIL(__super::Initialize(pArg))
    m_iCurrentChildIdx = -1;
    return S_OK;
}

CBT_Node::ESTATE CBT_Selector::Update(_float fTimeDelta)
{
    if (m_iCurrentChildIdx!=-1 && m_vecChildNodes[m_iCurrentChildIdx]->GetNodeType() == NT_DECORATOR) {
        auto pDecorator = static_cast<CBT_Decorator*>(m_vecChildNodes[m_iCurrentChildIdx].get());
        if (pDecorator->GetAbortType() == CBT_Decorator::EABORTMODE::NONE) {
            auto eState = m_vecChildNodes[m_iCurrentChildIdx]->Tick(fTimeDelta);
            if (in(eState, { ESTATE::ES_SUCCESS , ESTATE::ES_RUNNING })) return eState;
        }
    }
    for (size_t i = 0; i < m_vecChildNodes.size(); i++) {
        auto eState = m_vecChildNodes[i]->Tick(fTimeDelta);
        m_eState            = eState;
        m_iCurrentChildIdx  = i;
        if (in(eState, { ESTATE::ES_SUCCESS , ESTATE::ES_RUNNING })) {
            for (size_t i = 0; i < m_vecChildNodes.size(); i++) if (m_iCurrentChildIdx != i) 
                m_vecChildNodes[i]->OnAbort(0);
            return eState;
        }
    }
    return ESTATE::ES_FAIL;
}

CBT_Selector* CBT_Selector::Create(void* pArg)
{
    CBT_Selector* pSequence = new CBT_Selector();
    if (FAILED(pSequence->Initialize(pArg))) {
        MSG_BOX("Fail to Create Selector");
        return nullptr;
    }
    return pSequence;
}

void CBT_Selector::OnAbort(int iLowerPriority)
{
    m_iCurrentChildIdx = -1;
    for (size_t i = iLowerPriority; i < m_vecChildNodes.size(); i++)
    {
        m_vecChildNodes[i]->OnAbort(0);
    }
    Exit();
}
