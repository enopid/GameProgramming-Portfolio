#include "BT_RandomSelector.h"

CBT_RandomSelector::CBT_RandomSelector()
{
}

HRESULT CBT_RandomSelector::Initialize(void* pArg)
{
    CHKFAIL(__super::Initialize(pArg))
    return S_OK;
}

CBT_Node::ESTATE CBT_RandomSelector::Update(_float fTimeDelta)
{
    auto eState = m_vecChildNodes[m_iCurrentChildIdx]->Tick(fTimeDelta);
    m_eState = eState;
    if (eState == ESTATE::ES_RUNNING) {
        for (size_t i = 0; i < m_vecChildNodes.size(); i++) if(m_iCurrentChildIdx!=i) m_vecChildNodes[i]->OnAbort(0);
        return eState;
    }
    else if (eState == ESTATE::ES_SUCCESS) {
        m_iCurrentChildIdx = GetNextChildIdx();
        Exit();
        return eState;
    }
    else if (eState == ESTATE::ES_FAIL) {
        m_iCurrentChildIdx = GetNextChildIdx();
        Exit();
        return eState;
    }
    return ESTATE::ES_FAIL;
}

CBT_RandomSelector* CBT_RandomSelector::Create(void* pArg)
{
    CBT_RandomSelector* pSequence = new CBT_RandomSelector();
    if (FAILED(pSequence->Initialize(pArg))) {
        MSG_BOX("Fail to Create Sequence");
        return nullptr;
    }
    return pSequence;
}

HRESULT CBT_RandomSelector::AddChildNode(CBT_Node* pChildNode)
{
    return AddChildNode(pChildNode, 1.f);
}

HRESULT CBT_RandomSelector::AddChildNode(CBT_Node* pChildNode, _float fWeight)
{
    m_vecWeights.push_back(fWeight);
    return __super::AddChildNode(pChildNode);;
}

_uint CBT_RandomSelector::GetNextChildIdx()
{
    vector<_float> vecAccSum;
    vecAccSum.push_back(0.f);
    for (auto fWeight : m_vecWeights)
        vecAccSum.push_back(vecAccSum.back() + fWeight);
    _float fSum = vecAccSum.back();

    _float fNum = (rand() % 10000)* fSum / 10000.f;

    for (_uint i = 1; i < vecAccSum.size(); i++)
    {
        if (vecAccSum[i] >= fNum) {
            return i-1;
            break;
        }
    }
}

void CBT_RandomSelector::OnAbort(int iLowerPriority)
{
    m_iCurrentChildIdx = 0;
    for (size_t i = iLowerPriority; i < m_vecChildNodes.size(); i++)
    {
        m_vecChildNodes[i]->OnAbort(0);
    }
    Exit();
}
