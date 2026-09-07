#include "Ozraeen.h"
#include "Collider.h"
#include "GameInstance.h"
#include "Transform.h"

#include "Ozraeen_Head.h"
#include "Ozraeen_Part.h"

#include "FireBall.h"
#include "FirePellet.h"

#include "Toodee.h"
#include "Topdee.h"

#include "Key.h"

#include "PSystem_Ozraeen.h"
#include "ParticleManager.h"

COzraeen::COzraeen(LPDIRECT3DDEVICE9 pGraphic_Device) : CBoss(pGraphic_Device, EBOSSSTATE::BS_END, NUMPATTERNGROUP)
{
}

COzraeen::COzraeen(const COzraeen& Prototype) :  CBoss(Prototype)
{
}

HRESULT COzraeen::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype())) return E_FAIL;

    for (_uint i = 0; i < BP_END; i++)  m_pivotTransformComs[i]  = nullptr;
    for (_uint i = 0; i < BP_END; i++)  m_ListBodyParts[i]      = list<COzraeen_Part*>();

    return S_OK;
}

HRESULT COzraeen::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) return E_FAIL;
    
    m_pChaseTimer           = m_pGameInstance->CreateTimerWithDuration(2.f, false, nullptr, false);

    m_pWaveTimer            = m_pGameInstance->CreateTimerWithDuration(3.f, false, nullptr, false);
    m_pBlackFireballTimer   = m_pGameInstance->CreateTimerWithDuration(2.f, false, nullptr, false);
    m_pFIrePelletTimer      = m_pGameInstance->CreateTimerWithDuration(.5f, false, nullptr, false);

    m_pBounceTimer          = m_pGameInstance->CreateTimerWithDuration(2.f, true, nullptr, true);
    m_pHitTimer             = m_pGameInstance->CreateTimerWithDuration(1.5f, false, nullptr, true);

    m_iCurState = BS_IDLE;
    m_iPattern1Idx = 0;

    m_iCurGuidedFireShotCnt = m_iInitGuidedFireShotCnt;
    m_iCurFirePelletShotCnt = 0;

    //Set State
    CHKFAIL(SetMoveState())
    CHKFAIL(SetFireBallState())
    CHKFAIL(SetBlackWaveState())


    CHKFAIL(SetChaseState())
    CHKFAIL(SetGuidedFireBallState())
    CHKFAIL(SetFirePelletState())


    CHKFAIL(SetHitState())
    CHKFAIL(SetDieState())
    CHKFAIL(SetIdleState())

    //Set Pattern Group
    for (_uint iState : arrPattern1Group) CHKFAIL(AddStateInPatternGrouup(iState, 0))
    for (_uint iState : arrPattern2Group) CHKFAIL(AddStateInPatternGrouup(iState, 1))
    for (_uint iState : arrPattern3Group) CHKFAIL(AddStateInPatternGrouup(iState, 2))

    if (!m_pSystemOzraeen)
    {
        m_pSystemOzraeen = new CPSystem_Ozraeen(m_pGraphic_Device);
        m_pSystemOzraeen->Initialize();
        m_pSystemOzraeen->SetTransform(m_pivotTransformComs[BP_BODY2]);
    }
    HRESULT hr;

    

    hr = AddGroupTransitionFunc(0,1, [=]() { 
        if (CKey::IsUnlock() || m_pGameInstance->IsKeyState(VK_SPACE, EKeyActionState::Enter)) {
            AdjustBodyScale(1.0f);
            m_bHit = false;
            return true;
        }
        return false;
     });
    CHKFAIL(hr)
    hr = AddGroupTransitionFunc(1,2, [=]() { 
        if (m_bHit || m_pGameInstance->IsKeyState(VK_SPACE, EKeyActionState::Enter)) {
            AdjustBodyScale(1.0f);
            return true;
        }
        return false; 
    });
    CHKFAIL(hr)
    hr = AddGroupTransitionFunc(2,0, [=]() {
        if (m_pGameInstance->IsKeyState(VK_SPACE, EKeyActionState::Enter)) {
            AdjustBodyScale(1.0f);
            return true;
        }
        return false;
    });
    CHKFAIL(hr)

    return S_OK;
}

HRESULT COzraeen::SetFirePelletState()
{
    HRESULT hr;
    hr = AddStateFunc(BS_FIREPELLET, EKeyActionState::Enter, [=](_float _fDeltaTime) {
        for (_uint i = 0; i < MAXNUMFIREPELLETS; i++) {
            if (m_pFirePellets[i]->IsActive()) continue;
            auto _vPos = m_pTransformCom->Get_WorldState(STATE::POSITION)+ _float3{ 0.f, -3.f, -0.0f };
            _vPos.z = ProjectileZOffset;
            m_pFirePellets[i]->Shot(
                _vPos);
            m_pFIrePelletTimer->Restart();
            m_iCurFirePelletShotCnt--;
            break;
        }
        });
    CHKFAIL(hr)

    hr = AddTransitionFunc(BS_FIREPELLET, BS_CHASE, [=]() {
    if (!m_pFIrePelletTimer->IsActive() && m_iCurFirePelletShotCnt <= 0) {
        return true;
    }
    return false;
        });
    CHKFAIL(hr)

    hr = AddTransitionFunc(BS_FIREPELLET, BS_FIREPELLET, [=]() {
    return !m_pFIrePelletTimer->IsActive() && m_iCurFirePelletShotCnt > 0;
        });
    CHKFAIL(hr)
    return S_OK;
}

HRESULT COzraeen::SetHitState()
{
    HRESULT hr;
    hr = AddStateFunc(BS_HIT, EKeyActionState::Enter, [=](_float _fDeltaTime) {
        HitBlink_Reset();
        m_pSystemOzraeen->HitBlinkReset();
        m_pHitTimer->Restart();
        m_iHP--;
    });
    CHKFAIL(hr)
        hr = AddStateFunc(BS_HIT, EKeyActionState::Exit, [=](_float _fDeltaTime) {
        m_bHit = false;
    });
    CHKFAIL(hr)

    hr = AddTransitionFunc(BS_HIT, BS_IDLE, [=]() {return !m_pHitTimer->IsActive(); });
    CHKFAIL(hr)
    hr = AddTransitionFunc(BS_HIT, BS_DIE, [=]() {return m_iHP==0; });
    CHKFAIL(hr)

    return S_OK;
}

HRESULT COzraeen::SetDieState()
{
    //HRESULT hr;
    return S_OK;
}

HRESULT COzraeen::SetIdleState()
{
    HRESULT hr;
    hr = AddStateFunc(BS_IDLE, EKeyActionState::Enter, [=](_float _fDeltaTime) {
        m_iPattern1Idx = 0;
    });
    CHKFAIL(hr)

    hr = AddTransitionFunc(BS_IDLE, BS_MOVE, [=]() {return true; });
    CHKFAIL(hr)

    return S_OK;
}

HRESULT COzraeen::SetGuidedFireBallState()
{
    HRESULT hr;
    hr = AddStateFunc(BS_BLACKFIREBALL, EKeyActionState::Enter, [=](_float _fDeltaTime) {
        for (_uint i = 0; i < MAXGUIDEDFIREBALLCNT; i++){
            if (m_pGuidedFireBalls[i]->IsActive()) continue;
            auto _vPos = m_pTransformCom->Get_WorldState(STATE::POSITION);
            _vPos.z = ProjectileZOffset;
            m_pGuidedFireBalls[i]->Shot(
                _vPos,
                _float3{ 1.f, 1.f, 0.f });
            m_pBlackFireballTimer->Restart();
            m_iCurGuidedFireShotCnt--;
            break;
        }
    });
    CHKFAIL(hr)

    hr = AddStateFunc(BS_BLACKFIREBALL, EKeyActionState::Stay, [=](_float _fDeltaTime) {
        _float _fRatio = static_cast<_float>(m_pBlackFireballTimer->GetElapsedRatio());
        m_pTransformCom->Rotation(_float3{ 0.f, 1.f, 0.f }, 4 * 2 * D3DX_PI * _fRatio);

        AdjustBodyScale(_fRatio);
        
    });
    CHKFAIL(hr)

    hr = AddTransitionFunc(BS_BLACKFIREBALL, BS_CHASE, [=]() {
        if (!m_pBlackFireballTimer->IsActive() && m_iCurGuidedFireShotCnt <= 0) {
            return true;
        }
        return false;
    });
    CHKFAIL(hr)

    hr = AddTransitionFunc(BS_BLACKFIREBALL, BS_BLACKFIREBALL, [=]() {
        return !m_pBlackFireballTimer->IsActive() && m_iCurGuidedFireShotCnt > 0;
    });
    CHKFAIL(hr)

    return S_OK;
}

void COzraeen::AdjustBodyScale(const Engine::_float& _fRatio)
{
    _float _fScale = _fRatio * 64.f * g_fDot + 0.01f;
    for (_uint i = BP_BODY0; i <= BP_BODY2; i++)
    {
        for (auto& _pPart : m_ListBodyParts[i]) {
            static_cast<CTransform*>(_pPart->Get_Component(L"Com_Transform"))->Scale(_fScale, _fScale, _fScale);
        }
    }
}

HRESULT COzraeen::SetBlackWaveState()
{
    HRESULT hr;
    hr = AddStateFunc(BS_BLACKWAVE, EKeyActionState::Enter, [=](_float _fDeltaTime) {
        Scream_Reset(4);
        HitBlink_Reset();
        m_pSystemOzraeen->HitBlinkReset();
        m_pWaveTimer->Restart();
        });
    CHKFAIL(hr)
    hr = AddStateFunc(BS_BLACKWAVE, EKeyActionState::Stay, [=](_float _fDeltaTime) {

        _float _fRatio = static_cast<_float>(m_pWaveTimer->GetElapsedRatio());
        m_pTransformCom->Rotation(_float3{ 0.f, 1.f, 0.f }, 10 * 2 * D3DX_PI * _fRatio);

        if (_fRatio < 0.5f) return;

        for (_uint i = 0; i < WAVEFIREBALLCNT; i++) {
            if (m_pWaveFireBalls[i]->IsActive()) break;
            auto _vPos = m_pTransformCom->Get_WorldState(STATE::POSITION);
            _vPos.z = ProjectileZOffset;
            m_pWaveFireBalls[i]->Shot(
                m_pTransformCom->Get_WorldState(STATE::POSITION),
                _float3{ sin(D3DX_PI * 2.f * i / WAVEFIREBALLCNT), cos(D3DX_PI * 2.f * i / WAVEFIREBALLCNT), 0.f });
        }
    });
    CHKFAIL(hr)
    hr = AddStateFunc(BS_BLACKWAVE, EKeyActionState::Exit, [=](_float _fDeltaTime) {

        });


    hr = AddTransitionFunc(BS_BLACKWAVE, BS_MOVE, [=]() { return !m_pWaveTimer->IsActive();  });
    CHKFAIL(hr)

    return S_OK;
}

HRESULT COzraeen::SetFireBallState()
{
    HRESULT hr;
    hr = AddStateFunc(BS_BIGFIREBALL, EKeyActionState::Enter, [=](_float _fDeltaTime) {
        if (m_pFireBall) {
            auto _vPos = m_pTransformCom->Get_WorldState(STATE::POSITION) + _float3{ 0.f, -1.f, 0.f };
            _vPos.z = ProjectileZOffset;
            m_pFireBall->Shot(
                _vPos,
                _float3{ 0.f, -1.f, 0.f }
            );
        }
        });
    CHKFAIL(hr)
    hr = AddStateFunc(BS_BIGFIREBALL, EKeyActionState::Exit, [=](_float _fDeltaTime) {});
    CHKFAIL(hr)


    hr = AddTransitionFunc(BS_BIGFIREBALL, BS_MOVE, [=]() {
        return true;
    });
    CHKFAIL(hr)
    return S_OK;
}

HRESULT COzraeen::SetMoveState()
{
    HRESULT hr;

    hr = AddStateFunc(BS_MOVE, EKeyActionState::Enter, [=](_float _fDeltaTime) {
        m_iPattern1Idx = (m_iPattern1Idx + 1) % 12;
        m_vTargetPos = PatternMovePos[m_iPattern1Idx];
        });
    CHKFAIL(hr)

    //Transit Func
    //MOVE->FIREBALL
    hr = AddTransitionFunc(BS_MOVE, BS_BIGFIREBALL, [=]() {
    if (IsArrived()) { if ((m_iPattern1Idx % 6) != 5 && (m_iPattern1Idx % 6) != 0) return true; }
    return false;
        });
    CHKFAIL(hr)

    //MOVE->MOVE
    hr = AddTransitionFunc(BS_MOVE, BS_MOVE, [=]() { return (IsArrived() && ((m_iPattern1Idx % 6) == 0)); });
    CHKFAIL(hr)
    //MOVE->WAVE
    hr = AddTransitionFunc(BS_MOVE, BS_BLACKWAVE, [=]() { return (IsArrived() && ((m_iPattern1Idx % 6) == 5)); });
    CHKFAIL(hr)

    return S_OK;
}

HRESULT COzraeen::SetChaseState()
{
    HRESULT hr;
    hr = AddStateFunc(BS_CHASE, EKeyActionState::Stay, [=](_float _fDeltaTime) { Chase(_fDeltaTime); });
    CHKFAIL(hr)
    hr = AddStateFunc(BS_CHASE, EKeyActionState::Enter, [=](_float _fDeltaTime) { m_pChaseTimer->Restart(); });
    CHKFAIL(hr)

    //CHASE->BLACKFIREBALL
    hr = AddTransitionFunc(BS_CHASE, BS_BLACKFIREBALL,  [=]() {
        if (!m_pChaseTimer->IsActive() && m_iCurGuidedFireShotCnt >0) {
            m_iCurFirePelletShotCnt = m_iInitFirePelletShotCnt;
            return true;
        }
        return false; });
    CHKFAIL(hr)

    //CHASE->FIREPELLET
    hr = AddTransitionFunc(BS_CHASE, BS_FIREPELLET,     [=]() {
        if (!m_pChaseTimer->IsActive() && m_iCurFirePelletShotCnt > 0) {
            m_iCurGuidedFireShotCnt = m_iInitGuidedFireShotCnt;
            return true;
        }
        return false; });
    CHKFAIL(hr)

    return S_OK;
}

void COzraeen::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void COzraeen::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    if (m_iCurState != BS_BLACKWAVE && m_iCurState != BS_BLACKFIREBALL && m_iCurState != BS_FIREPELLET) {
        Move(fTimeDelta);
    }
    UpdateBodyChain(fTimeDelta);
}

void COzraeen::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT COzraeen::Ready_Components()
{
    if (FAILED(__super::Ready_Components())) return E_FAIL;

    /* Com_Collider */
    CCollider::FColliderDesc ColliderDesc{};
    ColliderDesc.bInitialActive = true;
    ColliderDesc.bIsTrigger = false;
    ColliderDesc.func = [=](FCollisionInfo _collisionInfo) { OnCollision(_collisionInfo); };
    ColliderDesc.iLayerLevelIndex = 0;
    ColliderDesc.pObject = this;
    ColliderDesc.pTransform = m_pTransformCom;
    ColliderDesc.strCollisionLayerTag = TEXT("Boss");
    FRectColliderInfo _collisionInfo;
    _collisionInfo.fHeight = 2;
    _collisionInfo.fWidth = 2;
    _collisionInfo.vCenter = _float2{ 0,0 };
    ColliderDesc.colliderInfo = _collisionInfo;

    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    //Transform
    CTransform::TRANSFORM_DESC		TransformDesc{0.f, 0.f};
    wstring _strCom = L"Com_Transform";
    for (_uint i = 0; i < BP_END; i++)
    {
        if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
            (_strCom + to_wstring(i)), reinterpret_cast<CComponent**>(&m_pivotTransformComs[i]), &TransformDesc)))
            return E_FAIL;
        CTransform::TRANSFORM_DESC		_transformDesc{ 0.f, 0.f, m_pTransformCom };
        if (i< BP_BODY0 || i> BP_BODY2)
            m_pivotTransformComs[i]->SetParent(&_transformDesc);
    }

    m_pivotTransformComs[BP_NOSE] ->Set_State(STATE::POSITION,  _float3{+0.00f, 1.10f, -1.5f});
    m_pivotTransformComs[BP_LHEAD]->Set_State(STATE::POSITION,  _float3{-0.9f, 1.66f, -1.01f});
    m_pivotTransformComs[BP_RHEAD]->Set_State(STATE::POSITION,  _float3{+0.9f, 1.66f, -1.01f});


    float _angle = D3DX_PI / 6;
    float _radius = 1.15f;
    m_pivotTransformComs[BP_MOUTH0]->Set_State(STATE::POSITION, _float3{ +_radius * sin(-2 * _angle), 0.f, -_radius * cos(-2 * _angle) });
    m_pivotTransformComs[BP_MOUTH1]->Set_State(STATE::POSITION, _float3{ +_radius * sin(-1 * _angle), 0.f, -_radius * cos(-1 * _angle) });
    m_pivotTransformComs[BP_MOUTH2]->Set_State(STATE::POSITION, _float3{ +_radius * sin(+0 * _angle), 0.f, -_radius * cos(+0 * _angle) });
    m_pivotTransformComs[BP_MOUTH3]->Set_State(STATE::POSITION, _float3{ +_radius * sin(+1 * _angle), 0.f, -_radius * cos(+1 * _angle) });
    m_pivotTransformComs[BP_MOUTH4]->Set_State(STATE::POSITION, _float3{ +_radius * sin(+2 * _angle), 0.f, -_radius * cos(+2 * _angle) });

    m_pivotTransformComs[BP_BODY0]->Set_State(STATE::POSITION, _float3{0.f, 0.f, 0.f});
    m_pivotTransformComs[BP_BODY1]->Set_State(STATE::POSITION, _float3{0.f, -1.f, 0.f});

    m_pTransformCom->Set_State(STATE::POSITION, _float3{ 15.f,8.f, ZOffset });

    return S_OK;
}


COzraeen* COzraeen::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{

    COzraeen* pInstance = new COzraeen(pGraphic_Device);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : COzraeen");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* COzraeen::Clone(void* pArg)
{

    COzraeen* pInstance = new COzraeen(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : COzraeen");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void COzraeen::Free()
{
    __super::Free();

    CParticleManager::GetInstance()->Clear(m_pSystemOzraeen);
    Safe_Release(m_pSystemOzraeen);

    for (_uint i = 0; i < BP_END; i++) Safe_Release(m_pivotTransformComs[i]);
    for (_uint i = 0; i < BP_END; i++) { m_ListBodyParts[i].clear(); } 

    Safe_Release(m_pFireBall);
    for (_uint i = 0; i < WAVEFIREBALLCNT; i++)         Safe_Release(m_pWaveFireBalls[i]);
    for (_uint i = 0; i < MAXGUIDEDFIREBALLCNT; i++)    Safe_Release(m_pGuidedFireBalls[i]);
    for (_uint i = 0; i < MAXNUMFIREPELLETS; i++)       Safe_Release(m_pFirePellets[i]);
}

HRESULT COzraeen::AddBodyPart(COzraeen_Part* _pBodyPart, EBOSSPART _eBossPart)
{
    if (_eBossPart >= BP_END)                                                       return E_FAIL;
    if (FAILED(_pBodyPart->SetParent(this)))                                        return E_FAIL;
    if (FAILED(_pBodyPart->SetParentTransform(m_pivotTransformComs[_eBossPart])))   return E_FAIL;
    m_ListBodyParts[_eBossPart].push_back(_pBodyPart);
    return S_OK;
}

HRESULT COzraeen::CreateBody()
{
    //Create Body Part

    return S_OK;
}

void COzraeen::HitBlink_Reset()
{
    for (_uint i = 0; i < BP_END; i++) for (auto& _pPart : m_ListBodyParts[i]) _pPart->Hit_Reset();
}

HRESULT COzraeen::Reset()
{
    __super::Reset();
    m_pTransformCom->Set_State(STATE::POSITION, Pattern1MovePos1);
    AdjustBodyScale(1.f);

    m_iCurState = BS_IDLE;
    m_iPattern1Idx = 0;

    m_iCurGuidedFireShotCnt = m_iInitGuidedFireShotCnt;
    m_iCurFirePelletShotCnt = 0;


    for (_uint i = 0; i < WAVEFIREBALLCNT; i++)         m_pWaveFireBalls[i]->Vanish();
    for (_uint i = 0; i < MAXGUIDEDFIREBALLCNT; i++)    m_pGuidedFireBalls[i]->Vanish();
    for (_uint i = 0; i < MAXNUMFIREPELLETS; i++)       m_pFirePellets[i]->Vanish();
    m_pFireBall->Vanish();
    return S_OK;
}

_bool COzraeen::IsArrived()
{
    _float3 _vDir = m_vTargetPos - m_pTransformCom->Get_State(STATE::POSITION);
    _float _fDist = D3DXVec3Length(&_vDir);

    bool _bIsArrived = false;
    if (_fDist < MoveThreshold) {
        m_pTransformCom->Set_State(STATE::POSITION, m_vTargetPos);
        _bIsArrived = true;
    }
    return _bIsArrived;
}

void COzraeen::Move(_float fTimeDelta)
{
    auto _vTargetPos = m_vTargetPos;

    _vTargetPos.y += 4.f * cos(static_cast<_float>(m_pBounceTimer->GetElapsedRatio()) * D3DX_PI * 2.f);

    _float3 _vDir = _vTargetPos - m_pTransformCom->Get_State(STATE::POSITION);
    _vDir.z = 0;
    _float _fDist = D3DXVec3Length(&_vDir);
    D3DXVec3Normalize(&_vDir, &_vDir);

    _float3 _vNxtPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vNxtPos += _vDir * (_fDist / 10.f) * 3.f * MoveSpeed * fTimeDelta;
    _vNxtPos.z = ZOffset;
    m_pTransformCom->Set_State(STATE::POSITION, _vNxtPos);


    m_pTransformCom->Rotation(_float3{ 0.f, 1.f, 0.f }, D3DX_PI / 2 * -(_vDir.x) * (_fDist)/12.f);
}

void COzraeen::Chase(_float fTimeDelta)
{
    auto [_state, _fRatio] = m_pGameInstance->Get_DimensionInfo();
    CTransform* _pTargetTransform = nullptr;
    if (_fRatio > 0.5f)
        _pTargetTransform = static_cast<CTransform*>(m_pToodee->Get_Component(L"Com_Transform"));
    else
        _pTargetTransform = static_cast<CTransform*>(m_pTopdee->Get_Component(L"Com_Transform"));
    m_vTargetPos = _pTargetTransform->Get_WorldState(STATE::POSITION);
    m_vTargetPos.y = g_iMapSizeY - Pattern1Offset;

}

void COzraeen::UpdateBodyChain(_float fTimeDelta)
{
    _float3 _vNxtPos, _vCurPos, _vLerpPos;

    _vNxtPos = m_pTransformCom->Get_State(STATE::POSITION) + _float3{ 0.f, 1.f, 0.f };
    for (_uint i = BP_BODY0; i <= BP_BODY2; i++)
    {
        _vCurPos = m_pivotTransformComs[i]->Get_State(STATE::POSITION);


        _double _ratio = pow((pow(10, -60)), static_cast<_double>(fTimeDelta));
        _vLerpPos = _vCurPos * (1-_ratio) + _vNxtPos * (_ratio);

        m_pivotTransformComs[i]->Set_State(STATE::POSITION, _vLerpPos);

        _vNxtPos = _vLerpPos + _float3{ 0.f, -1.f, 0.f };
    }

}

HRESULT COzraeen::CreateResource(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
    auto iPrototypeLevelIndex = ENUM_TO_UINT(LEVEL::BOSS_STATIC);
    auto strPrototypeTag = TEXT("Prototype_GameObject_FireBall");

    CFireBall::FFireBallDesc _desc;

    _desc.pTopdee = m_pTopdee;
    _desc.pToodee = m_pToodee;

    _desc.fSpeed      = 12.f;
    _desc.fFireDelay  = .76f;
    _desc.bWallVanish = false;

    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag,
        iLayerLevelIndex, strLayerTag, &_desc, reinterpret_cast<CGameObject**>(&m_pFireBall))))
        return E_FAIL;

    _desc.fFireDelay = 0.f;
    _desc.fSpeed    = 6.f;
    _desc.bIsBlack = true;
    for (_uint i = 0; i < WAVEFIREBALLCNT; i++)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag,
            iLayerLevelIndex, strLayerTag, &_desc, reinterpret_cast<CGameObject**>(&m_pWaveFireBalls[i]))))
            return E_FAIL;
    }

    _desc.fFireDelay    = 0.f;
    _desc.fSpeed        = 7.f;
    _desc.bIsBlack      = true;
    _desc.bIsGuided     = true;
    for (_uint i = 0; i < MAXGUIDEDFIREBALLCNT; i++)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag,
            iLayerLevelIndex, strLayerTag, &_desc, reinterpret_cast<CGameObject**>(&m_pGuidedFireBalls[i]))))
            return E_FAIL;
    }

    strPrototypeTag = TEXT("Prototype_GameObject_FirePellet");

    CFirePellet::FFirePelletDesc _PelletDesc;
    _PelletDesc.vInitPos  = m_pTransformCom->Get_WorldState(STATE::POSITION);
    _PelletDesc.iDir      = 1;
    for (_uint i = 0; i < MAXNUMFIREPELLETS; i++) {
        CFirePellet* _pObject;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iPrototypeLevelIndex, strPrototypeTag,
            iLayerLevelIndex, strLayerTag, &_PelletDesc, reinterpret_cast<CGameObject**>(&_pObject))))
            return E_FAIL;
        m_pFirePellets[i] = _pObject;
    }

    return S_OK;
}

void COzraeen::OnCollision(FCollisionInfo _fCollisionInfo)
{
    __super::OnCollision(_fCollisionInfo);

    if (_fCollisionInfo.strCollisionLayerTag == L"FirePellet") {
        m_bHit = true;
    }
}
