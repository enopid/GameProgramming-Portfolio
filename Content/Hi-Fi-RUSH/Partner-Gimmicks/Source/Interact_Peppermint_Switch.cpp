#include "Interact_Peppermint_Switch.h"
#include "BaseModel.h"
#include "AnimController.h"
#include "GameInstance.h"
#include "UC_Interact.h"
#include "MeshRenderer.h"
#include "EffectRoot.h"

CGameObject* CInteract_Peppermint_Switch::Clone(void* pArg)
{
    return CloneBase<CInteract_Peppermint_Switch>(pArg);
}

HRESULT CInteract_Peppermint_Switch::Add_InitComponents()
{
    CHKFAIL(__super::Add_InitComponents());
    return S_OK;
}

CInteract_Peppermint_Switch::CInteract_Peppermint_Switch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractObject(pDevice, pContext)
{
    m_eInteractType = IT_PEPPERMINT_SWITCH;
}

CInteract_Peppermint_Switch::CInteract_Peppermint_Switch(const CInteract_Peppermint_Switch& Prototype)
    : CInteractObject(Prototype)
{
    m_eInteractType = IT_PEPPERMINT_SWITCH;
}

HRESULT CInteract_Peppermint_Switch::Initialize_Prototype(void* pArg)
{
    CHKFAIL(__super::Initialize_Prototype(pArg))
    return S_OK;
}

HRESULT CInteract_Peppermint_Switch::Initialize(void* pDesc)
{
    CHKFAIL(__super::Initialize(pDesc))

    //Model
    CModel::MODEL_DESC _modelDesc;
    lstrcpy(_modelDesc.m_sModelAssetName, L"GimmicObject_Peppermint_Switch_Export");
    _modelDesc.m_bIsCopy = false;
    Add_Component<CModel>(L"Com_Model_Main", &_modelDesc);

    auto wpComp = Get_Component<CModel>();
    m_wpModel = ConvertWPComponent<CModel>(wpComp);

    m_vColor = { 1.f,0.f,0.f };
    AddStateFunc(ESTATE::TRIGGER, EKEYACTIONSTATE::ENTER, [=](auto) {
        m_vColor = { 0.f,1.f,0.f }; 
        m_pSwitchOnEffect->Set_Pos(
            Get_MainTransform().lock()->Get_WorldState(STATE::POSITION)
            + XMVector3Normalize(Get_MainTransform().lock()->Get_WorldState(STATE::LOOK)) * 0.8f
        );
        m_pSwitchOnEffect->Restart();
    });

    return S_OK;
}

HRESULT CInteract_Peppermint_Switch::Late_Initialize()
{
    CHKFAIL(__super::Late_Initialize())
    Set_Animator(L"../AnimNodes/Partner/0.Peppermint/Gimmic/Animator_Gimmic_Peppermint_Switch.json");
   
    m_wpMeshRenderer = m_wpModel.lock()->GetMeshRenderer(1);
    CRenderer::SHADERPARAM_DESC desc = {};
    desc.eParamType = CRenderer::EPARAMTYPE::PT_RAWDATA;
    desc.iSize = sizeof(Engine::_float3);
    desc.sConstantName = "g_vEmissiveColor";
    desc.pData = { &m_vColor };
    m_wpMeshRenderer.lock()->Add_Parameters(desc);

    CGameObject::LoadPrototype(
        L"VFX_Switch_On",
        m_pGameInstance->GetLevelDesc().nextLevelTag,
        reinterpret_cast<CGameObject**>(&m_pSwitchOnEffect)
    );
    m_pSwitchOnEffect->Stop();
    m_pSwitchOnEffect->SetLevelObject();

    return S_OK;
}

void CInteract_Peppermint_Switch::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CInteract_Peppermint_Switch::Update(_float fTimeDelta)
{
    if (m_eCurState == ESTATE::DEACTIVE) Activate();

    __super::Update(fTimeDelta);
}

void CInteract_Peppermint_Switch::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

void CInteract_Peppermint_Switch::Render_Properties()
{
    __super::Render_Properties();
}

void CInteract_Peppermint_Switch::Free()
{
    __super::Free();
}


