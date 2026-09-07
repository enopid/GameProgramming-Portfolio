#include "Monster_Stick1.h"
#include "GameInstance.h"

#include "AnimController.h"
#include "BehaviourTree.h"
#include "BlackBoard.h"

#include "BT_Node.h"
#include "BT_Task.h"
#include "BT_Decorator.h"
#include "BT_Selector.h"
#include "BT_RandomSelector.h"
#include "BT_Sequence.h"

#include "Collidor.h"
#include "Player.h"

CGameObject* CMonster_Stick1::Clone(void* pArg)
{
    return CloneBase<CMonster_Stick1>(pArg);
}

CBT_Node* CMonster_Stick1::CreateNode_Combat()
{
    CBT_Node::BT_NODE_DESC _selDesc;
    _selDesc.m_sNodeTag = L"Selector\n(Combat)";
    auto pSelector_Atk = CBT_Selector::Create(&_selDesc);
    auto meleeRange = CreateDecorator_TargetInRange(0.f, m_fMidRange);
    auto midRange   = CreateDecorator_TargetInRange(m_fMidRange, m_fLongRange);
    auto longRange  = CreateDecorator_TargetInRange(m_fLongRange, m_fDeAggroRange);
    pSelector_Atk->AddChildNode(meleeRange);
    pSelector_Atk->AddChildNode(midRange);
    pSelector_Atk->AddChildNode(longRange);

    meleeRange->AddChildNode(CreateNode_MeleeCombat());
    midRange->AddChildNode(CreateNode_MidCombat());
    longRange->AddChildNode(CreateNode_LongCombat());
    return pSelector_Atk;
}

CBT_Node* CMonster_Stick1::CreateNode_Hit()
{
    CBT_Node::BT_NODE_DESC _seqDesc;
    _seqDesc.m_sNodeTag = L"Sequence\n(Hit)";
    auto pSequence_HitSeq = CBT_Sequence::Create(&_seqDesc);

    auto _pCondTask     = CreateTask_Condition([=]() {return m_bIsHit; });

    auto _pTask_HitAnim0 = CreateTask_PlayAnimation(10);
    auto _pTask_HitAnim1 = CreateTask_PlayAnimation(11);
    auto _pTask_HitAnim2 = CreateTask_PlayAnimation(12);

    _pTask_HitAnim0->AddExitFunc([=](CBT_Node* _pNode)  {m_bIsHit           = false; });
    _pTask_HitAnim1->AddExitFunc([=](CBT_Node* _pNode)  {m_bIsHit           = false; });
    _pTask_HitAnim2->AddExitFunc([=](CBT_Node* _pNode)  {m_bIsHit           = false; });

    CBT_Node::BT_NODE_DESC _selDesc;
    _selDesc.m_sNodeTag = L"Selector\n(HitType)";
    auto pSelector_Hit = CBT_Selector::Create(&_selDesc);
    auto SmallDMG   = CreateDecorator_Condition([=]() {return m_fLastDamage <= 30.f; });
    auto MidDMG     = CreateDecorator_Condition([=]() {return m_fLastDamage > 30.f && m_fLastDamage <= 50.f; });
    auto BigDMG     = CreateDecorator_Condition([=]() {return m_fLastDamage > 50.f; });
    pSelector_Hit->AddChildNode(SmallDMG);
    pSelector_Hit->AddChildNode(MidDMG);
    pSelector_Hit->AddChildNode(BigDMG);

    SmallDMG->AddChildNode(_pTask_HitAnim0);
    MidDMG->AddChildNode(_pTask_HitAnim1);
    BigDMG->AddChildNode(_pTask_HitAnim2);



    pSequence_HitSeq->AddChildNode(_pCondTask);
    pSequence_HitSeq->AddChildNode(pSelector_Hit);

    return pSequence_HitSeq;
}


CBT_Node* CMonster_Stick1::CreateNode_MeleeCombat()
{
    CBT_Node::BT_NODE_DESC _seqDesc;
    _seqDesc.m_sNodeTag = L"Sequence\n(Melee)";
    auto pSequence_AtkSeq = CBT_Sequence::Create(&_seqDesc);
    
    auto pDecorator_ForceTrue   = CreateDecorator_ForceResult(true);
    auto pTask_MoveTo           = CreateTask_MoveTo(12.f, m_fMidRange, m_fWalkSpeed);
    pDecorator_ForceTrue->AddChildNode(pTask_MoveTo);

    CBT_Node::BT_NODE_DESC _randoSelDesc;
    _randoSelDesc.m_sNodeTag = L"Random Selector\n(Melee)";
    auto pRandomSelector_AtkSel     = CBT_RandomSelector::Create(&_randoSelDesc);
    auto pTask_AnimPlay_Atk1        = CreateTask_PlayAnimation(MA_ATK_1);
    pTask_AnimPlay_Atk1->AddEnterFunc([=](auto) { m_iDamageType = 0; });
    auto pTask_AnimPlay_Atk2        = CreateTask_PlayAnimation(MA_ATK_2);
    pTask_AnimPlay_Atk2->AddEnterFunc([=](auto) { m_iDamageType = 1; });
    auto pTask_AnimPlay_AtkCombo    = CreateTask_PlayAnimation(MA_ATK_COMBO);
    pTask_AnimPlay_AtkCombo->AddEnterFunc([=](auto) { m_iDamageType = 2; });
    auto pTask_AnimPlay_BackJump    = CreateTask_PlayAnimation(MA_LOCO_JUMP_BACK);
    pRandomSelector_AtkSel->AddChildNode(pTask_AnimPlay_Atk1);
    pRandomSelector_AtkSel->AddChildNode(pTask_AnimPlay_Atk2);
    pRandomSelector_AtkSel->AddChildNode(pTask_AnimPlay_AtkCombo);
    pRandomSelector_AtkSel->AddChildNode(pTask_AnimPlay_BackJump);

    pSequence_AtkSeq->AddChildNode(pDecorator_ForceTrue);
    pSequence_AtkSeq->AddChildNode(pRandomSelector_AtkSel);

    return pSequence_AtkSeq;
}

CBT_Node* CMonster_Stick1::CreateNode_MidCombat()
{

    CBT_Node::BT_NODE_DESC _randoSelDesc;
    _randoSelDesc.m_sNodeTag = L"Random Selector\n(Mid)";
    auto pRandomSelector_AtkSel = CBT_RandomSelector::Create(&_randoSelDesc);

    CBT_Node::BT_NODE_DESC _seqDesc;
    _seqDesc.m_sNodeTag = L"Sequence\n(Charge)";
    auto pSequence_AtkSeq = CBT_Sequence::Create(&_seqDesc);
    auto pTask_AnimPlay_ChargeInto      = CreateTask_PlayAnimation(5);
    auto pTask_AnimPlay_ChargeLoop      = CreateTask_PlayAnimation(6, true);

    auto pDecorator_ForceTrue           = CreateDecorator_ForceResult(true);
    auto pTask_MoveTo                   = CreateTask_MoveTo(20.f, m_fDeAggroRange, m_fChargeSpeed);
    pDecorator_ForceTrue->AddChildNode(pTask_MoveTo);

    auto pTask_AnimPlay_ChargeAttack    = CreateTask_PlayAnimation(7);
    pTask_AnimPlay_ChargeAttack->AddEnterFunc([=](auto) { m_iDamageType = 3; });

    pSequence_AtkSeq->AddChildNode(pTask_AnimPlay_ChargeInto);
    pSequence_AtkSeq->AddChildNode(pTask_AnimPlay_ChargeLoop);
    pSequence_AtkSeq->AddChildNode(pDecorator_ForceTrue);
    pSequence_AtkSeq->AddChildNode(pTask_AnimPlay_ChargeAttack);

    auto pTask_AnimPlay_AtkJump = CreateTask_PlayAnimation(MA_ATK_JUMP);
    pTask_AnimPlay_ChargeAttack->AddEnterFunc([=](auto) { m_iDamageType = 4; });

    pRandomSelector_AtkSel->AddChildNode(pSequence_AtkSeq);
    pRandomSelector_AtkSel->AddChildNode(pTask_AnimPlay_AtkJump);

    return pRandomSelector_AtkSel;
}

CBT_Node* CMonster_Stick1::CreateNode_LongCombat()
{
    CBT_Node::BT_NODE_DESC _longRandomSelDesc;
    _longRandomSelDesc.m_sNodeTag = L"Random Selector\n(Long)";
    auto pRandomSelector_LongSel = CBT_RandomSelector::Create(&_longRandomSelDesc);
    auto pTask_AnimPlay_AtkRock = CreateTask_PlayAnimation(4);


    CBT_Node::BT_NODE_DESC _seqDesc;
    _seqDesc.m_sNodeTag = L"Sequence\n(Chase)";
    auto pSequence_Chase = CBT_Sequence::Create(&_seqDesc);
    auto pTask_AnimPlay_Chase = CreateTask_PlayAnimation(8, true);
    auto pTask_LongMoveTo = CreateTask_MoveTo(m_fMidRange, m_fDeAggroRange, m_fWalkSpeed);
    pSequence_Chase->AddChildNode(pTask_AnimPlay_Chase);
    pSequence_Chase->AddChildNode(pTask_LongMoveTo);

    //pRandomSelector_LongSel->AddChildNode(pTask_AnimPlay_AtkRock);
    pRandomSelector_LongSel->AddChildNode(pSequence_Chase);

    return pRandomSelector_LongSel;
}

void CMonster_Stick1::Free()
{
    __super::Free();
}

HRESULT CMonster_Stick1::Reset()
{
    return S_OK;
}

bool CMonster_Stick1::OnDamage(FDamageInfo _fDamageInfo)
{
    if (_fDamageInfo.bIsCritical) _fDamageInfo.fDamage *= 2.f;
    if (!m_upHitTimer->IsActive()) {
        m_upHitTimer->Restart();
        m_upAnimController->ResetAnimNode(MA_HIT_SMALL);
        m_bIsHit = false;

        wstring sTag = L"VOX_Enemy_Stick_Pain";
        sTag += to_wstring(Random(0, 3));
        m_pGameInstance->Play_VOX(sTag);

        return __super::OnDamage(_fDamageInfo);
    }

    return false;
}

HRESULT CMonster_Stick1::Add_InitComponents()
{
    CHKFAIL(__super::Add_InitComponents())
    return S_OK;
}

HRESULT CMonster_Stick1::Load_Config()
{
    CHKFAIL(__super::Load_Config())

    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Status.HP",              &m_desc.m_fMaxHp);

    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Range.MidRange",         &m_fMidRange);
    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Range.LongRange",        &m_fLongRange);
    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Range.AggroRange",       &m_fAggroRange);
    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Range.DeAggroRange",     &m_fDeAggroRange);

    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Rocomotion.WalkSpeed",   &m_fWalkSpeed);
    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Rocomotion.ChargeSpeed", &m_fChargeSpeed);

    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Combat.Atk1Damage",      &m_fDamages[0]);
    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Combat.Atk2Damage",      &m_fDamages[1]);
    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Combat.ChargeDamage",    &m_fDamages[2]);
    m_pGameInstance->ReadSubConfig(L"EnemyDataBase.json", L"Minion.Stick01.Combat.JumpDamage",      &m_fDamages[3]);

    return S_OK;
}

void CMonster_Stick1::Revive()
{
    __super::Revive();
    m_wpAttackCollidor.lock()->SetActive(false);

    wstring sTag = L"SFX_Enemy_Stick_Ressurect";
    sTag += to_wstring(Random(0, 2));
    m_pGameInstance->Play_SFX(sTag, 0.7f);
    m_fNameplateOffset = 20.f;
}

void CMonster_Stick1::InitializeNotifies()
{
    __super::InitializeNotifies();

    //Attack1
    AddNotifiy( 0.f,  5.f, 68.f, L"SK_Sticks01|Attack_Attack1", L"FollowPlayer");
    AddNotifiy(15.f, 20.f, 68.f, L"SK_Sticks01|Attack_Attack1", L"AttackCollidorOn");

    //Attack2
    AddNotifiy( 0.f,  5.f, 80.f, L"SK_Sticks01|Attack_Attack2", L"FollowPlayer");
    AddNotifiy(20.f, 25.f, 80.f, L"SK_Sticks01|Attack_Attack2", L"AttackCollidorOn");

    //Charge
    AddNotifiy( 0.f, 20.f, 64.f, L"SK_Sticks01|Attack_Charge_Attack", L"FollowPlayer");
    AddNotifiy(20.f, 30.f, 64.f, L"SK_Sticks01|Attack_Charge_Attack", L"AttackCollidorOn");

    //Combo
    AddNotifiy( 0.f,  5.f, 84.f, L"SK_Sticks01|Attack_ComboAttack", L"FollowPlayer");
    AddNotifiy(15.f, 20.f, 84.f, L"SK_Sticks01|Attack_ComboAttack", L"AttackCollidorOn");
    AddNotifiy(36.f, 43.f, 84.f, L"SK_Sticks01|Attack_ComboAttack", L"AttackCollidorOn");
    AddNotifiy(56.f, 61.f, 84.f, L"SK_Sticks01|Attack_ComboAttack", L"AttackCollidorOn");

    //JumpAttack
    AddNotifiy( 0.f, 15.f, 83.f, L"SK_Sticks01|Attack_JumpAttack", L"FollowPlayer");
    AddNotifiy(30.f, 40.f, 83.f, L"SK_Sticks01|Attack_JumpAttack", L"AttackCollidorOn");

    m_wpModel.lock()->AddListener(L"AttackCollidorOn", nullptr,
        [=](CBone* pBone) {
            m_wpAttackCollidor.lock()->SetActive(true); 

            _wstring sTag = L"SFX_Enemy_Stick_AxeSwing";
            sTag += to_wstring(Random(0, 5));
            m_pGameInstance->Play_SFX(sTag, 2.f, 0, 0.7f);

            sTag = L"VOX_Enemy_Stick_ATK";
            sTag += to_wstring(Random(0, 2));
            m_pGameInstance->Play_VOX(sTag);
        },
            [=](CBone* pBone) {
            m_wpAttackCollidor.lock()->SetActive(false); }
    , false);


    m_wpModel.lock()->AddListener(L"FollowPlayer", [=](auto, float fDeltaTime) {
        if (!m_pPlayer) return;
        Get_MainTransform().lock()->LookAtDamp(
            m_pPlayer->Get_MainTransform().lock()->Get_LocalState(STATE::POSITION),
            fDeltaTime,
            1.f,
            true
        );
        m_wpModel.lock()->SetCurrentDir();
    }, nullptr, nullptr, false);


    AddSFXNotify(0.f, 16.f, L"Attack_Charge_Base",      L"SFX_Enemy_Stick_FS0", 2.f);
    AddSFXNotify(8.f, 16.f, L"Attack_Charge_Base",      L"SFX_Enemy_Stick_FS1", 2.f);
    AddSFXNotify(8.f, 64.f, L"Attack_Charge_Attack",    L"SFX_Enemy_Stick_FS0", 2.f);
    AddSFXNotify(15.f, 64.f, L"Attack_Charge_Attack",   L"SFX_Enemy_Stick_FS1", 2.f);
    AddSFXNotify(3.f, 24.f, L"Loco_Walk_Forward",       L"SFX_Enemy_Stick_FS0", 2.f);
    AddSFXNotify(15.f, 24.f, L"Loco_Walk_Forward",      L"SFX_Enemy_Stick_FS1", 2.f);
}

CMonster_Stick1::CMonster_Stick1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMonster(pDevice, pContext)
{
}

CMonster_Stick1::CMonster_Stick1(const CMonster_Stick1& Prototype)
    : CMonster(Prototype)
{
}

HRESULT CMonster_Stick1::Initialize_Prototype(void* pArg)
{
    CHKFAIL(__super::Initialize_Prototype(pArg))
    return S_OK;
}

HRESULT CMonster_Stick1::Initialize(void* pDesc)
{
    Load_Config();
    m_desc.m_fCurHp = m_desc.m_fMaxHp;

    FMONSTER_DESC _MonsterDesc;
    if (pDesc) {
        _MonsterDesc = *static_cast<FMONSTER_DESC*>(pDesc);
    }
    else {

    }
    lstrcpy(_MonsterDesc.m_sModelAssetName, L"Export_Monster_Stick0");
    CHKFAIL(__super::Initialize(&_MonsterDesc))
    m_upHitTimer = m_pGameInstance->CreateTimerWithDuration(0.2f, false, [=]() {
    }, false);
    return S_OK;
}

void CMonster_Stick1::SetBT()
{
    __super::SetBT();
    CBT_Node::BT_NODE_DESC _selDesc;
    _selDesc.m_sNodeTag = L"Selector\n(State)";
    auto pSelector_State = CBT_Selector::Create(&_selDesc);

    pSelector_State->AddChildNode(CreateNode_Revive (MA_REACTION_REVIVE));
    pSelector_State->AddChildNode(CreateNode_Dead   (MA_REACTION_DIE));
    pSelector_State->AddChildNode(CreateNode_Bind   (MA_REACTION_BIND));
    pSelector_State->AddChildNode(CreateNode_Parry  (MA_REACTION_PARRY));
    pSelector_State->AddChildNode(CreateNode_Hit    ());
    pSelector_State->AddChildNode(CreateNode_Bomb   (MA_REACTION_BOMB));
    //Combat

    CBT_Node::BT_NODE_DESC _randoSelDesc;
    _randoSelDesc.m_sNodeTag = L"Random Selector\n(Combat)";
    auto pRandomSelector_Sel = CBT_RandomSelector::Create(&_randoSelDesc);
    pRandomSelector_Sel->AddChildNode(CreateNode_Combat(), 4.f);
    pRandomSelector_Sel->AddChildNode(CreateNode_Taunt({ MA_REACTION_TAUNT }), 1.f);
    pSelector_State->AddChildNode(pRandomSelector_Sel);

    //Sequence

    //pSequence_AtkSeq->AddChildNode(pTask_MoveTo);

    m_pBT->AddRootNode(pSelector_State);

    m_pBombSocketBone = m_wpModel.lock()->FindBoneWithName(L"BindSocket");
}

void CMonster_Stick1::SetBB()
{
    __super::SetBB();
}

HRESULT CMonster_Stick1::Late_Initialize()
{
    wstring ConfigPath = L"../Animator_Monster_Stick1.json";
    Json::Value tmp;
    m_pGameInstance->LoadJson(ConfigPath, tmp);
    m_upAnimController = CAnimController::Create(tmp, m_wpModel.lock());
    m_upAnimController->SetActive(true);

    CHKFAIL(__super::Late_Initialize())

    
    m_wpAttackCollidor = ConvertWPComponent<CCollidor>(Get_Component(L"Com_Collidor_Attack_Axe"));
    m_wpAttackCollidor.lock()->Set_Func(EKEYACTIONSTATE::ENTER, [=](auto _desc, auto _pCollidor) {
        OnHit(_desc, _pCollidor);
    });
    m_wpAttackCollidor.lock()->SetActive(false);
    
    return S_OK;
}

void CMonster_Stick1::OnHit(const CONTACTMF_DESC& _MFdesc, CCollidor* pCollidor)
{
    auto _pGameObject = pCollidor->GetGameObject();
    if (pCollidor->GetDesc()->m_iCollidorIdx == CT_PLAYER_HIT) {
        FDamageInfo _desc;
        _desc.pSourceObj = this;
        _desc.fDamage = 10.f;
        _desc.bCanGuard = true;
        _desc.bCanParry = true;
        _desc.vNormal   = _MFdesc.contacts[0].vNormal;
        _desc.vPosition = _MFdesc.contacts[0].vPosition;
        m_pPlayer->OnDamage(_desc);
    }
}
void CMonster_Stick1::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    if (!m_bIsActive) return;
}

void CMonster_Stick1::Update(_float fTimeDelta)
{
    if (m_pGameInstance->IsKeyState(DIK_O, EKEYACTIONSTATE::ENTER) && m_bIsDead) {
        Revive();
        SetActive(true);
    }
    __super::Update(fTimeDelta);
    if (!m_bIsActive) return;
    fTimeDelta *= m_fTimeScale;
    m_upAnimController->Update(fTimeDelta);
    //Revive
    //Test
    DEBUGSPHERE_DESC _desc;
    _desc.fRadius = m_fMidRange;
    XMStoreFloat4x4(&_desc.vWorldMatrix, m_wpMainTransformCom.lock()->GetWorldMatrix());
    m_pGameInstance->AddDebugRenderCall(&_desc);
    _desc.fRadius = m_fLongRange;
    XMStoreFloat4x4(&_desc.vWorldMatrix, m_wpMainTransformCom.lock()->GetWorldMatrix());
    m_pGameInstance->AddDebugRenderCall(&_desc);
    _desc.fRadius = m_fDeAggroRange;
    XMStoreFloat4x4(&_desc.vWorldMatrix, m_wpMainTransformCom.lock()->GetWorldMatrix());
    m_pGameInstance->AddDebugRenderCall(&_desc);

    m_wpMainTransformCom.lock()->Set_LocalState(STATE::POSITION, m_wpNavigation.lock()->SetUp_OnNavigation(m_wpMainTransformCom.lock()->Get_LocalState(STATE::POSITION)));
}

void CMonster_Stick1::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    if (!m_bIsActive) return;
}
