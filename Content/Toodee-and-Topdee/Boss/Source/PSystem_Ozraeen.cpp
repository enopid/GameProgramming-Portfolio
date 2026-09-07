#include "PSystem_Ozraeen.h"
#include "Particle_Ozraeen.h"
#include "ParticleManager.h"
#include "GameInstance.h"


CPSystem_Ozraeen::CPSystem_Ozraeen(LPDIRECT3DDEVICE9 pGraphic_Device)
    : CPSystem(pGraphic_Device, 40)
    , m_pGraphic_Device{ pGraphic_Device }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
}

HRESULT CPSystem_Ozraeen::Initialize()
{
    m_strPrototypeTag = TEXT("Prototype_GameObject_Particle_Ozraeen");
    if (FAILED(CreatePool(m_iNumMaxParticles, m_strPrototypeTag))) return E_FAIL;
    CParticleManager::GetInstance()->AddSystem(this);

    CreateSpawnTimer(.05f);
    SetSpawnFunc([=]() {
        _float3 vPos = m_pInitTransform->Get_WorldState(STATE::POSITION);
        _float  _fAngle = ((rand() % 1000) / 1000.f) * D3DX_PI * 2.f;
        _float3 _vOffset = { cos(_fAngle), sin(_fAngle), 0.f };
        _vOffset *= ((rand() % 1000) / 1000.f) * 2.f;
        vPos += _vOffset;

        _float3 vDir = { 0.f, -1.f, 0.f };
        _float speed = (rand() % 1000) / 1000.f; // 초당 이동 거리 
        speed *= 2.f;
        speed += 2.f;

        _float _duration  = (rand() % 1000) / 1000.f; // 수명
        _duration *= .5f;
        _duration += 1.5f;

        _float _scale = (rand() % 1000) / 1000.f; // 시작 크기 
        _scale *= 1.5f;
        _scale += 1.f;

        auto _vecParticles = GetUnActiveParticles();
        for (auto _pParticle : _vecParticles) {
            _pParticle->SetDuration(_duration);
            _pParticle->SetScale(_scale, 0.f);
            _pParticle->Respawn(vPos, vDir, speed, 1.f);
        }
    });

    m_pHitTimer = m_pGameInstance->CreateTimerWithFrequency(6.f, true, [=]() {
        m_iBlinkCnt--;
        }, false);
    return S_OK;
}

HRESULT CPSystem_Ozraeen::CreatePool(_uint _MaxNum, const _wstring& strPrototypeTag)
{
    if (FAILED(__super::CreatePool(_MaxNum, strPrototypeTag)))
        return E_FAIL;

    return S_OK;
}

void CPSystem_Ozraeen::Trigger(_float3 vPos)
{
}

void CPSystem_Ozraeen::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    if (m_iBlinkCnt == 0)
        m_pHitTimer->SetActive(false);
    m_bBlink = m_iBlinkCnt % 2;
    for (auto& p : m_vecPool)
    {
        static_cast<CParticle_Ozraeen*>(p)->SetBlink(m_bBlink);
    }
}

void CPSystem_Ozraeen::Render()
{
    __super::Render();
}

void CPSystem_Ozraeen::Free()
{
    __super::Free();
}

void CPSystem_Ozraeen::DeactivateAll()
{
    __super::DeactivateAll();
}
