#include "AnimController.h"
#include "BaseModel.h"
#include "LayerAnimNode.h"
#include "LayerActionAnimNode.h"

CAnimController::CAnimController(shared_ptr<CModel> spModel)
{
    m_wpModel = spModel;
}

CAnimController::CAnimController(const CAnimController& prototype, shared_ptr<CModel> spModel)
    :   m_bIsActive                 ( false ),
        m_tableTransitions          ( prototype.m_tableTransitions ),
        m_vecAnyStateTransitions    ( prototype.m_vecAnyStateTransitions ),
        m_iStartNodeIdx             ( prototype.m_iStartNodeIdx ),
        m_iCurrentNodeIdx           ( prototype.m_iCurrentNodeIdx)
{
    m_wpModel = spModel;
    m_vecAnimNodes.clear();
    m_vecTransitions.clear();
    for (auto& upAnimNode : prototype.m_vecAnimNodes) {
        unique_ptr<CAnimNode> _upAnimNode = upAnimNode->Clone(spModel);
        m_vecAnimNodes.emplace_back(move(_upAnimNode));
    }
    for (auto& upAnimTransition : prototype.m_vecTransitions) {
        unique_ptr<CAnimTransition> _upAnimTransition = upAnimTransition->Clone(spModel);
        m_vecTransitions.emplace_back(move(_upAnimTransition));
    }
}

HRESULT CAnimController::PlayAnimNode(int NodeIdx, float fBlendTime)
{
    m_transitDesc = ANIMTRANSITION_DESC{};
    m_transitDesc.m_iDstAnimNodeIdx = NodeIdx;
    m_transitDesc.m_fBlendTime      = fBlendTime;
    m_transitDesc.m_bHasExitTIme    = false;

    m_vecAnimNodes[m_iCurrentNodeIdx]->Exit(m_transitDesc);
    m_iCurrentNodeIdx = m_transitDesc.m_iDstAnimNodeIdx;
    m_vecAnimNodes[m_iCurrentNodeIdx]->Enter(m_transitDesc);
    m_transitDesc = ANIMTRANSITION_DESC{};

    return S_OK;
}

_bool CAnimController::CheckAnimNodeEnd(int NodeIdx)
{
    if (m_iCurrentNodeIdx != NodeIdx) return false;
    return m_vecAnimNodes[NodeIdx]->IsEnd();
}

void CAnimController::ResetAnimNode(int NodeIdx)
{
    PrintDebug(m_iCurrentNodeIdx, " ", NodeIdx);
    if (m_iCurrentNodeIdx != NodeIdx) return;
    return m_vecAnimNodes[NodeIdx]->Reset();
}   

_int CAnimController::GetCurrentAnimNode()
{
    return m_iCurrentNodeIdx;
}

HRESULT CAnimController::AddAnimNode(unique_ptr<CAnimNode> upAnimNode, bool bIsStartNode)
{
    if (bIsStartNode) {
        if (m_iStartNodeIdx >= 0) {
            MSG_BOX("Fail to add AnimNode :Already has a start node");
            return E_FAIL;
        }
        m_iStartNodeIdx = (_int)m_vecAnimNodes.size();
    }
    if (FindAnimNode(upAnimNode->GetID()) >= 0) {
        MSG_BOX("Fail to add AnimNode :Already has a same id");
        return E_FAIL;
    }
    if (auto pLayerNode = dynamic_cast<CLayerAnimNode*>(upAnimNode.get())) {
        string sLayerTag = pLayerNode->GetLayerTag();
        m_mapLayerAnimNodes[sLayerTag].push_back(pLayerNode);
    }
    else if (auto pLayerActionNode = dynamic_cast<CLayerActionAnimNode*>(upAnimNode.get())) {
        string sLayerActionTag = pLayerActionNode->GetLayerTag();
        m_mapLayerActionAnimNodes[sLayerActionTag].push_back(pLayerActionNode);
    }
    m_vecAnimNodes.emplace_back(move(upAnimNode));
    return S_OK;
}

HRESULT CAnimController::AddAnimTransitition(Json::Value _animTransition)
{
    for (auto& sSrcID : _animTransition["Desc"]["SrcNodeID"]) for (auto& sDstID : _animTransition["Desc"]["DstNodeID"]) {
        auto iSrcIdx = FindAnimNode(sSrcID.asString());
        auto iDstIdx = FindAnimNode(sDstID.asString());
        if (iSrcIdx < 0 || iDstIdx < 0) {
            MSG_BOX("Fail to Create Transsition : Can't Find Src(Dst) ID");
            continue;
        }
        auto upAnimTransition = CAnimTransition::Create(_animTransition, m_wpModel.lock());
        upAnimTransition->SetTransitIdx(iSrcIdx, iDstIdx);
        m_tableTransitions[iSrcIdx].push_back((_int)m_vecTransitions.size());
        m_vecTransitions.emplace_back(move(upAnimTransition));
        sort(m_tableTransitions[iSrcIdx].begin(), m_tableTransitions[iSrcIdx].end(), [&](int idx0, int idx1) -> bool {
            return m_vecTransitions[idx0]->GetDesc().m_iPriority < m_vecTransitions[idx1]->GetDesc().m_iPriority;
            });
    }

    if (_animTransition["Desc"]["SrcNodeID"].size() == 0) {
        for (auto& sDstID : _animTransition["Desc"]["DstNodeID"]) {
            auto iDstIdx = FindAnimNode(sDstID.asString());
            if (iDstIdx < 0) {
                MSG_BOX("Fail to Create Transsition : Can't Find Dst ID");
                continue;
            }
            auto upAnimTransition = CAnimTransition::Create(_animTransition, m_wpModel.lock());
            upAnimTransition->SetTransitIdx(0, iDstIdx);
            m_vecAnyStateTransitions.push_back((_int)m_vecTransitions.size());
            m_vecTransitions.emplace_back(move(upAnimTransition));

            sort(m_vecAnyStateTransitions.begin(), m_vecAnyStateTransitions.end(), [&](int idx0, int idx1) -> bool {
                return m_vecTransitions[idx0]->GetDesc().m_iPriority < m_vecTransitions[idx1]->GetDesc().m_iPriority;
            });
        }
    }
    return S_OK;
}

void CAnimController::SetRootMotionScale(string sTag, float fXScale, float fYScale)
{
    int iAnimNodeIdx = FindAnimNode(sTag);
    if (iAnimNodeIdx == -1) return;

    m_vecAnimNodes[iAnimNodeIdx]->SetRootMotionScale(fXScale, fYScale);
}

HRESULT CAnimController::Initialize(Json::Value jsonValue)
{
    for (auto _animNode : jsonValue["AnimNodes"]) {
        AddAnimNode(move(CAnimNode::Create(_animNode, m_wpModel.lock())));
    }
    m_tableTransitions.resize(m_vecAnimNodes.size(), vector<int>());

    m_iStartNodeIdx = FindAnimNode(jsonValue["StartNode"].asString());

    for (auto& _animTransition : jsonValue["AnimTransitions"]) {
        AddAnimTransitition(_animTransition);
    }
    return S_OK;
}


void CAnimController::Update(_float fDeltaTime)
{
    if (!m_bIsActive) return;
    m_vecAnimNodes[m_iCurrentNodeIdx]->Update(fDeltaTime);
}

void CAnimController::UpdateCTX(const ANIMCTX_DESC& ctx)
{
    if (!m_bIsActive) return;
    CheckTransition(ctx);
    if (m_transitDesc.m_iDstAnimNodeIdx == -1) {
        //CheckTransition(ctx);
    }
    else {
        if (m_vecAnimNodes[m_iCurrentNodeIdx]->CanExit(m_transitDesc)) {
            m_vecAnimNodes[m_iCurrentNodeIdx]->Exit(m_transitDesc);
            m_iCurrentNodeIdx = m_transitDesc.m_iDstAnimNodeIdx;
            m_vecAnimNodes[m_iCurrentNodeIdx]->Enter(m_transitDesc);
            m_transitDesc = ANIMTRANSITION_DESC{};
        }
    }
    m_vecAnimNodes[m_iCurrentNodeIdx]->UpdateCTX(ctx);
}

bool CAnimController::CanExit(_bool bHasExitTIme, _float fRatio)
{
    return m_vecAnimNodes[m_iCurrentNodeIdx]->CanExit(m_transitDesc);
}

void CAnimController::Enter()
{
    if (m_iStartNodeIdx<0) {
        MSG_BOX("No Start Node Set : 0 default");
        m_iStartNodeIdx = 0;
    }

    m_transitDesc.m_bHasExitTIme    = false;
    m_transitDesc.m_fBlendTime      = 0.2f;
    m_transitDesc.m_iDstAnimNodeIdx = m_iStartNodeIdx;
    m_vecAnimNodes[m_iStartNodeIdx]->Enter(m_transitDesc);
    m_iCurrentNodeIdx = m_iStartNodeIdx;
    m_transitDesc = ANIMTRANSITION_DESC{};

    if (m_pEnterFunc) m_pEnterFunc();
}

void CAnimController::Exit()
{
    m_vecAnimNodes[m_iCurrentNodeIdx]->Exit(m_transitDesc);
    m_iCurrentNodeIdx = -1;
    if (m_pExitFunc) m_pExitFunc();
}

void CAnimController::SetActive(_bool bIsActive)
{
    if (!m_bIsActive  && bIsActive ) Enter();
    if (m_bIsActive   && !bIsActive) Exit();
    m_bIsActive = bIsActive;
}

void CAnimController::BindExitFunc(function<void(void)> pFunc)
{
    m_pExitFunc = pFunc;
}

void CAnimController::BindEnterFunc(function<void(void)> pFunc)
{
    m_pEnterFunc = pFunc;
}

void CAnimController::SetLayerIdx(const string& sLayerTag, int idx)
{
    _bool _bFail = true;
    auto it = m_mapLayerAnimNodes.find(sLayerTag);
    if (it != m_mapLayerAnimNodes.end()) {
        for (auto& pLayerNode : it->second) {
            pLayerNode->SetLayerIdx(idx);
        }
        _bFail = false;
    }

    auto it0 = m_mapLayerActionAnimNodes.find(sLayerTag);
    if (it0 != m_mapLayerActionAnimNodes.end()) {
        for (auto& pLayerActionNode : it0->second) {
            pLayerActionNode->SetLayerIdx(idx);
        }
        _bFail = false;
    }

    if (_bFail) {
        MSG_BOX("Fail to set Layer idx : Fail to find layer tag");

    }

}

unique_ptr<CAnimController> CAnimController::Clone(shared_ptr<CModel> spModel)
{
    unique_ptr<CAnimController> _upAnimController = unique_ptr<CAnimController>(new CAnimController(*this, spModel));
    return _upAnimController;
}

unique_ptr<CAnimController> CAnimController::Create(Json::Value jsonValue, shared_ptr<CModel> spModel)
{
    if (!spModel) {
        MSG_BOX("Fail to create AnimController : Empty Model");
        return nullptr;
    }
    unique_ptr<CAnimController> upAnimController = unique_ptr<CAnimController>(new CAnimController(spModel));
    if (FAILED(upAnimController->Initialize(jsonValue))) {
        MSG_BOX("Fail to create AnimController : fail to initialize");
        return nullptr;
    }

    return upAnimController;
}

void CAnimController::CheckTransition(const ANIMCTX_DESC& ctx)
{
    int _iTransitionAnimIdx = -1;
    for (auto iTransitionIdx : m_vecAnyStateTransitions) {
        if (m_vecTransitions[iTransitionIdx]->Evaluate(ctx)) {
            _iTransitionAnimIdx = iTransitionIdx;
            break;
        }
    }
    if (_iTransitionAnimIdx==-1) {
        for (auto iTransitionIdx : m_tableTransitions[m_iCurrentNodeIdx]) {
            if (m_vecTransitions[iTransitionIdx]->Evaluate(ctx)) {
                _iTransitionAnimIdx = iTransitionIdx;
                break;
            }
        }
    }
    if (_iTransitionAnimIdx != -1) {
        if (m_vecTransitions[_iTransitionAnimIdx]->GetDesc().m_iPriority < m_transitDesc.m_iPriority) {
            m_transitDesc = m_vecTransitions[_iTransitionAnimIdx]->GetDesc();
        }
            
    }
    return;
}

_int CAnimController::FindAnimNode(const string& sIDtag)
{
    for (size_t i = 0; i < m_vecAnimNodes.size(); i++) {
        if (m_vecAnimNodes[i]->GetID() == sIDtag) return (_int)i;
    }
    return -1;
}
