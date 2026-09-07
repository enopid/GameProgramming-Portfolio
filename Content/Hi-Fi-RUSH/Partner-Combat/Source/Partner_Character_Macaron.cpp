#include "Partner_Character_Macaron.h"
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
#include "Interact_Mimosa_Object.h"
#include "EnemyShield.h"
#include "Interact_RhythmChallenge.h"

//UI
#include "UC_RhythmChallenge_Macaron.h"
#include "UC_Aiming.h"

#include "EffectRoot.h"
#include "Enemy.h"
#include "Kale_SArm.h"
#include "Interact_RoboArm.h"

#include "GravitySphere.h"

CPartner_Character_Macaron::CPartner_Character_Macaron(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartner(pDevice, pContext)
{
	m_ePartnerType = EPartnerType::PT_MACARON;
}

CPartner_Character_Macaron::CPartner_Character_Macaron(const CPartner_Character_Macaron& Prototype)
	: CPartner(Prototype)
{
	m_ePartnerType = EPartnerType::PT_MACARON;
}

HRESULT CPartner_Character_Macaron::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
		return S_OK;
}

HRESULT CPartner_Character_Macaron::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	CGameObject::LoadPrototype(
		L"Prototype_Gravity_Sphere_Inner",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pSphereInner)
	);
	CGameObject::LoadPrototype(
		L"Prototype_Gravity_Sphere_Outer",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pSphereOuter)
	);
	
	return S_OK;
}

HRESULT CPartner_Character_Macaron::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize())

	//SetController
	auto spModel = m_wpModel.lock();

	Json::Value tmp;
	wstring ConfigPath;
	ConfigPath = L"../AnimNodes/Partner/1.Macaron/Animator_Partner_Macaron_Gimmic.json";
	m_pGameInstance->LoadJson(ConfigPath, tmp);

	SetBBFromAnimJson(tmp, m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)]);

	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)] = CAnimController::Create(tmp, m_wpModel.lock());
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]->SetActive(true);

	ConfigPath = L"../AnimNodes/Partner/1.Macaron/Animator_Partner_Macaron_Battle.json";
	m_pGameInstance->LoadJson(ConfigPath, tmp);
	SetBBFromAnimJson(tmp, m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)]);
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)] = CAnimController::Create(tmp, m_wpModel.lock());
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]->SetActive(true);

	m_pCameraBone	= m_wpModel.lock()->FindBoneWithName(L"prop_02");
	m_pGravity_Bone = m_wpModel.lock()->FindBoneWithName(L"prop_00");

	m_wpModel.lock()->AddListener(
		L"pl_ch2000_atk_pa_punch_imp_01_Play",
		nullptr,
		[=]() {
			CEffectRoot* _pEffect = nullptr;
			_pEffect = m_pMacaronEffects[VFX_MACARON_ATK_NORMAL1];
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
		L"LightAttack1",
		nullptr,
		[=]() {
			CEffectRoot* _pEffect = nullptr;
			_pEffect = m_pMacaronEffects[VFX_MACARON_ATK_NORMAL0];
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
		[=]() {if (m_pPlayerCam) m_pPlayerCam->SetTargetBone(m_pCameraBone, 0.4f); },
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

	m_wpModel.lock()->AddListener(
		L"Trigger",
		nullptr,
		[=]() {if (m_pPartnerTarget) {
			m_pPartnerTarget->Trigger();
			CEffectRoot* _pEffect = nullptr;

			if (m_pPartnerTarget->GetInteractType() == IT_MACARON_WALL)
				_pEffect = m_pMacaronEffects[VFX_MACARON_HIT_BOOM];
			else
				_pEffect = m_pMacaronEffects[VFX_MACARON_HIT_NORMAL];

			if (_pEffect && m_pParticle_Bone) {
				auto _desc = m_pParticle_Bone->GetDesc();
				_pEffect->Set_Pos(
					XMLoadFloat4(reinterpret_cast<_float4*>(&_desc.m_CombinedTransformationMatrix.m[3]))
				);
				_pEffect->Restart();
			}
		}},
		nullptr,
		false
	);
	m_wpModel.lock()->AddListener(
		L"MimosaEventTrigger",
		nullptr,
		[=]() {if (m_pEventTarget) {
		static_cast<CInteract_Mimosa_Object*>(m_pEventTarget)->OnShieldDamage(200.f);
		}},
		nullptr,
		false
	);
	m_wpModel.lock()->AddListener(
		L"pl_ch2000_act_swish_norm_01_Play",
		nullptr,
		[=]() {if (m_pEventTarget) {
		CEffectRoot* _pEffect = m_pMacaronEffects[VFX_MACARON_HIT_NORMAL];
		auto vPos = m_pEventTarget->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
		_pEffect->Set_Pos(
			vPos
		);
		_pEffect->Restart();
		}},
		nullptr,
		false
	);
	m_wpModel.lock()->AddListener(
		L"pl_ch2000_atk_pa_Gravity_Start_01_Play",
		nullptr,
		[=]() {
			auto _desc = m_pGravity_Bone->GetDesc();
			_vector vPos = XMLoadFloat4(
				reinterpret_cast<_float4*>(&_desc.m_CombinedTransformationMatrix.m[3])
			) + XMVectorSet(0.f, -2.5f, 0.f, 0.f);

			auto pBone0 = m_wpModel.lock()->FindBoneWithName(L"l_hand");
			auto pBone1 = m_wpModel.lock()->FindBoneWithName(L"r_hand");

			auto _desc0 = pBone0->GetDesc();   
			auto _desc1 = pBone1->GetDesc();

			_vector vPos0 = XMLoadFloat4(
				reinterpret_cast<_float4*>(&_desc0.m_CombinedTransformationMatrix.m[3])
			);
			_vector vPos1 = XMLoadFloat4(
				reinterpret_cast<_float4*>(&_desc1.m_CombinedTransformationMatrix.m[3])
			);
			auto _vDir = (vPos0 + vPos1) * 0.5f - vPos;
			m_pSphereInner->Appear(vPos, XMVectorSet(0.f, 0.f, 1.f, 0.f), false);
			m_pSphereOuter->Appear(vPos, _vDir, true);

			m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetBoolElement("ONGRAVITY", true);
		},
		nullptr,
		false
	);

	m_wpModel.lock()->AddListener(
		L"pl_ch2000_atk_pa_Gravity_end_01_Play",
		nullptr,
		[=]() {
			m_pSphereInner->Disappear();
			m_pSphereOuter->Disappear();
		},
		nullptr,
		false
	);

	auto fBeatOffset = 0.25f;
	m_upPGFSM->AddStateFunc(
		ENUM_TO_UINT(EPartnerGimmicState::SHOT),
		EKEYACTIONSTATE::ENTER,
		[=](auto) {
			auto fBeat = m_pGameInstance->GetElapsedBeat();
			if (
				(2.f - fBeatOffset <= fBeat && fBeat <= 2.f + fBeatOffset) ||
				(4.f - fBeatOffset <= fBeat || fBeat <= 0.f + fBeatOffset)
			) {
				m_bIsOnSuccess	= true;
				m_bIsOnFail		= false;
			}
			else {
				m_bIsOnSuccess	= false;
				m_bIsOnFail		= true;
			}
		}
	);


	m_upPBFSM->AddStateFunc(
		ENUM_TO_UINT(EPartnerBattleState::EVENT),
		EKEYACTIONSTATE::ENTER,
		[=](auto) {
			if (m_pEventTarget) {
				auto _vTargetPos	= m_pEventTarget->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
				auto _vTargetLook	= 
					XMVector3Normalize(XMVectorSetY(
					m_pEventTarget->Get_MainTransform().lock()->Get_WorldState(STATE::LOOK)
						, 0.f));
				auto _vCurPos		= Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);

				Get_MainTransform().lock()->Aim(-_vTargetLook);
				Get_MainTransform().lock()->Set_LocalState(STATE::POSITION,
					XMVectorSetY(_vTargetPos, XMVectorGetY(_vCurPos))
					+ _vTargetLook * 2.5f
				);
			}
		}
	);
	m_upPBFSM->AddStateFunc(
		ENUM_TO_UINT(EPartnerBattleState::NORMAL),
		EKEYACTIONSTATE::ENTER,
		[=](auto) {
			m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetBoolElement("ONGRAVITY", false);
		}
	);

	
	if (auto spCollidor = m_pMacaronEffects[MACARONVFX::VFX_MACARON_ATK_NORMAL0]->GetEffectCollider(L"Com_Collider_CH2000_Atk02").lock()) {
		spCollidor->Set_Func(
			EKEYACTIONSTATE::ENTER,
			[=](CONTACTMF_DESC _mfDesc, CCollidor* pCollidor) {

				FDamageInfo _desc;
				if (auto pEnemy = dynamic_cast<CEnemy*>(pCollidor->GetGameObject()))
				{
					_desc.fDamage		= 10.f;
					_desc.eAttackType	= CCharacter::EAttackType::NORMAL;
					if (!pEnemy->GetIsInvincible())
					{
						pEnemy->OnDamage(_desc);
					}
				}
				if (auto pKaleArm = dynamic_cast<CKale_Arm*>(pCollidor->GetGameObject()))
				{
					if (pKaleArm->GetArmState() == CInteract_RoboArm::EARMSTATE::AS_GIMMIC_SUCCESS) {
						pKaleArm->OnDamage(10.f);
					}
					else if (
						pKaleArm->GetArmState() == CInteract_RoboArm::EARMSTATE::AS_GIMMIC_STAY &&
						pKaleArm->GetArmType() == CInteract_RoboArm::EARMTYPESTATE::AT_SARM) {
						static_cast<CKale_SArm*>(pKaleArm)->ShiledBreak();
					}
				}
				if (auto pShield = dynamic_cast<CEnemyShield*>(pCollidor->GetGameObject()))
				{
					pShield->Damage();
				}
				//auto pKaleArm = dynamic_cast<CKale_Arm*>(pCollidor->GetGameObject());
			}
		);
	}
	if (auto spCollidor = m_pMacaronEffects[MACARONVFX::VFX_MACARON_ATK_NORMAL1]->GetEffectCollider(L"Com_Collider_CH2000_Atk01").lock()) {
		spCollidor->Set_Func(
			EKEYACTIONSTATE::ENTER,
			[=](CONTACTMF_DESC _mfDesc, CCollidor* pCollidor) {
				auto pEnemy = dynamic_cast<CEnemy*>(pCollidor->GetGameObject());

				FDamageInfo _desc;
				if (auto pEnemy = dynamic_cast<CEnemy*>(pCollidor->GetGameObject()))
				{
					_desc.fDamage = 10.f;
					_desc.eAttackType = CCharacter::EAttackType::NORMAL;
					if (!pEnemy->GetIsInvincible())
					{
						pEnemy->OnDamage(_desc);
					}
				}
				if (auto pKaleArm = dynamic_cast<CKale_Arm*>(pCollidor->GetGameObject()))
				{
					if (pKaleArm->GetArmState() == CInteract_RoboArm::EARMSTATE::AS_GIMMIC_SUCCESS) {
						pKaleArm->OnDamage(10.f);
					}
					else if (
						pKaleArm->GetArmState() == CInteract_RoboArm::EARMSTATE::AS_GIMMIC_STAY &&
						pKaleArm->GetArmType() == CInteract_RoboArm::EARMTYPESTATE::AT_SARM) {
						static_cast<CKale_SArm*>(pKaleArm)->ShiledBreak();
					}
				}
				if (auto pShield = dynamic_cast<CEnemyShield*>(pCollidor->GetGameObject()))
				{
					pShield->Damage();
				}
				
			}
		);
	}
	

	return S_OK;
}


void CPartner_Character_Macaron::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CPartner_Character_Macaron::Update(_float fTimeDelta)
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
	}

	if (m_pEventTarget && m_pEventTarget->IsActive()) {
		m_bIsReadyEvent = m_pEventTarget->GetInteractType() == IT_MIMOSA_EQUALIZER_SHIELD;
	}
	else {
		m_bIsReadyEvent = false;
	}
}
void CPartner_Character_Macaron::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CPartner_Character_Macaron::Add_Listener()
{
	__super::Add_Listener();
}

CGameObject* CPartner_Character_Macaron::Clone(void* pArg)
{
	return CloneBase<CPartner_Character_Macaron>(pArg);
}

void CPartner_Character_Macaron::Free()
{
	__super::Free();
}

HRESULT CPartner_Character_Macaron::Reset()
{
	return S_OK;
}

HRESULT CPartner_Character_Macaron::Save(void* _pDesc, _uint& _iSize) const
{
	_iSize = 0;
	return S_OK;
}

HRESULT CPartner_Character_Macaron::Load(void* _pDesc)
{
	return S_OK;
}

HRESULT CPartner_Character_Macaron::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents())

	//Model
	CModel::MODEL_DESC _modelDesc;
	lstrcpy(_modelDesc.m_sModelAssetName, L"Partner_Macaron_Export");
	_modelDesc.m_bIsCopy = false;
	Add_Component<CModel>(L"Com_Model_Main", &_modelDesc);


	return S_OK;
}

void CPartner_Character_Macaron::CheckOnTarget()
{
	auto ray = m_pGameInstance->GetViewportRay();
	_int iCollidorMask = 0;
	iCollidorMask |= (1 << ECollidorType::CT_INTERACTIVE);
	_float fMaxDist = 200.f;
	list<pair<_float, WPCollidor>> _lstCollidors;
	if (m_pGameInstance->RayCast_Collidor(iCollidorMask, ray, fMaxDist, _lstCollidors)) {
		auto [_fDist, _pCollidor] = *_lstCollidors.begin();
		
		if (auto _pInteractTarget = dynamic_cast<CInteract_RhythmChallenge*>(_pCollidor.lock()->GetGameObject())) {
			if (in(_pInteractTarget->GetInteractType(), { IT_MACARON_PLATFORM, IT_MACARON_SHIELD, IT_MACARON_WALL })) {
				if (_pInteractTarget->GetState() == CInteractObject::ESTATE::ACTIVE) {
					m_pPartnerTarget = _pInteractTarget;
					_pInteractTarget->Move(m_pPlayer, this);
					m_eTarget = ETARGET::Wall;
					m_bIsOnTarget = true;
				}
			}
		}
	}
}

HRESULT CPartner_Character_Macaron::Load_Config()
{
	return S_OK;
}

void CPartner_Character_Macaron::Update_PB(_float fTimeDelta)
{
	__super::Update_PB(fTimeDelta);
}

void CPartner_Character_Macaron::Update_PN(_float fTimeDelta)
{
	__super::Update_PN(fTimeDelta);
}

void CPartner_Character_Macaron::Update_PG_CTX(_float fTimeDelta)
{
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsOnGimmicStart",		m_bGimmicStart && m_bIsKeyPressed);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsOnTarget",			m_bIsOnTarget);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsFar", m_bIsFar);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsOnSuccess", m_bIsOnSuccess);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsOnFail", m_bIsOnFail);

	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsOnShot",			!m_bIsKeyPressed);
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]->UpdateCTX(m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)]);
}

void CPartner_Character_Macaron::Update_PB_CTX(_float fTimeDelta)
{
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONHOLD", m_bIsKeyPressed);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONNORMAL", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::NORMAL));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONEVENT", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::EVENT));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONPARRY", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::PARRY));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONSPECIAL", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::SPECIAL));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONGROUNDJAM", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::GROUNDJAM));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONAIRJAM", m_iPBState == ENUM_TO_UINT(EPartnerBattleState::AIRJAM));
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]->UpdateCTX(m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)]);
}

void CPartner_Character_Macaron::Update_PN_CTX(_float fTimeDelta)
{
}

void CPartner_Character_Macaron::InitUI()
{
	__super::InitUI();
	if (auto _pUC = m_pGameInstance->GetUC(L"UC_RhythmChallenge_Macaron")) {
		m_pUC_RC_Macaron = dynamic_cast<CUC_RhythmChallenge_Macaron*>(_pUC);
		m_pUC_RC_Macaron->SetActive(false);
		m_upPGFSM->AddStateFunc(
			ENUM_TO_UINT(EPartnerGimmicState::LOCKON),
			EKEYACTIONSTATE::ENTER,
			[=](auto) {m_pUC_RC_Macaron->OnChallenge(); }
		);
		m_upPGFSM->AddStateFunc(
			ENUM_TO_UINT(EPartnerGimmicState::SUCCESS),
			EKEYACTIONSTATE::ENTER,
			[=](auto) {m_pUC_RC_Macaron->Success(); }
		);
		m_upPGFSM->AddStateFunc(
			ENUM_TO_UINT(EPartnerGimmicState::FAIL),
			EKEYACTIONSTATE::ENTER,
			[=](auto) {m_pUC_RC_Macaron->Fail(); }
		);
		m_upPGFSM->AddStateFunc(
			ENUM_TO_UINT(EPartnerGimmicState::EXIT),
			EKEYACTIONSTATE::ENTER,
			[=](auto) {m_pUC_RC_Macaron->SetActive(false); }
		);
	}
	m_pRC_Bone			= m_wpModel.lock()->FindBoneWithName(L"prop_01");
	m_pParticle_Bone	= m_wpModel.lock()->FindBoneWithName(L"r_hand");
}

void CPartner_Character_Macaron::SetUIState()
{
	__super::SetUIState();
	if (m_pRC_Bone) {
		auto _desc = m_pRC_Bone->GetDesc();
		auto _vViewportPos = m_pGameInstance->ConvertViewportPos(
			XMLoadFloat4(reinterpret_cast<const _float4*>(&_desc.m_CombinedTransformationMatrix.m[3]))
		);
		_float2 _vPos;
		_vPos.x = XMVectorGetX(_vViewportPos) * g_iWinSizeX * 0.5f;
		_vPos.y = XMVectorGetY(_vViewportPos) * g_iWinSizeY * 0.5f;
;		if (m_pUC_RC_Macaron) m_pUC_RC_Macaron->SetPos(_vPos);
		
	}
}

_bool CPartner_Character_Macaron::OnDamage(FDamageInfo _fDamageInfo)
{
	return _bool();
}


HRESULT CPartner_Character_Macaron::InitFX()
{
	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH2000_Appearance",
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			reinterpret_cast<CGameObject**>(&m_pEffects[VFX_APEEARANCE]))
	)

	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH2000_Appearance_Attack",
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			reinterpret_cast<CGameObject**>(&m_pEffects[VFX_BATTLEAPEEARANCE]))
	)
	for (size_t i = 0; i < VFX_END; i++)
	{
		if (m_pEffects[i]) {
			m_pEffects[i]->Stop();
		}
	}

	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH2000_Wall_Hit",
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			reinterpret_cast<CGameObject**>(&m_pMacaronEffects[VFX_MACARON_HIT_BOOM]))
	)
	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_Enemy_Hit",
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			reinterpret_cast<CGameObject**>(&m_pMacaronEffects[VFX_MACARON_HIT_NORMAL]))
	)
	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH2000_Attack",
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			reinterpret_cast<CGameObject**>(&m_pMacaronEffects[VFX_MACARON_ATK_NORMAL0]))
	)
	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH2000_LandingSmoke",
			m_pGameInstance->GetLevelDesc().nextLevelTag,
			reinterpret_cast<CGameObject**>(&m_pMacaronEffects[VFX_MACARON_ATK_NORMAL1]))
	)

	for (size_t i = 0; i < VFX_MACARON_END; i++)
	{
		if (m_pMacaronEffects[i]) {
			m_pMacaronEffects[i]->Stop();
		}
	}
	return S_OK;
}