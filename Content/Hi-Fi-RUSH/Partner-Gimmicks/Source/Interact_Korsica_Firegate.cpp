#include "Interact_Korsica_Firegate.h"
#include "BaseModel.h"
#include "AnimController.h"
#include "GameInstance.h"
#include "UC_UIEffect.h"
#include "Player.h"
#include "EffectRoot.h"

CGameObject* CInteract_Korsica_Firegate::Clone(void* pArg)
{
	return CloneBase<CInteract_Korsica_Firegate>(pArg);
}

HRESULT CInteract_Korsica_Firegate::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents());
	return S_OK;
}

CInteract_Korsica_Firegate::CInteract_Korsica_Firegate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteract_RhythmChallenge(pDevice, pContext)
{
	m_eInteractType = IT_KORSICA_FIREGATE;
}

CInteract_Korsica_Firegate::CInteract_Korsica_Firegate(const CInteract_Korsica_Firegate& Prototype)
	: CInteract_RhythmChallenge(Prototype)
{
	m_eInteractType = IT_KORSICA_FIREGATE;
}

HRESULT CInteract_Korsica_Firegate::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
		return S_OK;
}

HRESULT CInteract_Korsica_Firegate::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	////Model
	//CModel::MODEL_DESC _modelDesc;
	//lstrcpy(_modelDesc.m_sModelAssetName, L"GimmicObject_Korsika_FIregate");
	//_modelDesc.m_bIsCopy = false;
	//Add_Component<CModel>(L"Com_Model_Main", &_modelDesc);

	auto wpComp = Get_Component<CModel>();
	m_wpModel = ConvertWPComponent<CModel>(wpComp);

	m_fSoundCullingRange = 25.f;

	CGameObject::LoadPrototype(
		L"VFX_FireGate_Fire",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pEffect)
	);
	m_pEffect->SetLevelObject();
	return S_OK;
}

HRESULT CInteract_Korsica_Firegate::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize());

	Add_Listener();

	if (m_pGameInstance->GetCurrentLevelTag() != L"Level_Editor")
		m_pUCEffect = static_cast<CUC_UIEffect*>(m_pGameInstance->GetUC(L"UC_UIEffect"));

	m_wpCollidor_FireHit = ConvertWPComponent<CCollidor>(
		Get_Component(L"Com_Collidor_FireBodyHit")
	);
	m_wpCollidor_FireBody = ConvertWPComponent<CCollidor>(
		Get_Component(L"Com_Collidor_FireBodyBody")
	);
	m_wpCollidor_FireHit.lock()->Set_Func(
		EKEYACTIONSTATE::ENTER,
		[=](CONTACTMF_DESC _desc, CCollidor* _pCollidor) {
			if (auto pPlayer = static_cast<CPlayer*>(_pCollidor->GetGameObject())) {
				CCharacter::FDamageInfo _desc;
				_desc.eAttackType = CCharacter::EAttackType::FIRE;
				_desc.fDamage = 0.f;
				pPlayer->OnDamage(_desc);
			}
		}
	);

	m_pEffect->Set_Pos(Get_MainTransform().lock()->Get_WorldState(STATE::POSITION));
	m_pEffect->Set_Dir(Get_MainTransform().lock()->Get_WorldState(STATE::LOOK));
	m_pEffect->Restart();

	return S_OK;
}

void CInteract_Korsica_Firegate::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CInteract_Korsica_Firegate::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	m_wpCollidor_FireHit.lock()	->SetActive(m_eCurState != ESTATE::TRIGGER);
	m_wpCollidor_FireBody.lock()->SetActive(m_eCurState != ESTATE::TRIGGER);

	//===============

	if (m_ePrevState == ESTATE::TRIGGER && m_eCurState == ESTATE::DEACTIVE && !m_bRebootComplete)
		m_bIsReboot = true;

	if (m_eCurState == ESTATE::TRIGGER)
		m_bRebootComplete = false;

	if (m_ePrevState == ESTATE::TRIGGER && m_eCurState != ESTATE::TRIGGER) {
		m_pEffect->Set_Pos(Get_MainTransform().lock()->Get_WorldState(STATE::POSITION));
		m_pEffect->Set_Dir(Get_MainTransform().lock()->Get_WorldState(STATE::LOOK));
		m_pEffect->Restart();
	}
	if (m_ePrevState != ESTATE::TRIGGER && m_eCurState == ESTATE::TRIGGER) {
		m_pEffect->DissolveStop();
	}

	auto spTransform = m_wpMainTransformCom.lock();

	if (spTransform && m_bIsReboot)
	{
		_float3 vPos{};
		XMStoreFloat3(&vPos, spTransform->Get_WorldState(Engine::STATE::POSITION));

		if (!IsOutofRange())
		{
			//���� �߰�����
		}

		m_bIsReboot = false;
		m_bRebootComplete = true;
	}

	if (m_pUCEffect != nullptr)
		m_pUCEffect->StartPowerPlantDeActiveTime((Engine::_float)m_upDeactiveTimer->GetElapsedRatio());
}

void CInteract_Korsica_Firegate::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CInteract_Korsica_Firegate::Add_Listener()
{
	auto spModel = m_wpModel.lock();

	if (spModel)
	{
		// ���� �߰�����
	}
}

_float3 CInteract_Korsica_Firegate::Get_CurrentPos()
{
	_float3 vPos{};
	
	auto spTransform = m_wpMainTransformCom.lock();
	if (!spTransform) return vPos;

	XMStoreFloat3(&vPos, spTransform->Get_WorldState(Engine::STATE::POSITION));

	return vPos;
}

_bool CInteract_Korsica_Firegate::IsOutofRange()
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

void CInteract_Korsica_Firegate::Render_Properties()
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

void CInteract_Korsica_Firegate::Free()
{
	__super::Free();
}
