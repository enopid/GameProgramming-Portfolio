#include "Interact_Macaron_MonsterShield.h"
#include "BaseModel.h"
#include "AnimController.h"
#include "GameInstance.h"
#include "MeshRenderer.h"

CGameObject* CInteract_Macaron_MonsterShield::Clone(void* pArg)
{
	return CloneBase<CInteract_Macaron_MonsterShield>(pArg);
}

HRESULT CInteract_Macaron_MonsterShield::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents());
	return S_OK;
}

CInteract_Macaron_MonsterShield::CInteract_Macaron_MonsterShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteract_RhythmChallenge(pDevice, pContext)
{
	m_eInteractType = EInteractType::IT_MACARON_SHIELD;
}

CInteract_Macaron_MonsterShield::CInteract_Macaron_MonsterShield(const CInteract_Macaron_MonsterShield& Prototype)
	: CInteract_RhythmChallenge(Prototype)
{
	m_eInteractType = EInteractType::IT_MACARON_SHIELD;
}

HRESULT CInteract_Macaron_MonsterShield::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
		return S_OK;
}

HRESULT CInteract_Macaron_MonsterShield::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	//Model
	CModel::MODEL_DESC _modelDesc;
	lstrcpy(_modelDesc.m_sModelAssetName, L"GimmicObject_Macaron_MonsterShield");
	_modelDesc.m_bIsCopy = false;
	Add_Component<CModel>(L"Com_Model_Main", &_modelDesc);

	auto wpComp = Get_Component<CModel>();
	m_wpModel = ConvertWPComponent<CModel>(wpComp);

	return S_OK;
}

HRESULT CInteract_Macaron_MonsterShield::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize())
	Set_Animator(L"../AnimNodes/Partner/1.Macaron/Gimmic/Animator_Gimmic_Macaron_MonsterShield.json");

	auto _spModel = m_wpModel.lock();


	CRenderer::SHADERPARAM_DESC _desc;
	_desc.eParamType = CRenderer::EPARAMTYPE::PT_RAWDATA;
	_desc.iSize = sizeof(_float);
	_desc.pData = &m_fAmount;
	_desc.sConstantName = "g_fU";
	m_wpModel.lock()->GetMeshRenderer(5)->Add_Parameters(_desc);

	return S_OK;
}

void CInteract_Macaron_MonsterShield::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CInteract_Macaron_MonsterShield::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_fAmount += fTimeDelta;
}

void CInteract_Macaron_MonsterShield::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CInteract_Macaron_MonsterShield::Render_Properties()
{
	__super::Render_Properties();
}

void CInteract_Macaron_MonsterShield::Free()
{
	__super::Free();
}
