#include "Projectile_Peppermint_Bullet.h"
#include "Interact_Peppermint_Switch.h"
#include "Interact_Mimosa_Object.h"
#include "Interact_Magnetic.h"
#include "GameInstance.h"
#include "EffectRoot.h"
#include "EnemyBarrier.h"
#include "Enemy.h"
#include "Character.h"
#include "Kale_BArm.h"
#include "Interact_RoboArm.h"

CGameObject* CProjectile_Peppermint_Bullet::Clone(void* pArg)
{
    return CloneBase<CProjectile_Peppermint_Bullet>(pArg);
}

CProjectile_Peppermint_Bullet::CProjectile_Peppermint_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProjectile(pDevice, pContext)
{
}

CProjectile_Peppermint_Bullet::CProjectile_Peppermint_Bullet(const CProjectile_Peppermint_Bullet& Prototype)
    : CProjectile(Prototype)
{
}

HRESULT CProjectile_Peppermint_Bullet::Initialize(void* pDesc)
{
    CHKFAIL(__super::Initialize(pDesc))

    CHKFAIL(
        CGameObject::LoadPrototype(
            L"VFX_CH1000_Bullet",
            m_pGameInstance->GetLevelDesc().nextLevelTag,
            reinterpret_cast<CGameObject**>(&m_pEffect[BULLET]))
    )
    CHKFAIL(
        CGameObject::LoadPrototype(
            L"VFX_CH1000_Muzzle",
            m_pGameInstance->GetLevelDesc().nextLevelTag,
            reinterpret_cast<CGameObject**>(&m_pEffect[MUZZLE]))
    )
    CHKFAIL(
        CGameObject::LoadPrototype(
            L"VFX_CH1000_Hit",
            m_pGameInstance->GetLevelDesc().nextLevelTag,
            reinterpret_cast<CGameObject**>(&m_pEffect[HIT]))
    )
    for (_int i = 0; i < VFX::END; i++)
    {
        m_pEffect[i]->SetLevelObject();
        m_pEffect[i]->SetActive(false);
        m_pEffect[i]->Stop();
    }

    return S_OK;
}

HRESULT CProjectile_Peppermint_Bullet::Late_Initialize()
{
    CHKFAIL(__super::Late_Initialize())
    if (auto spCol = m_wpHitCollidor.lock()) {
        spCol->Set_Func(EKEYACTIONSTATE::ENTER, [=](auto, CCollidor* pCollidor) {
        if (pCollidor->GetDesc()->m_iCollidorIdx == CT_INTERACTIVE) {
            if (auto spSwitch = dynamic_cast<CInteract_Peppermint_Switch*>(pCollidor->GetGameObject())) {
                spSwitch->Trigger();
            }
            else if (auto spSwitch = dynamic_cast<CInteract_Magnetic*>(pCollidor->GetGameObject())) {
                spSwitch->Activate();
            }
            else if (auto spMimosaObject = dynamic_cast<CInteract_Mimosa_Object*>(pCollidor->GetGameObject())) {
                spMimosaObject->OnShieldDamage(20.f);
            }
        }
        if (pCollidor->GetDesc()->m_iCollidorIdx == CT_ENEMYBARRIER) {
            if (auto spBarrier = dynamic_cast<CEnemyBarrier*>(pCollidor->GetGameObject())) {
                spBarrier->OnDamage(10.f);
            }
        }
        if (pCollidor->GetDesc()->m_iCollidorIdx == CT_ENEMY_HIT) {
            ;

            CCharacter::FDamageInfo _desc;
            if (auto pEnemy = dynamic_cast<CEnemy*>(pCollidor->GetGameObject()))
            {
                _desc.fDamage = 1.f;
                _desc.eAttackType = CCharacter::EAttackType::NORMAL;
                if (!pEnemy->GetIsInvincible())
                {
                    pEnemy->OnDamage(_desc);
                }
            }
            if (auto pKaleArm = dynamic_cast<CKale_Arm*>(pCollidor->GetGameObject()))
            {
                if (pKaleArm->GetArmState() == CInteract_RoboArm::EARMSTATE::AS_GIMMIC_SUCCESS) {
                    pKaleArm->OnDamage(2.f);
                }
                else if (
                    pKaleArm->GetArmState() == CInteract_RoboArm::EARMSTATE::AS_GIMMIC_STAY &&
                    pKaleArm->GetArmType()  == CInteract_RoboArm::EARMTYPESTATE::AT_BARM) {
                    static_cast<CKale_BArm*>(pKaleArm)->OnShieldDamage(5.f);
                }
            }
        }
        });
    }
    return S_OK;
}

void CProjectile_Peppermint_Bullet::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_pEffect[BULLET]->Set_Pos(Get_MainTransform().lock()->Get_WorldState(STATE::POSITION));
}

void CProjectile_Peppermint_Bullet::Shot(RAY ray)
{
    __super::Shot(ray);
    if (m_pEffect) {
        m_pEffect[BULLET]->Set_Pos(XMLoadFloat4(&ray.vRayOrigin));
        m_pEffect[BULLET]->Set_Dir(XMLoadFloat4(&ray.vRayDir));
        m_pEffect[BULLET]->Restart();

        m_pEffect[MUZZLE]->Set_Pos(XMLoadFloat4(&ray.vRayOrigin));
        m_pEffect[MUZZLE]->Set_Dir(XMLoadFloat4(&ray.vRayDir));
        m_pEffect[MUZZLE]->Restart();
    }
}

HRESULT CProjectile_Peppermint_Bullet::Save(void* _pDesc, _uint& _iSize) const
{
    static_assert(is_trivially_copyable_v<EPROJECTILETYPE>);
    _iSize = sizeof(FPROJECTILEDESC);
    memcpy(_pDesc, &m_desc, _iSize);
    return S_OK;
}

HRESULT CProjectile_Peppermint_Bullet::Load(void* _pDesc)
{
    auto _iSize = sizeof(FPROJECTILEDESC);
    memcpy(&m_desc, _pDesc, _iSize);
    return S_OK;
}

void CProjectile_Peppermint_Bullet::Vanish()
{
    __super::Vanish();
    m_pEffect[BULLET]->Stop();

    auto pos = m_wpMainTransformCom.lock()->Get_LocalState(STATE::POSITION);
    m_pEffect[HIT]->Set_Pos(pos);
    m_pEffect[HIT]->Restart();
}
