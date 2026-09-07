#include "LayerAnimNode.h"
#include "BaseModel.h"

CLayerAnimNode::CLayerAnimNode(shared_ptr<CModel> spModel)
	: CAnimNode(spModel)
{
}

CLayerAnimNode::CLayerAnimNode(const CLayerAnimNode& prototype, shared_ptr<CModel> spModel)
	: CAnimNode(prototype, spModel)
{
}

void CLayerAnimNode::Update(float _fDeltaTime)
{
}

void CLayerAnimNode::UpdateCTX(const ANIMCTX_DESC& ctx)
{
}

bool CLayerAnimNode::CanExit(ANIMTRANSITION_DESC _desc)
{
    _bool bCanExit = true;
    if (_desc.m_bHasExitTIme) {
        auto _fRatio = m_wpModel.lock()->GetAnimRatio(m_vecAnimClipDesc[m_iLayerIdx].iLocomotionAnimIdx);
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

void CLayerAnimNode::Enter(ANIMTRANSITION_DESC _desc)
{
    CLIPTRANSIT_DESC _transitDesc;
    _transitDesc.fBlendDuration = _desc.m_fBlendTime;
    _transitDesc.bForceTransit = true;
    m_iLayerIdx = m_iNxtLayerIdx;
    m_wpModel.lock()->TransitLocomotion(m_vecAnimClipDesc[m_iLayerIdx], _transitDesc);
    //PrintDebug("Enter Layer \t: ", m_wpModel.lock()->FindAnimTag(m_vecAnimClipDesc[m_iLayerIdx].iLocomotionAnimIdx));
}

void CLayerAnimNode::Exit(ANIMTRANSITION_DESC _desc)
{
}

bool CLayerAnimNode::IsEnd()
{
    if (m_vecAnimClipDesc[m_iLayerIdx].bIsLocomotionAnimLoop) return false;
    auto _fRatio = m_wpModel.lock()->GetAnimRatio(m_vecAnimClipDesc[m_iLayerIdx].iLocomotionAnimIdx);
    return _fRatio >= 0.98f;
}

void CLayerAnimNode::SetLayerIdx(int i)
{
    if (m_vecAnimClipDesc.size() <= i) {
        MSG_BOX("Fail to Set Layer Idx : Layer Idx is too large");
        return;
    }
    m_iNxtLayerIdx = i;
}

string CLayerAnimNode::GetLayerTag()
{
    return m_sLayerIdxTag;
}

unique_ptr<CAnimNode> CLayerAnimNode::Clone(shared_ptr<CModel> spModel)
{
    if (!spModel) {
        MSG_BOX("Fail to clone LayerAnimNOde : Empty Model");
        return nullptr;
    }

	unique_ptr<CLayerAnimNode> _upLayerAnimNode = unique_ptr<CLayerAnimNode>(new CLayerAnimNode(*this, spModel));
	return _upLayerAnimNode;
}

unique_ptr<CLayerAnimNode> CLayerAnimNode::Create(Json::Value jsonValue, shared_ptr<CModel> spModel)
{
    if (!spModel) {
        MSG_BOX("Fail to create LayerAnimNOde : Empty Model");
        return nullptr;
    }
    unique_ptr<CLayerAnimNode> _upSingleAnimNode = unique_ptr<CLayerAnimNode>(new CLayerAnimNode(spModel));
    if (FAILED(_upSingleAnimNode->Initialize(jsonValue))) {
        MSG_BOX("Fail to create LayerAnimNode : fail to initialize");
        return nullptr;
    }

    return _upSingleAnimNode;
}

HRESULT CLayerAnimNode::Initialize(Json::Value jsonValue)
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

void CLayerAnimNode::SetRootMotionScale(float fXScale, float fYScale)
{
    for (size_t i = 0; i < m_vecAnimClipDesc.size(); i++)
    {
        m_vecAnimClipDesc[i].fRootMotionScale.x = fXScale;
        m_vecAnimClipDesc[i].fRootMotionScale.y = fYScale;
    }
}

void CLayerAnimNode::Reset()
{
    m_wpModel.lock()->AnimReset(m_vecAnimClipDesc[m_iLayerIdx].iLocomotionAnimIdx);
}
