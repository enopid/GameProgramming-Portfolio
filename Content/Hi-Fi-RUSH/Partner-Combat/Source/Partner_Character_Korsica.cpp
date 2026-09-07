#include "Partner_Character_Korsica.h"
#include "BaseModel.h"
#include "BlackBoard.h"
#include "Bone.h"
#include "GameInstance.h"
#include "AnimController.h"
#include "Collidor.h"
#include "MeshRenderer.h"

#include "FSM_PartnerState.h"
#include "FSM_PartnerGimmic.h"
#include "FSM_PartnerBattle.h"

#include "Player.h"
#include "Weapon.h"
#include "Player_Cam.h"


#include "InteractObject.h"
#include "Interact_RhythmChallenge.h"

#include "UC_Aiming.h"
#include "UC_RhythmChallenge_Korsica.h"

#include "EffectRoot.h"
#include "Korsica_Wind.h"
#include "Enemy.h"
#include "Kale_FArm.h"
#include "Interact_RoboArm.h"
#include "Enemy_GNRFLO.h"

CPartner_Character_Korsica::CPartner_Character_Korsica(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartner(pDevice, pContext)
{
	m_ePartnerType = EPartnerType::PT_KORSICA;
}

CPartner_Character_Korsica::CPartner_Character_Korsica(const CPartner_Character_Korsica& Prototype)
	: CPartner(Prototype)
{
	m_ePartnerType = EPartnerType::PT_KORSICA;
}

HRESULT CPartner_Character_Korsica::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
		return S_OK;
}

HRESULT CPartner_Character_Korsica::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	CGameObject::LoadPrototype(
		L"Prototype_Weapon_Korsica",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pWeaponL)
	);
	m_pWeaponL->SetLevelObject();

	CGameObject::LoadPrototype(
		L"Prototype_Weapon_Korsica",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pWeaponR)
	);
	m_pWeaponR->SetLevelObject();
	
	m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::SUCCESS), EKEYACTIONSTATE::ENTER, [=](auto) {
		if (m_pPartnerTarget) {
			dynamic_cast<CInteract_RhythmChallenge*>(m_pPartnerTarget)->Success();
		}
	});
	m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::FAIL), EKEYACTIONSTATE::ENTER, [=](auto) {
		if (m_pPartnerTarget) {
			dynamic_cast<CInteract_RhythmChallenge*>(m_pPartnerTarget)->Fail();
		}
	});
	m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::SHOT), EKEYACTIONSTATE::ENTER, [=](auto) {
		if (m_pPartnerTarget) {
			static_cast<CInteract_RhythmChallenge*>(m_pPartnerTarget)->Challenge();
			static_cast<CInteract_RhythmChallenge*>(m_pPartnerTarget)->Move(m_pPlayer, this);
		}
	});

	auto _lstChildrens =  m_pKorsicaEffects[VFX_KORSICA_ATK_NORMAL]->GetChildrens();

	for (auto pChild : _lstChildrens) {
		if (auto spCollidor = ConvertWPComponent<CCollidor>(pChild->Get_Component<CCollidor>()).lock()) {
			spCollidor->Set_Func(EKEYACTIONSTATE::ENTER, [=](CONTACTMF_DESC _desc, CCollidor* pCollidor) {
				FDamageInfo _dmgdesc;
				if (auto pEnemy = dynamic_cast<CEnemy*>(pCollidor->GetGameObject()))
				{
					FDamageInfo _damgeDesc;
					_damgeDesc.fDamage = 0.f;
					_damgeDesc.eAttackType = CCharacter::EAttackType::KORSICA_WIND;
					_damgeDesc.pSourceObj = this;
					if (!pEnemy->GetIsInvincible())
					{
						pEnemy->OnDamage(_damgeDesc);
						PrintDebug("Korsica Wind Enter!");
					}
					
				}
				if (auto pKaleArm = dynamic_cast<CKale_Arm*>(pCollidor->GetGameObject()))
				{
					if (pKaleArm->GetArmState() == CInteract_RoboArm::EARMSTATE::AS_GIMMIC_SUCCESS) {
						//pKaleArm->OnDamage(0.f);
					}
					else if (
						pKaleArm->GetArmState() == CInteract_RoboArm::EARMSTATE::AS_GIMMIC_STAY &&
						pKaleArm->GetArmType() == CInteract_RoboArm::EARMTYPESTATE::AT_FARM) {
						static_cast<CKale_FArm*>(pKaleArm)->Extinguish();
					}
				}
				}
			);
		}
	}

	return S_OK;
}

HRESULT CPartner_Character_Korsica::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize())
		
	//SetController
	auto spModel = m_wpModel.lock();

	Json::Value tmp;
	wstring ConfigPath;
	ConfigPath = L"../AnimNodes/Partner/2.Korsica/Animator_Partner_Korsica_Gimmic.json";
	m_pGameInstance->LoadJson(ConfigPath, tmp);
	SetBBFromAnimJson(tmp, m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)]);
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)] = CAnimController::Create(tmp, m_wpModel.lock());
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]->SetActive(true);

	ConfigPath = L"../AnimNodes/Partner/2.Korsica/Animator_Partner_Korsica_Battle.json";
	m_pGameInstance->LoadJson(ConfigPath, tmp);
	SetBBFromAnimJson(tmp, m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)]);
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)] = CAnimController::Create(tmp, m_wpModel.lock());
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]->SetActive(true);

	m_pTornadoBone[0]	= m_wpModel.lock()->FindBoneWithName(L"prop_00");
	m_pTornadoBone[1]	= m_wpModel.lock()->FindBoneWithName(L"prop_01");
	m_pSpinBone			= m_wpModel.lock()->FindBoneWithName(L"spine_00");
	m_pCameraBone		= m_wpModel.lock()->FindBoneWithName(L"prop_02");

	if (m_pKorsicaEffects[VFX_KORSICA_GIMMIC_LOOP0]) m_pKorsicaEffects[VFX_KORSICA_GIMMIC_LOOP0]->Set_Bone(m_pTornadoBone[0]);
	if (m_pKorsicaEffects[VFX_KORSICA_GIMMIC_LOOP1]) m_pKorsicaEffects[VFX_KORSICA_GIMMIC_LOOP1]->Set_Bone(m_pTornadoBone[0]);
	m_wpModel.lock()->AddListener(
		L"HOLD_ATTACK",
		nullptr,
		[=]() {
			m_pHoldWind->Shot(
				Get_MainTransform().lock()->Get_WorldState(STATE::POSITION),
				Get_MainTransform().lock()->Get_WorldState(STATE::LOOK)
			);
		},
		nullptr,
		false
	);
	m_wpModel.lock()->AddListener(
		L"VFX_ATTACK_NORMAL",
		nullptr,
		[=]() {
			CEffectRoot* _pEffect = m_pKorsicaEffects[VFX_KORSICA_ATK_NORMAL];
			auto _desc = m_pSpinBone->GetDesc();
			_pEffect->Set_Pos(
				XMLoadFloat4(reinterpret_cast<_float4*>(_desc.m_CombinedTransformationMatrix.m[3]))
			);
			_pEffect->Set_Dir(
				Get_MainTransform().lock()->Get_WorldState(STATE::LOOK)
			);
			_pEffect->Restart();
		},
		nullptr,
		false
	);
	m_wpModel.lock()->AddListener(
		L"VFX_TURN",
		nullptr,
		[=]() {
			CEffectRoot* _pEffect = m_pKorsicaEffects[VFX_KORSICA_TURN];
			auto _desc = m_pSpinBone->GetDesc();
			_pEffect->Set_Pos(
				XMLoadFloat4(reinterpret_cast<_float4*>(_desc.m_CombinedTransformationMatrix.m[3]))
			);
			_pEffect->Set_Dir(
				Get_MainTransform().lock()->Get_WorldState(STATE::LOOK)
			);
			_pEffect->Restart();
		},
		nullptr,
		false
	);
	m_wpModel.lock()->AddListener(
		L"VFX_SUCCESS",
		nullptr,
		[=]() {
			CEffectRoot* _pEffect = m_pKorsicaEffects[VFX_KORSICA_GIMMIC_SUCCESS];
			_pEffect->Set_Pos(
				Get_MainTransform().lock()->Get_WorldState(STATE::POSITION)
			);
			_pEffect->Set_Dir(
				Get_MainTransform().lock()->Get_WorldState(STATE::LOOK)
			);
			_pEffect->Restart();
		},
		nullptr,
		false
	);
	m_wpModel.lock()->AddListener(
		L"IsCamAttachSrt",
		nullptr,
		[=]() {if (m_pPlayerCam) m_pPlayerCam->SetTargetBone(m_pCameraBone, 0.4f, XM_PI *2.f / 3.0f); },
		nullptr,
		false
	);
	m_wpModel.lock()->AddListener(
		L"IsCamAttachEnd",
		nullptr,
		[=]() {if (m_pPlayerCam) m_pPlayerCam->SetTargetBone(nullptr,0.5f); },
		nullptr,
		false
	);

	Set_Weapon();
	 

	return S_OK;
}

void CPartner_Character_Korsica::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CPartner_Character_Korsica::Update(_float fTimeDelta)
{
	if (in(m_iPGState, { ENUM_TO_UINT(EPartnerGimmicState::IDLE) })) {
		CheckOnTarget();
	}


	__super::Update(fTimeDelta);
	if (m_pGameInstance->GetLevelDesc().nextLevelTag != L"Level_Editor") {
		for (size_t i = 0; i < m_wpModel.lock()->Get_NumMeshes(); i++)
		{
			m_wpModel.lock()->GetMeshRenderer((Engine::_uint)i)->SetActive(m_iPSState != 0);
		}
		m_pWeaponL->SetActive(m_iPSState != 0);
		m_pWeaponR->SetActive(m_iPSState != 0);
	}

}
void CPartner_Character_Korsica::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CPartner_Character_Korsica::Add_Listener()
{
	static bool bFlag = false;
	__super::Add_Listener();
	m_wpModel.lock()->AddListener(
		L"Test",
		nullptr,
		[=]() {
			//PrintDebug("Test FX");
			bFlag ^= true;
			if ( bFlag && m_pKorsicaEffects[VFX_KORSICA_GIMMIC_LOOP0]) m_pKorsicaEffects[VFX_KORSICA_GIMMIC_LOOP0]->Restart();
			if (!bFlag && m_pKorsicaEffects[VFX_KORSICA_GIMMIC_LOOP1]) m_pKorsicaEffects[VFX_KORSICA_GIMMIC_LOOP1]->Restart();
		},
		nullptr,
		false
	);
}

CGameObject* CPartner_Character_Korsica::Clone(void* pArg)
{
	return CloneBase<CPartner_Character_Korsica>(pArg);
}

void CPartner_Character_Korsica::Free()
{
	/*m_pWeaponL->SetDestroyFlag();
	m_pWeaponR->SetDestroyFlag();*/
	__super::Free();
}

HRESULT CPartner_Character_Korsica::Reset()
{
	return S_OK;
}

HRESULT CPartner_Character_Korsica::Save(void* _pDesc, _uint& _iSize) const
{
	_iSize = 0;
	return S_OK;
}

HRESULT CPartner_Character_Korsica::Load(void* _pDesc)
{
	return S_OK;
}

HRESULT CPartner_Character_Korsica::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents())

	//Model
	CModel::MODEL_DESC _modelDesc;
	lstrcpy(_modelDesc.m_sModelAssetName, L"Partner_Korsica_Export");
	_modelDesc.m_bIsCopy = false;
	Add_Component<CModel>(L"Com_Model_Main", &_modelDesc);

	return S_OK;
}

void CPartner_Character_Korsica::CheckOnTarget()
{
	auto ray = m_pGameInstance->GetViewportRay();
	_int iCollidorMask = 0;
	iCollidorMask |= (1 << ECollidorType::CT_INTERACTIVE);
	_float fMaxDist = 200.f;
	list<pair<_float, WPCollidor>> _lstCollidors;
	if (m_pGameInstance->RayCast_Collidor(iCollidorMask, ray, fMaxDist, _lstCollidors)) {
		auto [_fDist, _pCollidor] = *_lstCollidors.begin();
		
		if (auto _pInteractTarget = dynamic_cast<CInteract_RhythmChallenge*>(_pCollidor.lock()->GetGameObject())) {
			if (in(_pInteractTarget->GetInteractType(), { IT_KORSICA_FIREGATE, IT_KORSICA_GENERATOR })) {
				if (_pInteractTarget->GetState() == CInteractObject::ESTATE::DEACTIVE) {
					m_pPartnerTarget = _pInteractTarget;
					_pInteractTarget->Activate();


					if (_pInteractTarget->GetInteractType()			== IT_KORSICA_FIREGATE)
						m_eTarget = ETARGET::FIRE;
					else if (_pInteractTarget->GetInteractType()	== IT_KORSICA_GENERATOR)
						m_eTarget = ETARGET::GENERATOR;
					
					m_bIsOnTarget = true;
				}
			}
		}
	}
}

HRESULT CPartner_Character_Korsica::Load_Config()
{
	return S_OK;
}

void CPartner_Character_Korsica::Update_PB(_float fTimeDelta)
{
	__super::Update_PB(fTimeDelta);
}

void CPartner_Character_Korsica::Update_PN(_float fTimeDelta)
{
	__super::Update_PN(fTimeDelta);
}

void CPartner_Character_Korsica::Update_PG_CTX(_float fTimeDelta)
{
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsOnGimmicStart",		m_bGimmicStart && m_bIsKeyPressed);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsOnTarget",			m_bIsOnTarget);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsFar",				m_bIsFar);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("RhythmResult",		m_iRhythmResult);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsOnShot",			!m_bIsKeyPressed);

	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]->UpdateCTX(m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)]);
}

void CPartner_Character_Korsica::Update_PB_CTX(_float fTimeDelta)
{
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONHOLD", m_bIsKeyPressed);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONNORMAL", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::NORMAL));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONPARRY", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::PARRY));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONSPECIAL", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::SPECIAL));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONGROUNDJAM", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::GROUNDJAM));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONAIRJAM", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::AIRJAM));
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]->UpdateCTX(m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)]);
}

void CPartner_Character_Korsica::Update_PN_CTX(_float fTimeDelta)
{
}

void CPartner_Character_Korsica::Set_Weapon()
{
	auto _pBoneL = m_wpModel.lock()->FindBoneWithName(L"l_attach_hand_00");
	auto _pBoneR = m_wpModel.lock()->FindBoneWithName(L"r_attach_hand_00");
	m_pWeaponL->Set_Bone(_pBoneL);
	m_pWeaponR->Set_Bone(_pBoneR);
	//m_pWeaponL->Set_Animator(L"../AnimNodes/Weapon/Peppermint/Animator_Partner_Peppermint_Blaster.json");
	//m_pWeaponR->Set_Animator(L"../AnimNodes/Weapon/Peppermint/Animator_Partner_Peppermint_Blaster.json");
}

void CPartner_Character_Korsica::InitUI()
{
	__super::InitUI();
	if (auto _pUC = m_pGameInstance->GetUC(L"UC_RhythmChallenge_Korsica")) {
		m_pUC_RC_Korsica = dynamic_cast<CUC_RhythmChallenge_Korsica*>(_pUC);
		m_pUC_RC_Korsica->SetActive(false);
		m_upPGFSM->AddStateFunc(
			ENUM_TO_UINT(EPartnerGimmicState::SHOT),
			EKEYACTIONSTATE::ENTER,
			[=](auto) {m_pUC_RC_Korsica->OnChallenge(
				m_pPartnerTarget->GetAdditionalType()
			); }
		);
		m_upPGFSM->AddStateFunc(
			ENUM_TO_UINT(EPartnerGimmicState::SUCCESS),
			EKEYACTIONSTATE::ENTER,
			[=](auto) {m_pUC_RC_Korsica->Success(); }
		);
	}
}

void CPartner_Character_Korsica::SetUIState()
{
	__super::SetUIState();

	if (m_iPGState == ENUM_TO_UINT(EPartnerGimmicState::SHOT) && m_pUC_RC_Korsica && m_pUC_RC_Korsica->CheckOnChallenge()) {
		if (m_pGameInstance->IsKeyState(CALLKEY, EKEYACTIONSTATE::ENTER)) {

			m_pGameInstance->Play_SFX(L"KorsicaGimmick_Inst_FX_Play", 1.f);

			if (m_pUC_RC_Korsica->CheckHit()) {
				m_pUC_RC_Korsica->Hit();
			}
			else {
				m_pUC_RC_Korsica->Fail();
			}
		}
		else if (m_pUC_RC_Korsica->CheckMiss()) {
			m_pUC_RC_Korsica->Fail();
		}
		if (m_pUC_RC_Korsica->CheckSuccess()) {
			m_pUC_RC_Korsica->Success();
		}
		m_iRhythmResult = m_pUC_RC_Korsica->GetHitNoteNum();
	}
	else {
		m_iRhythmResult = 0;
	}
	m_bIsOnSuccess	= m_iRhythmResult == +4;
	m_bIsOnFail		= m_iRhythmResult == -1;

}

_bool CPartner_Character_Korsica::OnDamage(FDamageInfo _fDamageInfo)
{
	return _bool();
}

HRESULT CPartner_Character_Korsica::InitFX()
{
	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH4000_Appearance",
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			reinterpret_cast<CGameObject**>(&m_pEffects[VFX_APEEARANCE]))
	)
	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH4000_Appearance_Attack",
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			reinterpret_cast<CGameObject**>(&m_pEffects[VFX_BATTLEAPEEARANCE]))
	)
	for (size_t i = 0; i < VFX_END; i++)
	{
		if (m_pEffects[i]) {
			m_pEffects[i]->Stop();
		}
	}

	CGameObject::LoadPrototype(
		L"Prototype_Korsica_Wind",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pHoldWind));

	CGameObject::LoadPrototype(
		L"VFX_CH4000_Attack",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pKorsicaEffects[VFX_KORSICA_ATK_NORMAL]));

	CGameObject::LoadPrototype(
		L"VFX_CH4000_Attack_Hold",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pKorsicaEffects[VFX_KORSICA_ATK_HOLD]));

	CGameObject::LoadPrototype(
		L"VFX_CH4000_Attack_Turn",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pKorsicaEffects[VFX_KORSICA_TURN]));

	CGameObject::LoadPrototype(
		L"VFX_CH4000_Gimmic",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pKorsicaEffects[VFX_KORSICA_GIMMIC_SUCCESS]));

	CGameObject::LoadPrototype(
		L"VFX_CH4000_Gimmic_Loop",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pKorsicaEffects[VFX_KORSICA_GIMMIC_LOOP0]));

	CGameObject::LoadPrototype(
		L"VFX_CH4000_Gimmic_Loop",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pKorsicaEffects[VFX_KORSICA_GIMMIC_LOOP1]));

	
	for (size_t i = 0; i < VFX_KORSICA_END; i++)
	{
		if (m_pKorsicaEffects[i]) {
			m_pKorsicaEffects[i]->Stop();
		}
	}

	return S_OK;
}
