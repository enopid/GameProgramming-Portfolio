#include "Interact_Macaron_Platform.h"
#include "Interact_Electric_CirclePlatform.h"
#include "BaseModel.h"
#include "AnimController.h"
#include "GameInstance.h"
#include "MeshRenderer.h"

CGameObject* CInteract_Macaron_Platform::Clone(void* pArg)
{
	return CloneBase<CInteract_Macaron_Platform>(pArg);
}

HRESULT CInteract_Macaron_Platform::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents());
	return S_OK;
}

CInteract_Macaron_Platform::CInteract_Macaron_Platform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteract_RhythmChallenge(pDevice, pContext)
{
	m_eInteractType = EInteractType::IT_MACARON_PLATFORM;
}

CInteract_Macaron_Platform::CInteract_Macaron_Platform(const CInteract_Macaron_Platform& Prototype)
	: CInteract_RhythmChallenge(Prototype)
{
	m_eInteractType = EInteractType::IT_MACARON_PLATFORM;
}

HRESULT CInteract_Macaron_Platform::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
		return S_OK;
}

HRESULT CInteract_Macaron_Platform::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	//Model
	CModel::MODEL_DESC _modelDesc;
	lstrcpy(_modelDesc.m_sModelAssetName, L"GimmicObject_Macaron_Gimmic_Export");
	_modelDesc.m_bIsCopy = false;
	Add_Component<CModel>(L"Com_Model_Main", &_modelDesc);

	auto wpComp = Get_Component<CModel>();
	m_wpModel = ConvertWPComponent<CModel>(wpComp);

	m_vColor = { 0.8f,0.8f,0.f };

	return S_OK;
}

HRESULT CInteract_Macaron_Platform::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize())
	Set_Animator(L"../AnimNodes/Partner/1.Macaron/Gimmic/Animator_Gimmic_Macaron_Platform.json");
	
	m_pMacaron_Platform = static_cast<CInteract_Electric_CirclePlatform*>(m_pGameInstance->Get_Object(
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		L"Layer_Interact",
		L"HappyHappyMacaron"
	));

	auto _spModel = m_wpModel.lock();
	_spModel->AddListener(L"IsGimmicEnd", nullptr, [=]() { 
		SetActive(false);
		if (m_pMacaron_Platform) m_pMacaron_Platform->SetActive(true);
		if (m_pMacaron_Platform) m_pMacaron_Platform->Activate();
	}, nullptr);

	m_wpMeshRenderer = m_wpModel.lock()->GetMeshRenderer(2);
	CRenderer::SHADERPARAM_DESC desc = {};
	desc.eParamType = CRenderer::EPARAMTYPE::PT_RAWDATA;
	desc.iSize = sizeof(_float3);
	desc.sConstantName = "g_vEmissiveColor";
	desc.pData = &m_vColor;
	m_wpMeshRenderer.lock()->Add_Parameters(desc);

	return S_OK;
}

void CInteract_Macaron_Platform::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CInteract_Macaron_Platform::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	if (!m_pMacaron_Platform) return;

	if (m_eCurState == ESTATE::TRIGGER) {
	}
	else {
		m_pMacaron_Platform->SetActive(false);
		m_pMacaron_Platform->Deactivate();
	}
}

void CInteract_Macaron_Platform::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CInteract_Macaron_Platform::Render_Properties()
{
	__super::Render_Properties();
}

void CInteract_Macaron_Platform::Free()
{
	__super::Free();
}
