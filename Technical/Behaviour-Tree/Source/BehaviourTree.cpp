#include "BehaviourTree.h"
#include "BT_Node.h"
#include "BlackBoard.h"
#include "cgraph.h"
#include "gvc.h"

CBehaviourTree::CBehaviourTree()
{
}

HRESULT CBehaviourTree::Initialize(void* pArg)
{
	m_pBlackBoard = unique_ptr<CBlackBoard>(CBlackBoard::Create());

	return S_OK;
}

CBT_Node::ESTATE CBehaviourTree::Tick(_float fTimeDelta)
{
	return m_pRootNode->Tick(fTimeDelta);
}

CBlackBoard* CBehaviourTree::GetBlackBoard()
{
	return m_pBlackBoard.get();
}

CBehaviourTree* CBehaviourTree::Create(void* pArg)
{
    CBehaviourTree* pTree = new CBehaviourTree();
    if (FAILED(pTree->Initialize(pArg))) {
        MSG_BOX("Fail to Create Task");
        return nullptr;
    }
    return pTree;
}

HRESULT CBehaviourTree::AddRootNode(CBT_Node* pRootNode)
{
    m_pRootNode = unique_ptr<CBT_Node>(pRootNode);
    return S_OK;
}

void SetGraphSpacing(
    Agraph_t* pGraph,
    float siblingGapPixel,
    float depthGapPixel)
{
    constexpr float DPI = 96.f;

    std::string nodeSep =
        std::to_string(siblingGapPixel / DPI);

    std::string rankSep =
        std::to_string(depthGapPixel / DPI);

    agsafeset(
        pGraph,
        const_cast<char*>("nodesep"),
        nodeSep.data(),
        const_cast<char*>("")
    );

    agsafeset(
        pGraph,
        const_cast<char*>("ranksep"),
        rankSep.data(),
        const_cast<char*>("")
    );
}

void CBehaviourTree::RenderBT()
{
    GVC_t* pContext = gvContext();

    Agraph_t* pGraph = agopen(
        const_cast<char*>("BT"),
        Agdirected,
        nullptr
    );

    agsafeset(
        pGraph,
        const_cast<char*>("rankdir"),
        const_cast<char*>("TB"),
        const_cast<char*>("")
    );
    SetGraphSpacing(
        pGraph,
        15.f,
        20.f
    );



    //======================================
    static ed::Config config;
    static ed::EditorContext* m_Context = ed::CreateEditor(&config);
    
    ImGui::PushStyleColor(
        ImGuiCol_WindowBg,
        ImVec4(0.f, 0.f, 0.f, 0.5f)
    );

    ImGui::Begin("BT");

    ed::SetCurrentEditor(m_Context);
    ed::PushStyleColor(
        ed::StyleColor_Bg,
        ImVec4(0.f, 0.f, 0.f, 0.f)
    );
    ed::Begin("My Editor", ImVec2(0.0, 0.0f));

    ed::ClearSelection();

    if (m_pRootNode) {
        m_pRootNode->DrawNode(pGraph);
    }

    int result = gvLayout(pContext, pGraph, "dot");

    if (result == 0) {
        m_pRootNode->SetGVNodePos();
    }

    ed::NavigateToSelection(
        false,
        0.35f
    );

    ed::End();
    ed::PopStyleColor();
    ed::SetCurrentEditor(nullptr);

    ImGui::End();

    ImGui::PopStyleColor();
    //======================================


   /* if (result == 0)
    {
        auto root   = ND_coord(pRoot);
        auto a      = ND_coord(pA);
        auto b      = ND_coord(pB);

        printf("Root : %f, %f\n", root.x, root.y);
        printf("A    : %f, %f\n", a.x, a.y);
        printf("B    : %f, %f\n", b.x, b.y);
    }*/

    gvFreeLayout(pContext, pGraph);
    agclose(pGraph);
    gvFreeContext(pContext);
}
