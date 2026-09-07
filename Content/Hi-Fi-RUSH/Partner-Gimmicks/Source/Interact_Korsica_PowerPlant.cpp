#include "Interact_Korsica_PowerPlant.h"
#include "BaseModel.h"
#include "AnimController.h"
#include "GameInstance.h"
#include "UC_UIEffect.h"
#include "MeshRenderer.h"

CGameObject* CInteract_Korsica_PowerPlant::Clone(void* pArg)
{
	return CloneBase<CInteract_Korsica_PowerPlant>(pArg);
}

HRESULT CInteract_Korsica_PowerPlant::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents());
	return S_OK;
}

CInteract_Korsica_PowerPlant::CInteract_Korsica_PowerPlant(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteract_RhythmChallenge(pDevice, pContext)
{
	m_eInteractType = IT_KORSICA_GENERATOR;
}

CInteract_Korsica_PowerPlant::CInteract_Korsica_PowerPlant(const CInteract_Korsica_PowerPlant& Prototype)
	: CInteract_RhythmChallenge(Prototype)
{
	m_eInteractType = IT_KORSICA_GENERATOR;
}

HRESULT CInteract_Korsica_PowerPlant::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
		return S_OK;
}

HRESULT CInteract_Korsica_PowerPlant::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

		//Model
		CModel::MODEL_DESC _modelDesc;
	lstrcpy(_modelDesc.m_sModelAssetName, L"SK_bg0770");
	_modelDesc.m_bIsCopy = false;
	Add_Component<CModel>(L"Com_Model_Main", &_modelDesc);

	auto wpComp = Get_Component<CModel>();
	m_wpModel = ConvertWPComponent<CModel>(wpComp);

	_float fEmissivePower = 0.8f;
	m_vColor = { 0.f,fEmissivePower,0.f };
	AddStateFunc(ESTATE::TRIGGER, EKEYACTIONSTATE::ENTER, [=](auto) { if (m_pGameInstance->GetCurrentSequenceIndex() == ENUM_TO_UINT(SEQUENCE::NONE)) { m_pGameInstance->SetSequence(ENUM_TO_UINT(SEQUENCE::GENERATOR)); m_vColor = { fEmissivePower,0.f,0.f }; } });
	AddStateFunc(ESTATE::TRIGGER, EKEYACTIONSTATE::EXIT, [=](auto) { if (m_pGameInstance->GetCurrentSequenceIndex() == ENUM_TO_UINT(SEQUENCE::GENERATOR)) { m_pGameInstance->SetSequence(ENUM_TO_UINT(SEQUENCE::NONE)); m_vColor = { 0.f,fEmissivePower,0.f }; } });

	m_fSoundCullingRange = 25.f;

	return S_OK;
}

HRESULT CInteract_Korsica_PowerPlant::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize());

	Set_Animator(L"../AnimNodes/Partner/2.Korsica/Gimmic/Animator_Gimmic_Korsica_PowerPlant.json");

	Add_Listener();

	if (m_pGameInstance->GetCurrentLevelTag() != L"Level_Editor")
		m_pUCEffect = static_cast<CUC_UIEffect*>(m_pGameInstance->GetUC(L"UC_UIEffect"));

	m_wpMeshRenderer[0] = m_wpModel.lock()->GetMeshRenderer(0);
	m_wpMeshRenderer[1] = m_wpModel.lock()->GetMeshRenderer(1);
	m_wpMeshRenderer[2] = m_wpModel.lock()->GetMeshRenderer(3);

	CRenderer::SHADERPARAM_DESC desc = {};
	desc.eParamType = CRenderer::EPARAMTYPE::PT_RAWDATA;
	desc.iSize = sizeof(_float3);
	desc.sConstantName = "g_vEmissiveColor";
	desc.pData = &m_vColor;
	for (size_t i = 0; i < 3; ++i)
		m_wpMeshRenderer[i].lock()->Add_Parameters(desc);

	return S_OK;
}

void CInteract_Korsica_PowerPlant::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CInteract_Korsica_PowerPlant::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	if (m_ePrevState == ESTATE::TRIGGER && m_eCurState == ESTATE::DEACTIVE && !m_bRebootComplete)
		m_bIsReboot = true;

	if (m_eCurState == ESTATE::TRIGGER)
		m_bRebootComplete = false;

	auto spTransform = m_wpMainTransformCom.lock();

	if (spTransform && m_bIsReboot)
	{
		_float3 vPos{};
		XMStoreFloat3(&vPos, spTransform->Get_WorldState(Engine::STATE::POSITION));

		if (!IsOutofRange())
		{
			m_pGameInstance->Play_3D_EnvSound(
				L"sp_ie_com_power_start_space_01_Play",
				vPos,
				{ 0.f, 0.f, 0.f },
				0.7f, 2.5f, 14.f);
		}

		m_bIsReboot = false;
		m_bRebootComplete = true;
	}

	if (m_pUCEffect != nullptr)
		m_pUCEffect->StartPowerPlantDeActiveTime((Engine::_float)m_upDeactiveTimer->GetElapsedRatio());
}

void CInteract_Korsica_PowerPlant::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CInteract_Korsica_PowerPlant::Add_Listener()
{
	auto spModel = m_wpModel.lock();

	if (spModel)
	{
		spModel->AddListener(L"Idle_Beatsound", nullptr, [=]() {
			_float3 vPos = Get_CurrentPos();
			if (!IsOutofRange())
			{
				m_pGameInstance->Play_3D_EnvSound(
				L"obj_gmk_com_krsc_gene_beat_norm_01_Play",
				vPos,
				{ 0.f, 0.f, 0.f },
				0.7f, 2.5f, 14.f);
			}
		}, nullptr);

		spModel->AddListener(L"Idle_MainSound", nullptr, [=]() {
			_float3 vPos = Get_CurrentPos();
			if (!IsOutofRange())
			{
				m_pGameInstance->Play_3D_EnvSound(
					L"obj_prop_com_clean_01_Play",
					vPos,
					{ 0.f, 0.f, 0.f },
					0.7f, 2.5f, 14.f);
			}
			}, nullptr);

		spModel->AddListener(L"ActivateSound", nullptr, [=]() {
			_float3 vPos = Get_CurrentPos();
			if (!IsOutofRange())
			{
				m_pGameInstance->Play_3D_EnvSound(
					L"obj_gmk_com_krsc_gene_rattle_norm_01_Play",
					vPos,
					{ 0.f, 0.f, 0.f },
					0.7f, 2.5f, 14.f);
			}
			}, nullptr);

		spModel->AddListener(L"ActivateSound2", nullptr, [=]() {
			_float3 vPos = Get_CurrentPos();
			if (!IsOutofRange())
			{
				m_pGameInstance->Play_3D_EnvSound(
					L"obj_gmk_com_krsc_gene_head_rattle_01_Play",
					vPos,
					{ 0.f, 0.f, 0.f },
					0.7f, 2.5f, 14.f);
			}
			}, nullptr);

		spModel->AddListener(L"RunningSound", nullptr, [=]() {
			_float3 vPos = Get_CurrentPos();
			if (!IsOutofRange())
			{
				m_pGameInstance->Play_3D_EnvSound(
					L"obj_gmk_com_krsc_gene_run_break_01_Play",
					vPos,
					{ 0.f, 0.f, 0.f },
					0.7f, 2.5f, 14.f);
			}
			}, nullptr);

		spModel->AddListener(L"HatchSound", nullptr, [=]() {
			_float3 vPos = Get_CurrentPos();
			if (!IsOutofRange())
			{
				m_pGameInstance->Play_3D_EnvSound(
					L"obj_gmk_com_krsc_gene_hatch_rattle_01_Play",
					vPos,
					{ 0.f, 0.f, 0.f },
					0.7f, 2.5f, 14.f);
			}
			}, nullptr);

		spModel->AddListener(L"HatchStop", nullptr, [=]() {
			_float3 vPos = Get_CurrentPos();
			if (!IsOutofRange())
			{
				m_pGameInstance->Play_3D_EnvSound(
					L"obj_gmk_com_steam_floor_hatch_01_Stop_Play",
					vPos,
					{ 0.f, 0.f, 0.f },
					0.7f, 2.5f, 14.f);
			}
			}, nullptr);

		spModel->AddListener(L"HeadSound", nullptr, [=]() {
			_float3 vPos = Get_CurrentPos();
			if (!IsOutofRange())
			{
				m_pGameInstance->Play_3D_EnvSound(
					L"obj_gmk_com_krsc_gene_head_rattle_01_Play",
					vPos,
					{ 0.f, 0.f, 0.f },
					0.7f, 2.5f, 14.f);
			}
			}, nullptr);

		spModel->AddListener(L"BuzzerSound", nullptr, [=]() {
			_float3 vPos = Get_CurrentPos();
			if (!IsOutofRange())
			{
				m_pGameInstance->Play_3D_EnvSound(
					L"obj_gmk_com_krsc_gene_buzzer_01_Play",
					vPos,
					{ 0.f, 0.f, 0.f },
					0.7f, 1.f, 25.f);
			}
			}, nullptr);

		spModel->AddListener(L"ChallengeFail_Sound", nullptr, [=]() {
			_float3 vPos = Get_CurrentPos();
			if (!IsOutofRange())
			{
				m_pGameInstance->Play_3D_EnvSound(
					L"obj_gmk_com_krsc_gene_beat_break_01_Play",
					vPos,
					{ 0.f, 0.f, 0.f },
					0.7f, 2.5f, 14.f);
			}
			}, nullptr);
		//sp_ie_com_power_start_space_01_Play
	}
}

_float3 CInteract_Korsica_PowerPlant::Get_CurrentPos()
{
	_float3 vPos{};
	
	auto spTransform = m_wpMainTransformCom.lock();
	if (!spTransform) return vPos;

	XMStoreFloat3(&vPos, spTransform->Get_WorldState(Engine::STATE::POSITION));

	return vPos;
}

_bool CInteract_Korsica_PowerPlant::IsOutofRange()
{
	const _float4* vCamPos = m_pGameInstance->Get_CamPosition();

	if (!vCamPos)
		return true;

	auto spTransform = m_wpMainTransformCom.lock();

	if (!spTransform)
		return true;

	_vector vPos = spTransform->Get_WorldState(Engine::STATE::POSITION);
	_float fLengthSq = XMVectorGetX(XMVector3LengthSq(XMLoadFloat4(vCamPos) - vPos));

	_float fRangeSq = m_fSoundCullingRange * m_fSoundCullingRange;

	if (fLengthSq >= fRangeSq)
		return true;

	else
		return false;
}

void CInteract_Korsica_PowerPlant::Render_Properties()
{
	__super::Render_Properties();
	ImGui::Text("Pattern");
	for (_uint i = 0; i < 16; i++)
	{
		ImGui::PushID(i);
		_bool bCheck = m_desc.m_iAdditionalType & (1 << i);
		if (ImGui::Checkbox("", &bCheck)) {
			m_desc.m_iAdditionalType ^= (1 << i);
		}
		ImGui::SameLine();
		if (i % 4 == 3) {
			ImGui::Text("|");
			if (i!=15) ImGui::SameLine();
		}
		ImGui::PopID();
	}
}

void CInteract_Korsica_PowerPlant::Free()
{
	__super::Free();
}
