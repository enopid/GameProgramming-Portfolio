#include "ActionAnimNode.h"
#include "BaseModel.h"

CActionAnimNode::CActionAnimNode(shared_ptr<CModel> spModel)
	: CAnimNode(spModel)
{
}

CActionAnimNode::CActionAnimNode(const CActionAnimNode& prototype, shared_ptr<CModel> spModel)
	: CAnimNode(prototype, spModel)
{
}

void CActionAnimNode::Update(float _fDeltaTime)
{
}

void CActionAnimNode::UpdateCTX(const ANIMCTX_DESC& ctx)
{
}

bool CActionAnimNode::CanExit(ANIMTRANSITION_DESC _desc)
{
    _bool bCanExit = true;
    if (_desc.m_bHasExitTIme) {
        auto _fRatio = m_wpModel.lock()->GetAnimRatio(m_animClipDesc.iActionAnimIdx);
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

void CActionAnimNode::Enter(ANIMTRANSITION_DESC _desc)
{
    CLIPTRANSIT_DESC _transitDesc;
    _transitDesc.fBlendDuration = _desc.m_fBlendTime;
    _transitDesc.bForceTransit = true;
    m_wpModel.lock()->TransitAction(m_animClipDesc, _transitDesc);

    //PrintDebug("Enter Action \t: ", m_wpModel.lock()->FindAnimTag(m_animClipDesc.iActionAnimIdx));
}

void CActionAnimNode::Exit(ANIMTRANSITION_DESC _desc)
{
}

unique_ptr<CAnimNode> CActionAnimNode::Clone(shared_ptr<CModel> spModel)
{
    if (!spModel) {
        MSG_BOX("Fail to clone ActionAnimNOde : Empty Model");
        return nullptr;
    }

	unique_ptr<CActionAnimNode> _upSingleAnimNode = unique_ptr<CActionAnimNode>(new CActionAnimNode(*this, spModel));
	return _upSingleAnimNode;
}

unique_ptr<CActionAnimNode> CActionAnimNode::Create(Json::Value jsonValue, shared_ptr<CModel> spModel)
{
    if (!spModel) {
        MSG_BOX("Fail to create SingleAnimNOde : Empty Model");
        return nullptr;
    }
    unique_ptr<CActionAnimNode> _upSingleAnimNode = unique_ptr<CActionAnimNode>(new CActionAnimNode(spModel));
    if (FAILED(_upSingleAnimNode->Initialize(jsonValue))) {
        MSG_BOX("Fail to create SingleAnimNOde : fail to initialize");
        return nullptr;
    }

    return _upSingleAnimNode;
}

HRESULT CActionAnimNode::Initialize(Json::Value jsonValue)
{
    CHKFAIL(__super::Initialize(jsonValue))
    LoadClip(m_animClipDesc, jsonValue["Clips"][0]);
	return S_OK;
}

void CActionAnimNode::SetRootMotionScale(float fXScale, float fYScale)
{
    m_animClipDesc.fRootMotionScale.x = fXScale;
    m_animClipDesc.fRootMotionScale.y = fYScale;
}

void CActionAnimNode::Reset()
{
    //m_wpModel.lock()->AnimReset(m_animClipDesc, _transitDesc);
}
