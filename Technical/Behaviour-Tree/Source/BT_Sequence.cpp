#include "BT_Sequence.h"

CBT_Sequence::CBT_Sequence()
    : CBT_Composit()
{
}

HRESULT CBT_Sequence::Initialize(void* pArg)
{
    CHKFAIL(__super::Initialize(pArg))
    return S_OK;
}

CBT_Node::ESTATE CBT_Sequence::Update(_float fTimeDelta)
{
    while (m_iCurrentChildIdx < m_vecChildNodes.size()) {
        auto eState = m_vecChildNodes[m_iCurrentChildIdx]->Tick(fTimeDelta);
        m_eState = eState;
        if (eState==ESTATE::ES_SUCCESS) {
            m_iCurrentChildIdx++;
        }
        else if (eState == ESTATE::ES_FAIL) {
            m_iCurrentChildIdx = 0;
            return ESTATE::ES_FAIL;
        }
        else if (eState == ESTATE::ES_RUNNING) {
            return ESTATE::ES_RUNNING;
        }
    }
    m_iCurrentChildIdx = 0;
    return ESTATE::ES_SUCCESS;
}

CBT_Sequence* CBT_Sequence::Create(void* pArg)
{
    CBT_Sequence* pSequence = new CBT_Sequence();
    if (FAILED(pSequence->Initialize(pArg))) {
        MSG_BOX("Fail to Create Sequence");
        return nullptr;
    }
    return pSequence;
}

void CBT_Sequence::OnAbort(int iLowerPriority)
{
    m_vecChildNodes[m_iCurrentChildIdx]->OnAbort(0);
    m_iCurrentChildIdx = 0;
    Exit();
}
