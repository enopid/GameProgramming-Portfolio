#include "LayerActionAnimNode.h"
#include "BaseModel.h"

CLayerActionAnimNode::CLayerActionAnimNode(shared_ptr<CModel> spModel)
	: CAnimNode(spModel)
{
}

CLayerActionAnimNode::CLayerActionAnimNode(const CLayerActionAnimNode& prototype, shared_ptr<CModel> spModel)
	: CAnimNode(prototype, spModel)
{
}

void CLayerActionAnimNode::Update(float _fDeltaTime)
{
}

void CLayerActionAnimNode::UpdateCTX(const ANIMCTX_DESC& ctx)
{
}

bool CLayerActionAnimNode::CanExit(ANIMTRANSITION_DESC _desc)
{
    _bool bCanExit = true;
    if (_desc.m_bHasExitTIme) {
        auto _fRatio = m_wpModel.lock()->GetAnimRatio(m_vecAnimClipDesc[m_iLayerIdx].iActionAnimIdx);
        bCanExit = (_fRatio >= _desc.m_fExitRatio);
    }
    else {
        bCanExit = true;
    }
    if (bCanExit) {
        if (m_iLayerIdx!= m_iNxtLayerIdx)
            PrintDebug("Transit Info \t: ", _desc.m_iSrtAnimNodeIdx, "->", _desc.m_iDstAnimNodeIdx, "\tHasExitTIme : ", (_desc.m_bHasExitTIme) ? "True" : "False", "\tExitRatio : ", _desc.m_fExitRatio);
    }
    

    return bCanExit;
}

void CLayerActionAnimNode::Enter(ANIMTRANSITION_DESC _desc)
{
    CLIPTRANSIT_DESC _transitDesc;
    _transitDesc.fBlendDuration = _desc.m_fBlendTime;
    _transitDesc.bForceTransit = true;
    m_iLayerIdx = m_iNxtLayerIdx;
    m_wpModel.lock()->TransitAction(m_vecAnimClipDesc[m_iLayerIdx], _transitDesc);
    //PrintDebug("Enter Layer \t: ", m_wpModel.lock()->FindAnimTag(m_vecAnimClipDesc[m_iLayerIdx].iLocomotionAnimIdx));
}

void CLayerActionAnimNode::Exit(ANIMTRANSITION_DESC _desc)
{
}

bool CLayerActionAnimNode::IsEnd()
{
    if (m_vecAnimClipDesc[m_iLayerIdx].bIsLocomotionAnimLoop) return false;
    auto _fRatio = m_wpModel.lock()->GetAnimRatio(m_vecAnimClipDesc[m_iLayerIdx].iLocomotionAnimIdx);
    return _fRatio >= 0.98f;
}

void CLayerActionAnimNode::SetLayerIdx(int i)
{
    if (m_vecAnimClipDesc.size() <= i) {
        MSG_BOX("Fail to Set Layer Idx : Layer Idx is too large");
        return;
    }
    m_iNxtLayerIdx = i;
}

string CLayerActionAnimNode::GetLayerTag()
{
    return m_sLayerIdxTag;
}

unique_ptr<CAnimNode> CLayerActionAnimNode::Clone(shared_ptr<CModel> spModel)
{
    if (!spModel) {
        MSG_BOX("Fail to clone LayerAnimNOde : Empty Model");
        return nullptr;
    }

	unique_ptr<CLayerActionAnimNode> _upLayerAnimNode = unique_ptr<CLayerActionAnimNode>(new CLayerActionAnimNode(*this, spModel));
	return _upLayerAnimNode;
}

unique_ptr<CLayerActionAnimNode> CLayerActionAnimNode::Create(Json::Value jsonValue, shared_ptr<CModel> spModel)
{
    if (!spModel) {
        MSG_BOX("Fail to create LayerAnimNOde : Empty Model");
        return nullptr;
    }
    unique_ptr<CLayerActionAnimNode> _upSingleAnimNode = unique_ptr<CLayerActionAnimNode>(new CLayerActionAnimNode(spModel));
    if (FAILED(_upSingleAnimNode->Initialize(jsonValue))) {
        MSG_BOX("Fail to create LayerAnimNode : fail to initialize");
        return nullptr;
    }

    return _upSingleAnimNode;
}

HRESULT CLayerActionAnimNode::Initialize(Json::Value jsonValue)
{
    CHKFAIL(__super::Initialize(jsonValue))
    m_sLayerIdxTag = jsonValue["LayerIdxTag"].asString();
    m_vecAnimClipDesc.resize(jsonValue["Clips"].size());
    for (int i = 0; i < (int)jsonValue["Clips"].size(); i++)
    {
        LoadClip(m_vecAnimClipDesc[i], jsonValue["Clips"][i]);
    }
	return S_OK;
}

void CLayerActionAnimNode::SetRootMotionScale(float fXScale, float fYScale)
{
    for (size_t i = 0; i < m_vecAnimClipDesc.size(); i++)
    {
        m_vecAnimClipDesc[i].fRootMotionScale.x = fXScale;
        m_vecAnimClipDesc[i].fRootMotionScale.y = fYScale;
    }
}

void CLayerActionAnimNode::Reset()
{
    //m_wpModel.lock()->AnimReset(m_vecAnimClipDesc[m_iLayerIdx].iActionAnimIdx);
}
