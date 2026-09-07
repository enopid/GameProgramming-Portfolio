#include "Partner_Character_Peppermint.h"
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


#include "InteractObject.h"

#include "UC_Aiming.h"
#include "EffectRoot.h"
#include "Projectile.h"

#include "DebugManager_Api.h"
#include "Enemy.h"
#include "Kale_Arm.h"
#include "EnemyBarrier.h"

CPartner_Character_Peppermint::CPartner_Character_Peppermint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartner(pDevice, pContext)
{
	m_ePartnerType = EPartnerType::PT_PEPPERMINT;
}

CPartner_Character_Peppermint::CPartner_Character_Peppermint(const CPartner_Character_Peppermint& Prototype)
	: CPartner(Prototype)
{
	m_ePartnerType = EPartnerType::PT_PEPPERMINT;
}

HRESULT CPartner_Character_Peppermint::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))
		return S_OK;
}

HRESULT CPartner_Character_Peppermint::Initialize(void* pDesc)
{
	CHKFAIL(__super::Initialize(pDesc))

	//Weapon
	CGameObject::LoadPrototype(
		L"Prototype_Weapon_Peppermint_Shotgun",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pShotGun)
	);
	m_pShotGun->SetLevelObject(); 
	m_pShotGun->SetActive(false);
	CGameObject::LoadPrototype(
		L"Prototype_Weapon_Peppermint_Blaster",
		m_pGameInstance->GetLevelDesc().nextLevelTag,
		reinterpret_cast<CGameObject**>(&m_pBlaster)
	);
	m_pBlaster->SetLevelObject();
	m_pBlaster->SetActive(false);

	m_upMagazine = CMagazine::Create(L"Prototype_Projectile_Peppermint_Bullet", 200);
	return S_OK;
}

HRESULT CPartner_Character_Peppermint::Late_Initialize()
{
	CHKFAIL(__super::Late_Initialize())

	//SetController
	auto spModel = m_wpModel.lock();

	Json::Value tmp;
	wstring ConfigPath;
	ConfigPath = L"../AnimNodes/Partner/0.Peppermint/Animator_Partner_Peppermint_Gimmic.json";
	m_pGameInstance->LoadJson(ConfigPath, tmp);

	SetBBFromAnimJson(tmp, m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)]);

	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)] = CAnimController::Create(tmp, m_wpModel.lock());
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]->SetActive(false);
	
	ConfigPath = L"../AnimNodes/Partner/0.Peppermint/Animator_Partner_Peppermint_Battle.json";
	m_pGameInstance->LoadJson(ConfigPath, tmp);
	SetBBFromAnimJson(tmp, m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)]);
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)] = CAnimController::Create(tmp, m_wpModel.lock());
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]->SetActive(true);
	
	/*
	ConfigPath = L"../AnimNodes/Partner/0.Peppermint/Animator_Partner_Peppermint_Normal.json";
	m_pGameInstance->LoadJson(ConfigPath, tmp);
	SetBBFromAnimJson(tmp, m_ctxDescs[ENUM_TO_UINT(EPartnerState::NORMAL)]);
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::NORMAL)] = CAnimController::Create(tmp, m_wpModel.lock());
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::NORMAL)]->SetActive(true);
	*/

	//state
	m_upPSFSM->AddStateFunc(ENUM_TO_UINT(EPartnerState::NONE), EKEYACTIONSTATE::ENTER,
		[=](auto) { ActiveWeapon(EWEAPON::UNARM); });
	m_upPSFSM->AddStateFunc(ENUM_TO_UINT(EPartnerState::BATTLE), EKEYACTIONSTATE::ENTER,
		[=](auto) { ActiveWeapon(EWEAPON::HANDGUN); });
	m_upPSFSM->AddStateFunc(ENUM_TO_UINT(EPartnerState::GIMMIC), EKEYACTIONSTATE::ENTER,
		[=](auto) { ActiveWeapon(EWEAPON::HANDGUN); });
	m_upPSFSM->AddStateFunc(ENUM_TO_UINT(EPartnerState::NORMAL), EKEYACTIONSTATE::ENTER,
		[=](auto) { ActiveWeapon(EWEAPON::UNARM); });
	
	//gimmic
	m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::LOCKON), EKEYACTIONSTATE::ENTER,
		[=](auto) {m_wpModel.lock()->SetLookUse(true); });
	m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::LOCKON), EKEYACTIONSTATE::EXIT,
		[=](auto) {m_wpModel.lock()->SetLookUse(false); });

	m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::IDLE), EKEYACTIONSTATE::ENTER,
		[=](auto) {m_wpModel.lock()->SetLookUse(true); });
	m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::IDLE), EKEYACTIONSTATE::EXIT,
		[=](auto) {m_wpModel.lock()->SetLookUse(false); });
	m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::IDLE), EKEYACTIONSTATE::STAY,
		[=](auto) {
			Compute_Angle();
			Set_Ratio();
			Set_TurnValue(); });
	m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::LOCKON), EKEYACTIONSTATE::STAY,
		[=](auto) {
			Compute_Angle();
			Set_Ratio();
			Set_TurnValue(); });

	//battle
	m_upPBFSM->AddStateFunc(ENUM_TO_UINT(EPartnerBattleState::NORMAL), EKEYACTIONSTATE::ENTER, [=](auto) {
		_int iTargetIdx = 1;
		if (m_pArmTarget) {
			auto _vEnemyPos		= m_pArmTarget->GetArmCenter();
			auto _vPartnerPos	= Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
			auto _vDist = _vEnemyPos - _vPartnerPos;
			if (XMVectorGetY(_vDist) > XMVectorGetX(XMVector3Length(XMVectorSetY(_vDist, 0.f))))
				iTargetIdx = 2;
			if (-XMVectorGetY(_vDist) > XMVectorGetX(XMVector3Length(XMVectorSetY(_vDist, 0.f))))
				iTargetIdx = 0;
		}
		else if (m_pEnemyTarget) {
			auto _vEnemyPos		= m_pEnemyTarget->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
			auto _vPartnerPos	= Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
			auto _vDist = _vEnemyPos - _vPartnerPos;
			if (XMVectorGetY(_vDist) > XMVectorGetX(XMVector3Length(XMVectorSetY(_vDist, 0.f))))
				iTargetIdx = 2;
			if (-XMVectorGetY(_vDist) > XMVectorGetX(XMVector3Length(XMVectorSetY(_vDist, 0.f))))
				iTargetIdx = 0;
		}
		m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]->SetLayerIdx("TargetDir", iTargetIdx);
	});
	Set_Weapon();


	if (auto spCollidor = m_pBeamEffect->GetEffectCollider(L"Com_Collider_CH1000_Beam", true).lock()) {
		spCollidor->Set_Func(EKEYACTIONSTATE::ENTER, [=](auto, CCollidor* pCollidor) {
			if (pCollidor->GetDesc()->m_iCollidorIdx == CT_ENEMYBARRIER) {
				if (auto spBarrier = dynamic_cast<CEnemyBarrier*>(pCollidor->GetGameObject())) {
					spBarrier->OnDamage(100.f);
				}
			}
			if (pCollidor->GetDesc()->m_iCollidorIdx == CT_ENEMY_HIT) {
				auto _vPos = pCollidor->GetGameObject()->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
				m_pBeamHitEffect->Set_Pos(_vPos);
				m_pBeamHitEffect->Restart();

				CCharacter::FDamageInfo _desc;
				if (auto pEnemy = dynamic_cast<CEnemy*>(pCollidor->GetGameObject()))
				{
					_desc.fDamage = 30.f;
					_desc.pSourceObj = this;
					_desc.eAttackType = CCharacter::EAttackType::NORMAL;
					if (!pEnemy->GetIsInvincible())
					{
						pEnemy->OnDamage(_desc);
					}
				}
			}
		});
	}

	return S_OK;
}

void CPartner_Character_Peppermint::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CPartner_Character_Peppermint::Update(_float fTimeDelta)
{
	m_bIsOnTarget	= m_eTarget != ETARGET::NONE;
	m_bIsOnSuccess	= true;
	m_bIsOnFail		= false;

	if (in(m_iPGState, { ENUM_TO_UINT(EPartnerGimmicState::IDLE), ENUM_TO_UINT(EPartnerGimmicState::LOCKON) })) {
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
		m_bIsReadyEvent = m_pEventTarget->GetInteractType() == IT_MIMOSA_EQUALIZER_BARRIER;
	}
	else {
		m_bIsReadyEvent = false;
	}

}
void CPartner_Character_Peppermint::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CPartner_Character_Peppermint::Add_Listener()
{
	__super::Add_Listener();

	m_wpModel.lock()->AddListener(L"Handgun_L", nullptr, [=]() {
		Shot_Handgun(true, false);
	}, nullptr, false);
 	m_wpModel.lock()->AddListener(L"Handgun_R", nullptr, [=]() {
		Shot_Handgun(false, false);
	}, nullptr, false);


	m_wpModel.lock()->AddListener(L"OnBlaster", nullptr, [=]() {
		ActiveWeapon(EWEAPON::BLASTER);
		m_pBlaster->Change_Animation(L"SK_wp1030|wp1030_atk_230");
	}, nullptr, false);

	m_wpModel.lock()->AddListener(L"OnHandgun", nullptr, [=]() {
		ActiveWeapon(EWEAPON::HANDGUN);
	}, nullptr, false);


	m_wpModel.lock()->AddListener(L"pl_ch1000_atk_pa_cannon_shot_01_Play", nullptr, [=]() {
		m_pBeamEffect->Set_Pos(m_wpMainTransformCom.lock()->Get_WorldState(STATE::POSITION));
		m_pBeamEffect->Set_Dir(m_wpMainTransformCom.lock()->Get_WorldState(STATE::LOOK));
		m_pBlaster->Change_Animation(L"SK_wp1030|wp1030_atk_232");
		m_pBeamEffect->Restart();
	}, nullptr, false);
}

CGameObject* CPartner_Character_Peppermint::Clone(void* pArg)
{
	return CloneBase<CPartner_Character_Peppermint>(pArg);
}

void CPartner_Character_Peppermint::Free()
{
	__super::Free();
}

HRESULT CPartner_Character_Peppermint::Reset()
{
	return S_OK;
}

HRESULT CPartner_Character_Peppermint::Save(void* _pDesc, _uint& _iSize) const
{
	_iSize = 0;
	return S_OK;
}

HRESULT CPartner_Character_Peppermint::Load(void* _pDesc)
{
	return S_OK;
}

HRESULT CPartner_Character_Peppermint::Add_InitComponents()
{
	CHKFAIL(__super::Add_InitComponents())

	CModel::MODEL_DESC _modelDesc;
	lstrcpy(_modelDesc.m_sModelAssetName, L"Partner_Peppermint_Export");
	_modelDesc.m_bIsCopy = false;
	Add_Component<CModel>(L"Com_Model_Main", &_modelDesc);

	return S_OK;
}

void CPartner_Character_Peppermint::CheckOnTarget()
{
	auto ray = m_pGameInstance->GetViewportRay();
	_int iCollidorMask = 0;
	iCollidorMask |= (1 << ECollidorType::CT_INTERACTIVE);
	_float fMaxDist = 200.f;
	list<pair<_float, WPCollidor>> _lstCollidors;
	if (m_pGameInstance->RayCast_Collidor(iCollidorMask, ray, fMaxDist, _lstCollidors)) {
		auto [_fDist, _pCollidor] = *_lstCollidors.begin();
		
		if (auto _pInteractTarget = dynamic_cast<CInteractObject*>(_pCollidor.lock()->GetGameObject())) {
			if (_pInteractTarget->GetInteractType() == IT_PEPPERMINT_SWITCH) {
				if (_pInteractTarget->GetState() == CInteractObject::ESTATE::ACTIVE) {
					m_pPartnerTarget = _pInteractTarget;
					if (in(_pInteractTarget->GetInteractType(), { IT_PEPPERMINT_SWITCH })) {
						m_eTarget = ETARGET::SWITCH;
					}
				}
				else {
					m_pPartnerTarget = nullptr;
					m_eTarget = ETARGET::NONE;
				}
			}
			else if (_pInteractTarget->GetInteractType() == IT_CHAI_MAGNET) {
				if (_pInteractTarget->GetState() == CInteractObject::ESTATE::DEACTIVE) {
					m_pPartnerTarget = _pInteractTarget;
					m_eTarget = ETARGET::SHIELD;
				}
				else {
					m_pPartnerTarget = nullptr;
					m_eTarget = ETARGET::NONE;
				}
			}
		}
	}
	else {
		m_pPartnerTarget	= nullptr;
		m_eTarget			= ETARGET::NONE;		
	}
}

HRESULT CPartner_Character_Peppermint::Load_Config()
{
	return S_OK;
}

void CPartner_Character_Peppermint::Update_PB(_float fTimeDelta)
{
	__super::Update_PB(fTimeDelta);
}

void CPartner_Character_Peppermint::Update_PN(_float fTimeDelta)
{
	__super::Update_PN(fTimeDelta);
}

void CPartner_Character_Peppermint::Update_PG_CTX(_float fTimeDelta)
{
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsOnGimmicStart",		m_bGimmicStart &&m_bIsKeyPressed);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("TurnSide",			m_iTurnSide);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("TurnSide",			m_iTurnSide);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsFar",					m_bIsFar);

	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsOnShot",			!m_bIsKeyPressed);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsShieldTarget",		m_eTarget == ETARGET::SHIELD);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)].pBlackboard->SetElement("IsSwitchTarget",		m_eTarget == ETARGET::SWITCH);
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]->UpdateCTX(m_ctxDescs[ENUM_TO_UINT(EPartnerState::GIMMIC)]);
}

void CPartner_Character_Peppermint::Update_PB_CTX(_float fTimeDelta)
{
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONHOLD",		m_bIsKeyPressed);
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONNORMAL",		m_iPBState == ENUM_TO_UINT(EPartnerBattleState::NORMAL));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONEVENT",		m_iPBState == ENUM_TO_UINT(EPartnerBattleState::EVENT));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONPARRY",		m_iPBState == ENUM_TO_UINT(EPartnerBattleState::PARRY));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONSPECIAL",		m_iPBState == ENUM_TO_UINT(EPartnerBattleState::SPECIAL));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONGROUNDJAM",	m_iPBState == ENUM_TO_UINT(EPartnerBattleState::GROUNDJAM));
	m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)].pBlackboard->SetElement("ISONAIRJAM",		m_iPBState == ENUM_TO_UINT(EPartnerBattleState::AIRJAM));
	m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]->UpdateCTX(m_ctxDescs[ENUM_TO_UINT(EPartnerState::BATTLE)]);
}

void CPartner_Character_Peppermint::Update_PN_CTX(_float fTimeDelta)
{
}

void CPartner_Character_Peppermint::Shot_Handgun(bool bIsLeft, bool bIsGimmic)
{
	auto pBullet = m_upMagazine->GetBullet();
	if (pBullet) {
		pBullet->Shot(Get_HandgunRay(bIsLeft));
	}
}

RAY CPartner_Character_Peppermint::Get_HandgunRay(bool bIsLeft)
{
	RAY _ray = {};
	CBone* pBone = nullptr;
	if (bIsLeft) {
		pBone = m_wpModel.lock()->FindBoneWithName(L"Weapon_attach_Socket_L");
	}
	else {
		pBone = m_wpModel.lock()->FindBoneWithName(L"Weapon_attach_Socket_R");
	}
	if (pBone) {
		auto _desc = pBone->GetDesc();

		_vector vPos		= XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_desc.m_CombinedTransformationMatrix.m[3]));
		_vector vUp =
			XMVector3Normalize(
				-XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_desc.m_CombinedTransformationMatrix.m[2]))
			);
			
		_vector vForward	=
			XMVector3Normalize(
				XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_desc.m_CombinedTransformationMatrix.m[0]))
			);
		vPos += vForward * 0.2f + vUp * 0.08f;
		XMStoreFloat4(&_ray.vRayOrigin, XMVectorSetW(vPos, 1.f));


		if (m_iPSState == ENUM_TO_UINT(EPartnerState::GIMMIC)) {
			if (m_pPartnerTarget) {

				if (auto spComp = ConvertWPComponent<CCollidor>(m_pPartnerTarget->Get_Component<CCollidor>()).lock()) {

					XMStoreFloat4(&_ray.vRayDir, XMVector3Normalize(
						spComp->Get_Center() -
						vPos
					));
				}
				else {
					XMStoreFloat4(&_ray.vRayDir, XMVector3Normalize(
						m_pPartnerTarget->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION) -
						vPos
					));
				}
			}
		}
		else if (m_iPSState == ENUM_TO_UINT(EPartnerState::BATTLE)) {
			if (m_pEventTarget) {
				XMStoreFloat4(&_ray.vRayDir, XMVector3Normalize(
					XMVectorSetY(m_pEventTarget->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION) - vPos, 0.f)
				));
			}
			else if (m_pArmTarget) {
				XMStoreFloat4(&_ray.vRayDir, XMVector3Normalize(
					m_pArmTarget->GetArmCenter() - vPos
				));
			}
			else if (m_pEnemyTarget) {

				XMStoreFloat4(&_ray.vRayDir, XMVector3Normalize(
					m_pEnemyTarget->GetEnemyCenter() - vPos
				));
			}
			else {
				XMStoreFloat4(&_ray.vRayDir, XMVector3Normalize(Get_MainTransform().lock()->Get_WorldState(STATE::LOOK)));
			}
			
		}
		

		//memcpy(&_ray.vRayDir,		_desc.m_CombinedTransformationMatrix.m[0], sizeof _float4);
	}
	return _ray;
}

void CPartner_Character_Peppermint::Compute_Angle()
{
	
	auto _vRay		= m_pGameInstance->GetViewportRay().vRayDir;
	auto _vCamDir	= XMVector3Normalize(XMLoadFloat4(&_vRay));
	auto _vCamXZDir = XMVector3Normalize(XMVectorSetY(_vCamDir, 0));
	auto _vChrDir	= XMVector3Normalize(XMVectorSetY(m_wpMainTransformCom.lock()->Get_LocalState(STATE::LOOK), 0));
	
	float dot = XMVectorGetX(XMVector3Dot(_vChrDir, _vCamXZDir));

	XMVECTOR cross = XMVector3Cross(_vChrDir, _vCamXZDir);
	float crossY = XMVectorGetY(cross);

	m_fYaw = atan2f(crossY, dot);
	m_fPitch = asinf(XMVectorGetY(_vCamDir));
}

void CPartner_Character_Peppermint::Set_Ratio()
{
	_float fYaw = wrap(m_fYaw, -XM_PI, +XM_PI);
	_float2 fRatio;
	fRatio.x = clamp(fYaw, -XM_PI * 0.25f, +XM_PI * 0.25f);
	fRatio.x /= XM_PI * 0.25f;
	fRatio.y = clamp(m_fPitch, -XM_PI * 0.25f, +XM_PI * 0.25f);
	fRatio.y /= XM_PI * 0.25f;
	fRatio.y *= -1.f;
	m_wpModel.lock()->SetLookRatio(fRatio);
}

void CPartner_Character_Peppermint::Set_TurnValue()
{
	_float fYaw = wrap(m_fYaw, -XM_PI, +XM_PI);
	if		(fYaw < XM_PI * -0.25f)	m_iTurnSide = -1;
	else if (fYaw < XM_PI * +0.25f)	m_iTurnSide = +0;
	else							m_iTurnSide = +1;
}

void CPartner_Character_Peppermint::Set_Weapon()
{
	auto _pBone = m_wpModel.lock()->FindBoneWithName(L"Weapon_attach_Socket_L");
	m_pShotGun->Set_Bone(_pBone);
	m_pBlaster->Set_Bone(_pBone);
	m_pBlaster->Set_Animator(L"../AnimNodes/Weapon/Peppermint/Animator_Partner_Peppermint_Blaster.json");


}

void CPartner_Character_Peppermint::ActiveWeapon(EWEAPON eWeaponTag)
{
	switch (eWeaponTag)
	{
	case Client::CPartner_Character_Peppermint::UNARM:
		m_pBlaster->SetActive(false);
		break;
	case Client::CPartner_Character_Peppermint::HANDGUN:
		m_pBlaster->SetActive(false);
		break;
	case Client::CPartner_Character_Peppermint::BLASTER:
		m_pBlaster->SetActive(true);
		break;
	default:
		break;
	}
}

_bool CPartner_Character_Peppermint::OnDamage(FDamageInfo _fDamageInfo)
{
	return _bool();
}

HRESULT CPartner_Character_Peppermint::InitFX()
{
	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH1000_Appearance", 
			m_pGameInstance->GetLevelDesc().nextLevelTag, 
			reinterpret_cast<CGameObject**>(&m_pEffects[VFX_APEEARANCE]))
	)
	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH1000_Appearance_Attack", 
			m_pGameInstance->GetLevelDesc().nextLevelTag, 
			reinterpret_cast<CGameObject**>(&m_pEffects[VFX_BATTLEAPEEARANCE]))
	)
	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH1000_Attack_Hold", 
			m_pGameInstance->GetLevelDesc().nextLevelTag, 
			reinterpret_cast<CGameObject**>(&m_pBeamEffect))
	)
	CHKFAIL(
		CGameObject::LoadPrototype(
			L"VFX_CH1000_HoldHit", 
			m_pGameInstance->GetLevelDesc().nextLevelTag, 
			reinterpret_cast<CGameObject**>(&m_pBeamHitEffect))
	)

	for (size_t i = 0; i < VFX_END; i++)
	{
		if (m_pEffects[i]) {
			m_pEffects[i]->Stop();
		}
	}
	m_pBeamEffect->Stop();
	m_pBeamHitEffect->Stop();

	return S_OK;
}
