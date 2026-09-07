#include "SingleAnimNode.h"
#include "BaseModel.h"

CSingleAnimNode::CSingleAnimNode(shared_ptr<CModel> spModel)
	: CAnimNode(spModel)
{
}

CSingleAnimNode::CSingleAnimNode(const CSingleAnimNode& prototype, shared_ptr<CModel> spModel)
	: CAnimNode(prototype, spModel)
{
}

void CSingleAnimNode::Update(float _fDeltaTime)
{
}

void CSingleAnimNode::UpdateCTX(const ANIMCTX_DESC& ctx)
{
}

bool CSingleAnimNode::CanExit(ANIMTRANSITION_DESC _desc)
{
    _bool bCanExit = true;
    if (_desc.m_bHasExitTIme) {
        auto _fRatio = m_wpModel.lock()->GetAnimRatio(m_animClipDesc.iLocomotionAnimIdx);
        bCanExit = (_fRatio >= _desc.m_fExitRatio);
    }
    else {
        bCanExit = true;
    }
    if (bCanExit) {
        //PrintDebug("Transit Info \t: ", _desc.m_iSrtAnimNodeIdx, "->", _desc.m_iDstAnimNodeIdx, "\tHasExitTIme : ", (_desc.m_bHasExitTIme) ? "True" : "False", "\tExitRatio : ", _desc.m_fExitRatio);
    }
    

    return bCanExit;
}

void CSingleAnimNode::Enter(ANIMTRANSITION_DESC _desc)
{
    CLIPTRANSIT_DESC _transitDesc;
    _transitDesc.fBlendDuration = _desc.m_fBlendTime;
    _transitDesc.bForceTransit = true;
    m_wpModel.lock()->TransitLocomotion(m_animClipDesc, _transitDesc);
    //PrintDebug("Enter Locomo \t: ", m_wpModel.lock()->FindAnimTag(m_animClipDesc.iLocomotionAnimIdx));
}

void CSingleAnimNode::Exit(ANIMTRANSITION_DESC _desc)
{
}

bool CSingleAnimNode::IsEnd()
{
    if (m_animClipDesc.bIsLocomotionAnimLoop) return false;
    auto _fRatio = m_wpModel.lock()->GetAnimRatio(m_animClipDesc.iLocomotionAnimIdx);
    return _fRatio>=0.98f;
}

unique_ptr<CAnimNode> CSingleAnimNode::Clone(shared_ptr<CModel> spModel)
{
    if (!spModel) {
        MSG_BOX("Fail to clone SingleAnimNOde : Empty Model");
        return nullptr;
    }

	unique_ptr<CSingleAnimNode> _upSingleAnimNode = unique_ptr<CSingleAnimNode>(new CSingleAnimNode(*this, spModel));
	return _upSingleAnimNode;
}

unique_ptr<CSingleAnimNode> CSingleAnimNode::Create(Json::Value jsonValue, shared_ptr<CModel> spModel)
{
    if (!spModel) {
        MSG_BOX("Fail to create SingleAnimNOde : Empty Model");
        return nullptr;
    }
    unique_ptr<CSingleAnimNode> _upSingleAnimNode = unique_ptr<CSingleAnimNode>(new CSingleAnimNode(spModel));
    if (FAILED(_upSingleAnimNode->Initialize(jsonValue))) {
        MSG_BOX("Fail to create SingleAnimNOde : fail to initialize");
        return nullptr;
    }

    return _upSingleAnimNode;
}

HRESULT CSingleAnimNode::Initialize(Json::Value jsonValue)
{
    CHKFAIL(__super::Initialize(jsonValue))
    LoadClip(m_animClipDesc, jsonValue["Clips"][0]);
	return S_OK;
}

void CSingleAnimNode::SetRootMotionScale(float fXScale, float fYScale)
{
    m_animClipDesc.fRootMotionScale.x = fXScale;
    m_animClipDesc.fRootMotionScale.y = fYScale;
}

void CSingleAnimNode::Reset()
{
    m_wpModel.lock()->AnimReset(m_animClipDesc.iLocomotionAnimIdx);
}
