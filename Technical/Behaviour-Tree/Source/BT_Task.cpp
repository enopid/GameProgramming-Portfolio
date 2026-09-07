#include "BT_Task.h"
#include "cgraph.h"
#include "gvc.h"

CBT_Task::CBT_Task()
    : CBT_Node()
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

RECT CBT_Task::DrawNode(Agraph_t* pGraph)
{
    ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.f, 1.f));
    ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.f, -1.f));
    if (m_eNodeState == CBT_Node::ESTATE::ES_RUNNING) {
        ed::PushStyleColor(
            ed::StyleColor_NodeBg,
            ImColor(208, 215, 15)
        );
    }
    else if (m_eNodeState == CBT_Node::ESTATE::ES_FAIL) {
        ed::PushStyleColor(
            ed::StyleColor_NodeBg,
            ImColor(240, 15, 15)
        );
    }
    else if (m_eNodeState == CBT_Node::ESTATE::ES_SUCCESS) {
        ed::PushStyleColor(
            ed::StyleColor_NodeBg,
            ImColor(15, 215, 15)
        );
    }
    else {
        ed::PushStyleColor(
            ed::StyleColor_NodeBg,
            ImColor(73, 72, 73)
        );
    }
    m_eNodeState = m_eState;

    //PushFont
    string _nodeName = ws2s(m_nodeDesc.m_sNodeTag);
    ImVec2 _textSize = ImGui::CalcTextSize(_nodeName.c_str());
    float bodyWidth  = max(_textSize.x + BODY_PADDINGX * 2.f, 40.f);
    float bodyHeight = 20.f;
    float pinWidth   = bodyWidth - PIN_PADDINGX * 2.f;

    ed::BeginNode(m_NodeID);

    //toppin
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + PIN_PADDINGX);
    ed::BeginPin(m_InputPinID, ed::PinKind::Input);
    ImGui::Dummy(ImVec2(pinWidth, PIN_HEIGHT));
    ed::EndPin();

    //body
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + BODY_PADDINGX);
    ImGui::Text("%s", ws2s(m_nodeDesc.m_sNodeTag).c_str());
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(BODY_PADDINGX, 0.f));


    ed::EndNode();

    ed::PopStyleVar(2);
    ed::PopStyleColor(1);

    std::string gvID = std::to_string(m_NodeID.Get());
    m_pGVNode = agnode(pGraph, gvID.data(), 1);
    SetGVNodeSize();

    return m_NodeRect;
}
