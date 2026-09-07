#include "Partner.h"

#include "FSM_PartnerState.h"
#include "FSM_PartnerGimmic.h"
#include "FSM_PartnerBattle.h"

#include "GameInstance.h"
#include "AnimController.h"
#include "EffectRoot.h"

#include "Player.h"
#include "Player_Cam.h"
#include "InteractObject.h"

#include "UC_Aiming.h"
#include "UC_Interact.h"
#include "UC_PlayerStatus.h"
#include "Enemy.h"
#include "Kale_Arm.h"

CPartner::CPartner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCharacter(pDevice, pContext)
{
}

CPartner::CPartner(const CPartner& Prototype)
    : CCharacter(Prototype)
{
}

HRESULT CPartner::Initialize_Prototype(void* pArg)
{
    __super::Initialize_Prototype(pArg);
    return S_OK;
}

HRESULT CPartner::Initialize(void* pDesc)
{
    __super::Initialize(pDesc);
    
    //SetFSM
    m_upPSFSM = CPartnerState_FSM   ::Create(ENUM_TO_UINT(EPartnerState::       NORMAL));
    m_upPGFSM = CPartnerGimmic_FSM  ::Create(ENUM_TO_UINT(EPartnerGimmicState:: END));
    m_upPBFSM = CPartnerBattle_FSM  ::Create(ENUM_TO_UINT(EPartnerBattleState:: END));

    //State
    m_upPSFSM->AddStateFunc(ENUM_TO_UINT(EPartnerState::GIMMIC), EKEYACTIONSTATE::ENTER, [=](auto) {
        m_bGimmicStart = true;
        if (m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]) m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]->SetActive(true);
    });
    m_upPSFSM->AddStateFunc(ENUM_TO_UINT(EPartnerState::GIMMIC), EKEYACTIONSTATE::EXIT, [=](auto) {
        m_bGimmicStart = false;
        if (m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]) m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]->SetActive(false);
    });
    m_upPSFSM->AddStateFunc(ENUM_TO_UINT(EPartnerState::BATTLE), EKEYACTIONSTATE::ENTER, [=](auto) {
        m_bGimmicStart = true;
        if (m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]) m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]->SetActive(true);
    });
    m_upPSFSM->AddStateFunc(ENUM_TO_UINT(EPartnerState::BATTLE), EKEYACTIONSTATE::EXIT, [=](auto) {
        m_bGimmicStart = false;
        m_upCooldownTimer->Restart();
        if (m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]) m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]->SetActive(false);
    });
    m_upPSFSM->AddStateFunc(ENUM_TO_UINT(EPartnerState::NONE), EKEYACTIONSTATE::EXIT, [=](auto) {
        m_bIsIsDone = false;
    });
    //Gimmic
    m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::NONE), EKEYACTIONSTATE::EXIT, [=](auto) {
        SetPartnerPos(_float3(-1.f, 0.f, -0.5f));
    });
    m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::ENTER), EKEYACTIONSTATE::ENTER, [=](auto) {
        m_pPartnerTarget    = nullptr;
        m_bIsOnTarget       = false;
        m_bIsEnterEnd       = false; 
        m_bIsExitEnd        = false; 
        m_bIsMoveEnd        = false;
        m_bIsTaskEnd        = false;
        if(m_pEffects[VFX_APEEARANCE]) {
            RestartVFX(VFX_APEEARANCE, Get_MainTransform().lock()->Get_LocalState(STATE::POSITION));
            Play_CallSFX();
        }
    });
    m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::LOCKON), EKEYACTIONSTATE::ENTER, [=](auto) {
        
    });
    m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::EXIT), EKEYACTIONSTATE::EXIT, [=](auto) {
        m_bIsIsDone = true;
    });


    m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::NONE), EKEYACTIONSTATE::ENTER, [=](auto) {
        m_pPartnerTarget = nullptr;
        m_bIsOnTarget = false;
        m_bIsEnterEnd = false;
        m_bIsExitEnd = false;
        m_bIsMoveEnd = false;
        m_bIsTaskEnd = false;
    });
    m_upPGFSM->AddStateFunc(ENUM_TO_UINT(EPartnerGimmicState::NONE), EKEYACTIONSTATE::EXIT, [=](auto) {
        m_pPartnerTarget = nullptr;
        m_bIsOnTarget = false;
        m_bIsEnterEnd = false;
        m_bIsExitEnd = false;
        m_bIsMoveEnd = false;
        m_bIsTaskEnd = false;
        });

    //Battle
    m_upPBFSM->AddStateFunc(ENUM_TO_UINT(EPartnerBattleState::NONE), EKEYACTIONSTATE::EXIT, [=](auto) {
        SetPartnerPos(_float3(-1.f, 0.f, -0.5f));
    });
    m_upPBFSM->AddStateFunc(ENUM_TO_UINT(EPartnerBattleState::NORMAL), EKEYACTIONSTATE::ENTER, [=](auto) {
        if (m_pEffects[VFX_BATTLEAPEEARANCE]) {
            RestartVFX(VFX_BATTLEAPEEARANCE, Get_MainTransform().lock()->Get_LocalState(STATE::POSITION));
            Play_CallSFX();
        }

    });
    m_upPBFSM->AddStateFunc(ENUM_TO_UINT(EPartnerBattleState::NONE), EKEYACTIONSTATE::ENTER, [=](auto) {
        m_bIsTaskEnd        = false;
        m_bIsReadyGroundJam = false;
        m_bIsReadyAirJam    = false;
        m_bIsReadyParryAtk  = false;
        m_bIsReadySpecialAtk= false;
    });
    m_upPBFSM->AddStateFunc(ENUM_TO_UINT(EPartnerBattleState::NONE), EKEYACTIONSTATE::EXIT, [=](auto) {
        m_bIsTaskEnd = false;
    });
    m_upPBFSM->AddStateFunc(ENUM_TO_UINT(EPartnerBattleState::NONE), EKEYACTIONSTATE::ENTER, [=](auto) {
        m_bIsIsDone = true;
    });
    //FX
    for (size_t i = 0; i < VFX::VFX_END; i++) m_pEffects[i] = nullptr;
    CHKFAIL(InitFX());

    //Cooldown
    m_upCooldownTimer    = m_pGameInstance->CreateTimerWithDuration(COOLDOWNDURATION, false, nullptr, false);
    m_upTurnbackTimer    = m_pGameInstance->CreateTimerWithDuration(TURNBACKDURATION, false, nullptr, false);
    m_upPartnerCallTimer = m_pGameInstance->CreateTimerWithDuration(1.f, false, [=]()
        {
            Play_AppearSFX();
        }, false);
    return S_OK;
}

void CPartner::SetPartnerPos(_float3 vOffset)
{
    if (m_pPlayer) {
        auto _vPos     = m_pPlayer->Get_MainTransform().lock()->Get_LocalState(STATE::POSITION);
        auto _vLookDir = m_pPlayer->Get_MainTransform().lock()->Get_LocalState(STATE::LOOK);
        auto _vOffset               = XMVectorSetW(XMLoadFloat3(&vOffset), 0.f);

        RAY vRay;
        vRay.vRayDir = _float4(0.f, -1.f, 0.f, 0.f);
        float fDist = 10.f;
        _int iCollidorMask = 0;
        iCollidorMask |= (1 << ECollidorType::CT_COMMON_BODY);


        Get_MainTransform().lock()->Set_LocalState(STATE::POSITION, _vPos);
        Get_MainTransform().lock()->Aim(_vLookDir);
        Get_MainTransform().lock()->Translate(vOffset.x, vOffset.y, vOffset.z);
        
        XMStoreFloat4(&vRay.vRayOrigin, Get_MainTransform().lock()->Get_WorldState(STATE::POSITION));

        list<pair<_float, WPCollidor>> lstCollidors;
        if (m_pGameInstance->RayCast_Collidor(iCollidorMask, vRay, fDist, lstCollidors) && fDist>=-0.1f) {
            Get_MainTransform().lock()->Translate(0.f, fDist * -1.f, 0.f, false);
            return;
        }

        Get_MainTransform().lock()->Set_LocalState(STATE::POSITION, _vPos);
        Get_MainTransform().lock()->Aim(_vLookDir);
        Get_MainTransform().lock()->Translate(vOffset.x * -1.f, vOffset.y, vOffset.z);

        XMStoreFloat4(&vRay.vRayOrigin, Get_MainTransform().lock()->Get_WorldState(STATE::POSITION));
        if (m_pGameInstance->RayCast_Collidor(iCollidorMask, vRay, fDist, lstCollidors) && fDist >= -0.1f) {
            Get_MainTransform().lock()->Translate(0.f, fDist * -1.f, 0.f, false);
        }
    }
}

HRESULT CPartner::Late_Initialize()
{
    __super::Late_Initialize();

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Object(m_pGameInstance->GetLevelDesc().nextLevelTag, L"Layer_Player", L"Player"));
    m_pPlayerCam = static_cast<CPlayer_Cam*>(m_pGameInstance->Get_Object(m_pGameInstance->GetLevelDesc().nextLevelTag, L"Layer_Camera", L"Player_Camera"));
    Add_Listener();

    InitUI();

    return S_OK;
}

void CPartner::InitUI()
{
    if (auto _pUC = m_pGameInstance->GetUC(L"UC_Aiming")) {
        m_pUC_Aiming = dynamic_cast<CUC_Aiming*>(_pUC);
    }
    if (auto _pUC = m_pGameInstance->GetUC(L"UC_Interact")) {
        m_pUC_Interact = dynamic_cast<CUC_Interact*>(_pUC);
    }
}

void CPartner::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    m_bIsKeyPressed = m_pGameInstance->IsKeyState(CALLKEY, EKEYACTIONSTATE::STAY);
    m_bIsFar = false;
    if (m_pPlayer) {
        auto _vPlayerPos    = m_pPlayer->Get_MainTransform().lock()->Get_LocalState(STATE::POSITION);
        auto _vPartnerPos   = Get_MainTransform().lock()->Get_LocalState(STATE::POSITION);
        _float fDist = XMVectorGetX(XMVector3Length(_vPlayerPos - _vPartnerPos));
        m_bIsFar = fDist > 500.f;
    }
}

void CPartner::Update(_float fTimeDelta)
{
    if (m_pArmTarget && m_pArmTarget->IsActive()) {
        auto iArmType = m_pArmTarget->GetArmType();
        m_bIsKaleArm = false;
        if (iArmType==0 && m_ePartnerType == PT_PEPPERMINT)
            m_bIsKaleArm = true;
        if (iArmType==1 && m_ePartnerType == PT_KORSICA)
            m_bIsKaleArm = true;
        if (iArmType==2 && m_ePartnerType == PT_MACARON)
            m_bIsKaleArm = true;
    }
    else {
        m_bIsKaleArm = false;
    }

    SetUIState();
    Turnback(fTimeDelta);

    m_bIsOnBattle = m_pEnemyTarget != nullptr;
    //if (m_bSelected && m_pGameInstance->IsKeyState(DIK_K, EKEYACTIONSTATE::ENTER)) m_bIsOnBattle ^= true;
    __super::Update(fTimeDelta);
    Update_PS(fTimeDelta);
    m_upPSFSM->Update(fTimeDelta);
    
    auto _prevPSState = m_iPSState;
    m_iPSState = m_upPSFSM->GetState();
    if (true && m_iPSState != _prevPSState) {
        PrintDebug("PartnerState Transition : ", PARTNERSTATENAME[_prevPSState], " to ", PARTNERSTATENAME[m_iPSState]);
    }

    if (m_iPSState == 0) {

    }

    if (m_iPSState == 1) {
        Update_PB(fTimeDelta);
        m_upPBFSM->Update(fTimeDelta);
        Update_PB_CTX(fTimeDelta);
        if (m_upPBFSM) {
            auto _prevPBState = m_iPBState;
            m_iPBState = m_upPBFSM->GetState();
            if (true && m_iPBState != _prevPBState) {
                PrintDebug("PartnerBattleState Transition : ", PARTNERBATTLENAME[_prevPBState], " to ", PARTNERBATTLENAME[m_iPBState]);
            }
        }
        if (m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]) m_upAnimControllers[ENUM_TO_UINT(EPartnerState::BATTLE)]->Update(fTimeDelta);
    }

    if (m_iPSState == 2) {
        Update_PG(fTimeDelta);
        m_upPGFSM->Update(fTimeDelta);
        Update_PG_CTX(fTimeDelta);
        if (m_upPGFSM) {
            auto _prevPGState = m_iPGState;
            m_iPGState = m_upPGFSM->GetState();
            if (true && m_iPGState != _prevPGState) {
                PrintDebug("PartnerGimmicState Transition : ", PARTNERGIMMICNAME[_prevPGState], " to ", PARTNERGIMMICNAME[m_iPGState]);
            }
        }
        m_upAnimControllers[ENUM_TO_UINT(EPartnerState::GIMMIC)]->Update(fTimeDelta);
    }

    if (m_iPSState == 3) {
        Update_PN(fTimeDelta);
        //m_upPNFSM->Update(fTimeDelta);
        Update_PN_CTX(fTimeDelta);
        //if (m_upPNFSM) m_iPNState = m_upPNFSM->GetState();
        //m_upAnimControllers[ENUM_TO_UINT(EPartnerState::NORMAL)]->Update(fTimeDelta);
    }
    SetTarget();
    SetEventTarget();
    SetArmTarget();
    m_bIsOnAvailable = !m_upCooldownTimer->IsActive();
}

void CPartner::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

void CPartner::Add_Listener()
{
    __super::Add_Listener();
    //SetListner
    auto _spModel = m_wpModel.lock();
    _spModel->AddListener(L"IsEnterEnd", nullptr, [=]() { m_bIsEnterEnd = true; }, nullptr);
    _spModel->AddListener(L"IsReturnEnd", nullptr, [=]() { m_bIsExitEnd = true; }, nullptr);
    _spModel->AddListener(L"IsTaskEnd", nullptr, [=]() { 
        m_bIsTaskEnd = true; 
        }, nullptr);
    _spModel->AddListener(L"IsMoveEnd", nullptr, [=]() { m_bIsMoveEnd = true; }, nullptr);

    _spModel->AddListener(L"TurnBack", 
       nullptr,
        [=]() { 
            _vector _vLook = Get_MainTransform().lock()->Get_LocalState(STATE::LOOK);
            _vLook *= -1.f;
            XMStoreFloat4(&m_vLookDir, _vLook);

            m_upTurnbackTimer->Restart();
        }, 
       nullptr
    );


    _spModel->AddListener(L"ChaseEnemyTarget",
        [=](float fDeltaTime) {
            if (m_pArmTarget) {
                Get_MainTransform().lock()->LookAtDamp(m_pArmTarget->GetArmCenter(), fDeltaTime, 30.f, true);
            }
            else if (m_pEventTarget) {
                Get_MainTransform().lock()->LookAtDamp(
                    m_pEventTarget->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION), fDeltaTime, 30.f, true);
            }
            else if (m_pEnemyTarget) {
                Get_MainTransform().lock()->LookAtDamp(m_pEnemyTarget->GetEnemyCenter(), fDeltaTime, 30.f, true);
            }
        },
        nullptr,
        nullptr
    );
}

void CPartner::SetSelected(bool bSelected)
{
    m_bSelected = bSelected;
}

void CPartner::Free()
{
    __super::Free();
    for (size_t i = 0; i < 3; i++)
    {
        Safe_Release(m_ctxDescs[i].pBlackboard);
    }
}

HRESULT CPartner::Save(void* _pDesc, _uint& _iSize) const
{
    _iSize = 0;
    return S_OK;
}

HRESULT CPartner::Load(void* _pDesc)
{
    return S_OK;
}

HRESULT CPartner::Add_InitComponents()
{
    CHKFAIL(__super::Add_InitComponents())
    return S_OK;
}

void CPartner::SetTarget(CEnemy* pEnemy)
{
    m_pEnemyTarget = pEnemy;
}

void CPartner::SetCooldown(_bool bIsCooldown)
{
    m_bIsCooldown = bIsCooldown;
}

void CPartner::SetGroundJam()
{
    m_bIsReadyGroundJam = true;
}

void CPartner::SetAirJam()
{
    m_bIsReadyAirJam = true;
}

void CPartner::SetParryAtk()
{
    m_bIsReadyParryAtk = true;
}

void CPartner::SetSpecialAtk()
{
    m_bIsReadySpecialAtk = true;
}

void CPartner::Play_AppearSFX()
{
    switch (m_ePartnerType)
    {
    case Client::PT_PEPPERMINT:
    {
        _int   INum = Random(1, 7);
        wstring strSfxTag = L"av_pep_co_response_01_Play" + to_wstring(INum);
        m_pGameInstance->Play_SFX(strSfxTag, 1.f);
        break;
    }

    case Client::PT_MACARON:
    {
        _int   INum = Random(1, 8);
        wstring strSfxTag =
            L"av_mac_co_response_01_Play" +
            to_wstring(INum);
        m_pGameInstance->Play_SFX(strSfxTag, 1.f);
        break;
    }


    case Client::PT_KORSICA:
    {
        _int   INum = Random(1, 9);
        wstring strSfxTag =
            L"av_kor_co_response_01_Play" +
            to_wstring(INum);

        m_pGameInstance->Play_SFX(strSfxTag, 1.f);
        break;
    }
    }
}

void CPartner::Play_CallSFX()
{
    switch (m_ePartnerType)
    {
    case Client::PT_PEPPERMINT: 
    {
        _int   INum = Random(1, 7);
        wstring strSfxTag = L"av_cha_co_gen_pep_01_Play" + to_wstring(INum);
        m_pGameInstance->Play_SFX(strSfxTag, 1.f);
        break;
    }
      
    case Client::PT_MACARON: 
    {
        _int   INum = Random(1, 7);
        wstring strSfxTag =
            L"av_cha_co_gen_mac_01_Play" +
            to_wstring(INum);
        m_pGameInstance->Play_SFX(strSfxTag, 1.f);
        break;
    }

     
    case Client::PT_KORSICA: 
    {
        _int   INum = Random(1, 7);
        wstring strSfxTag =
            L"av_cha_co_gen_kor_01_Play" +
            to_wstring(INum);

        m_pGameInstance->Play_SFX(strSfxTag, 1.f);
        break;
    }  
    }

    m_upPartnerCallTimer->Restart();
}

void CPartner::Update_PS(_float fTimeDelta)
{
    auto _pDesc = m_upPSFSM->Get_Context();
	_pDesc->m_bIsOnAvailable		= !m_bIsOnLock;
    _pDesc->m_bIsOnCombat           = true;
    _pDesc->m_bIsOnBattle           = m_bIsOnBattle || m_bIsReadyEvent || m_bIsKaleArm;
    _pDesc->m_bIsOnNormal           = false;
    _pDesc->m_bIsDone               = m_bIsIsDone;
    _pDesc->m_bSelected             = m_bSelected;
    
    _pDesc->m_bPartnerKeyPressed    = m_bIsKeyPressed;
    _pDesc->m_bIsOnCoolDown         = m_bIsCooldown;
}



void CPartner::Update_PG(_float fTimeDelta)
{
	auto _pDesc = m_upPGFSM->Get_Context();
	_pDesc->m_bIsOnAvailable		= m_bIsOnAvailable && !m_bIsOnLock;
	_pDesc->m_bPartnerKeyPressed	= m_bIsKeyPressed;
	_pDesc->m_bIsFar	            = m_bIsFar;

	_pDesc->m_bIsOnTarget			= m_bIsOnTarget;
	_pDesc->m_bIsFail				= m_bIsOnFail;
	_pDesc->m_bIsSuccess			= m_bIsOnSuccess;
	_pDesc->m_bEnterEnd				= m_bIsEnterEnd;
	_pDesc->m_bExitEnd				= m_bIsExitEnd;
	_pDesc->m_bTaskEnd				= m_bIsTaskEnd;
}

void CPartner::Update_PB(_float fTimeDelta)
{
	auto _pDesc = m_upPBFSM->Get_Context();
	_pDesc->m_bIsOnAvailable		= m_bIsOnAvailable && !m_bIsOnLock;
	_pDesc->m_bPartnerKeyPressed	= m_bIsKeyPressed;
	_pDesc->m_bTaskEnd				= m_bIsTaskEnd;

	_pDesc->m_bIsOnEvent            = m_bIsReadyEvent;
	_pDesc->m_bIsOnSpecial          = m_bIsReadySpecialAtk;
	_pDesc->m_bIsOnGroundJam        = m_bIsReadyGroundJam;
	_pDesc->m_bIsOnAirJam           = m_bIsReadyAirJam;
	_pDesc->m_bIsOnParry            = m_bIsReadyParryAtk;
}
void CPartner::Update_PN(_float fTimeDelta)
{
}

void CPartner::SetUIState()
{
    if (m_pUC_Aiming && m_bSelected) {
        m_pUC_Aiming->SetPartner(m_ePartnerType);
        m_pUC_Interact->Set_iGimmicState(m_iPGState);
        if (in(m_iPGState, { ENUM_TO_UINT(EPartnerGimmicState::IDLE), ENUM_TO_UINT(EPartnerGimmicState::ENTER) })) {
            m_pUC_Aiming->SetOnAim(true);
            m_pUC_Interact->SetCurPartner(m_ePartnerType);
            m_pUC_Interact->SetActiveReady(m_ePartnerType);
        }
        else {
            m_pUC_Aiming->SetOnAim(false);
        }
    }
}

void CPartner::SetTarget()
{
    if (m_pEnemyTarget && m_pEnemyTarget->IsActive()) return;
    m_pEnemyTarget = nullptr;
    if (m_pPlayer) {
        auto _lstEnemy = m_pPlayer->Get_EnemyList();
        _lstEnemy.remove_if([=](auto pEnemy) {return !pEnemy->IsActive(); });
        _lstEnemy.sort([=](CEnemy* pEnemy0, CEnemy* pEnemy1) {
            auto _vPos0         = pEnemy0->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
            auto _vPos1         = pEnemy1->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
            auto _vPartnerPos   = Get_MainTransform().lock()->Get_WorldState(STATE::POSITION);
            if (m_ePartnerType == PT_PEPPERMINT) {
                if (pEnemy0->CheckBarrier() && !pEnemy1->CheckBarrier()) return true;
                if (!pEnemy0->CheckBarrier() && pEnemy1->CheckBarrier()) return false;
            }
            return XMVectorGetX(XMVector3Length(_vPos0 - _vPartnerPos)) < XMVectorGetX(XMVector3Length(_vPos1 - _vPartnerPos));
        });
        if (_lstEnemy.size())
            m_pEnemyTarget = _lstEnemy.front();
    }
}

void CPartner::SetEventTarget()
{
    auto _lstInteracts = m_pGameInstance->Get_Objects(
        m_pGameInstance->GetLevelDesc().nextLevelTag,
        L"Layer_Interact"
    );

    auto _fDist = 1000;
    m_pEventTarget = nullptr;
    for (auto pObject : _lstInteracts)
    {
        auto _vViewportPos = m_pGameInstance->ConvertViewportPos(
            pObject->Get_MainTransform().lock()->Get_WorldState(STATE::POSITION)
        );
        if (auto pInteractObj = dynamic_cast<CInteractObject*>(pObject)) {
            if (!pInteractObj->IsActive()) continue;
            auto eType = pInteractObj->GetInteractType();

            _bool   bAvailable = false;
            if (eType == IT_MIMOSA_EQUALIZER_BARRIER    && m_ePartnerType == EPartnerType::PT_PEPPERMINT) bAvailable = true;
            if (eType == IT_MIMOSA_EQUALIZER_SHIELD     && m_ePartnerType == EPartnerType::PT_MACARON   ) bAvailable = true;
            if (!bAvailable) continue;

            _float _fTmpDist = XMVectorGetX(XMVector3Length(XMVectorSetZ(_vViewportPos, 0.f)));
            if (XMVectorGetZ(_vViewportPos) > 0.f) {
                if (_fDist > _fTmpDist) {
                    _fDist = (Engine::_int)_fTmpDist;
                    m_pEventTarget = pInteractObj;
                }
            }
        }
    }
}

void CPartner::SetArmTarget()
{
    auto _lstWeapons = m_pGameInstance->Get_Objects(
        m_pGameInstance->GetLevelDesc().nextLevelTag,
        L"Layer_Weapon"
    );

    m_pArmTarget = nullptr;
    for (auto pObject : _lstWeapons)
    {
        if (auto pArm = dynamic_cast<CKale_Arm*>(pObject)) {
            if (pArm->OnTarget())
                m_pArmTarget = pArm;
        }
    }
}

void CPartner::Turnback(float fDeltaTime)
{
    if (m_upTurnbackTimer && m_upTurnbackTimer->IsActive()) {
        Get_MainTransform().lock()->AimDamp(XMLoadFloat4(&m_vLookDir), fDeltaTime, 33.0, true);
        m_wpModel.lock()->SetCurrentDir();
    }

}

float CPartner::GetCooldownRatio()
{
    if (m_upCooldownTimer->IsActive()) {
        return (Engine::_float)m_upCooldownTimer->GetElapsedRatio();
    }
    else {
        return 1.0f;
    }
}

void CPartner::RestartVFX(VFX eID, _vector pos)
{
    if (m_pEffects[eID] == nullptr)
        return;

    VFX_SetPos(eID, pos);
    m_pEffects[eID]->Restart();
}

void CPartner::VFX_SetPos(VFX eID, _vector pos)
{
    if (m_pEffects[eID] == nullptr)
        return;

    m_pEffects[eID]->Set_Pos(pos);
}

void CPartner::VFX_SetBone(VFX eID, CBone* pBone)
{
    if (m_pEffects[eID] == nullptr)
        return;

    m_pEffects[eID]->Set_Bone(pBone);
}
