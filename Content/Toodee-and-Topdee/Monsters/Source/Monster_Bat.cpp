#include "Monster_Bat.h"
#include "Collider.h"
#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

#include "Toodee.h"
#include "Topdee.h"

CMonster_Bat* CMonster_Bat::Create(LPDIRECT3DDEVICE9 pGraphic_Deivce)
{

    CMonster_Bat* pInstance = new CMonster_Bat(pGraphic_Deivce);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CMonster_Bat");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster_Bat::Clone(void* pArg)
{

    CMonster_Bat* pInstance = new CMonster_Bat(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CMonster_Bat");
        Safe_Release(pInstance);
    }

    return pInstance;


}

void CMonster_Bat::Move(_float _deltaTime)
{
    auto _vTargetPos = m_pTransformCom->Get_WorldState(STATE::POSITION) + ((m_bIsRight) ? 1 : -1) * m_fMoveSpeed * _deltaTime * _float3(1.f, 0.f, 0.f);
    m_pTransformCom->Set_State(STATE::POSITION, _vTargetPos);
}

HRESULT CMonster_Bat::TwoDimRender()
{
    CHKFAIL(BodyRender());

    //ShadowRender
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0x10);

    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 0, 0, 0));
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

    m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 0, 0, 0));

    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    
    CTransformGaurd _trGaurd(m_pTransformCom);
    auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
    m_pTransformCom->Scaling(1.f, 1.f, 0.f);
    _vPos.z = 0.f;
    _vPos.y -= .5f;
    m_pTransformCom->Set_State(STATE::POSITION, _vPos);

    for (_uint i = 0; i < BP_END; i++) {
        m_pBodyPartTextureTransforms[i]->Set_State(STATE::POSITION, m_pBodyPartPivotTransforms[i]->Get_WorldState(STATE::POSITION));
        if (i == BP_LWING || i == BP_RWING || i == BP_LLEG || i == BP_RLEG || i == BP_LEAR || i == BP_REAR) continue;
        BillBoard(m_pBodyPartTextureTransforms[i]);
    }


    //Adjust Transform
    CHKFAIL(BodyRender());

    return S_OK;
}

HRESULT CMonster_Bat::TopDimRender()
{
    CHKFAIL(BodyRender());

    //ShadowRender
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0x10);

    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 0, 0, 0));
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

    m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(255, 0, 0, 0));

    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    CTransformGaurd _trGaurd(m_pTransformCom);
    auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
    m_pTransformCom->Scaling(1.f, 1.f, 0.f);
    _vPos.z = 0.f;
    m_pTransformCom->Set_State(STATE::POSITION, _vPos);
    
    for (_uint i = 0; i < BP_END; i++) {
        m_pBodyPartTextureTransforms[i]->Set_State(STATE::POSITION, m_pBodyPartPivotTransforms[i]->Get_WorldState(STATE::POSITION));
        if (i == BP_LWING || i == BP_RWING || i == BP_LLEG || i == BP_RLEG || i == BP_LEAR || i == BP_REAR) continue;
        BillBoard(m_pBodyPartTextureTransforms[i]);
    }

    //Adjust Transform
    CHKFAIL(BodyRender());

    return S_OK;
}

HRESULT CMonster_Bat::BodyRender()
{
    CHKFAIL(m_pVIBufferCom->Bind_Buffers())

        for (_uint i = 0; i < BP_END; i++) {
            CHKFAIL(m_pBodyPartTextureTransforms[i]->Bind_Resource())
                switch (i)
                {
                case Client::CMonster_Bat::BP_BODY:
                    CHKFAIL(m_pBodyTexture->Bind_Texture(0))
                        break;
                case Client::CMonster_Bat::BP_MOUTH:
                    CHKFAIL(m_pBodyTexture->Bind_Texture(3))
                        break;
                case Client::CMonster_Bat::BP_LLEG:
                case Client::CMonster_Bat::BP_RLEG:
                    CHKFAIL(m_pBodyTexture->Bind_Texture(4))
                        break;
                case Client::CMonster_Bat::BP_LEYE:
                case Client::CMonster_Bat::BP_REYE:
                    CHKFAIL(m_pBodyTexture->Bind_Texture(2))
                        break;
                case Client::CMonster_Bat::BP_LEAR:
                case Client::CMonster_Bat::BP_REAR:
                    CHKFAIL(m_pBodyTexture->Bind_Texture(1))
                        break;
                case Client::CMonster_Bat::BP_LWING:
                case Client::CMonster_Bat::BP_RWING:
                    CHKFAIL(m_pWingTexture->Bind_AnimTexture())
                        break;
                case Client::CMonster_Bat::BP_END:
                    break;
                default:
                    break;
                }
            CHKFAIL(m_pVIBufferCom->Render())
        }

    return S_OK;
}

HRESULT CMonster_Bat::SetTexture()
{
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_Bat_Body"),
        TEXT("Com_TextureBody"), reinterpret_cast<CComponent**>(&m_pBodyTexture))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_Bat_Wing"),
        TEXT("Com_TextureWing"), reinterpret_cast<CComponent**>(&m_pWingTexture))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Bat::SetTransform()
{

    CTransform::TRANSFORM_DESC		TransformDesc{};
    TransformDesc.parent = m_pTransformCom;

    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
        TEXT("Com_TopdeeTransform"), reinterpret_cast<CComponent**>(&m_pBodyTransform), &TransformDesc)))
        return E_FAIL;
    m_pBodyTransform->SetParent(&TransformDesc);

    for (_uint i = 0; i < BP_END; i++)
    {
        if (i==BP_BODY) TransformDesc.parent = m_pBodyTransform;
        else            TransformDesc.parent = m_pBodyPartPivotTransforms[BP_BODY];
        if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
            wstring(L"Com_TopdeePivotTransform") + to_wstring(i), reinterpret_cast<CComponent**>(&m_pBodyPartPivotTransforms[i]), &TransformDesc)))
            return E_FAIL;
        m_pBodyPartPivotTransforms[i]->SetParent(&TransformDesc);

        if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
            wstring(L"Com_TopdeeTextureTransform") + to_wstring(i), reinterpret_cast<CComponent**>(&m_pBodyPartTextureTransforms[i]), &TransformDesc)))
            return E_FAIL;
    }

    //Adjust Scale
    m_pBodyPartTextureTransforms[BP_LWING]->  Scale(21 * g_fDot, 45 * g_fDot, 1.f);
    m_pBodyPartTextureTransforms[BP_RWING]->  Scale(21 * g_fDot, 45 * g_fDot, 1.f);
    for (_uint i = 0; i < BP_END; i++) {
        if (i == BP_RWING || i == BP_LWING) continue;
        m_pBodyPartTextureTransforms[i]->Scale(19 * g_fDot, 18 * g_fDot, 1.f);
    }

    //Adjust Location
    //m_pBodyPartPivotTransforms

    _float _fR = 3.6f * g_fDot;
    //m_pBodyPartPivotTransforms[BP_BODY]->   Set_State(STATE::POSITION, _float3{ +0.0f * _fR, +0.0f * _fR, +0.0f * _fR });
    

    m_pBodyPartPivotTransforms[BP_MOUTH]->  Set_State(STATE::POSITION, _float3{ +2.8f * _fR, -0.6f * _fR, +0.0f * _fR });

    m_pBodyPartPivotTransforms[BP_LEYE]->   Set_State(STATE::POSITION, _float3{ +1.4f * _fR, +0.5f * _fR, -1.4f * _fR });
    m_pBodyPartPivotTransforms[BP_REYE]->   Set_State(STATE::POSITION, _float3{ +1.4f * _fR, +0.5f * _fR, +1.4f * _fR });

    m_pBodyPartPivotTransforms[BP_LEAR]->   Set_State(STATE::POSITION, _float3{ +0.5f * _fR, +2.4f * _fR, -1.5f * _fR });
    m_pBodyPartPivotTransforms[BP_REAR]->   Set_State(STATE::POSITION, _float3{ +0.5f * _fR, +2.4f * _fR, +1.5f * _fR });
    
    m_pBodyPartPivotTransforms[BP_LLEG]->   Set_State(STATE::POSITION, _float3{ +1.2f * _fR, -3.2f * _fR, -1.5f * _fR });
    m_pBodyPartPivotTransforms[BP_RLEG]->   Set_State(STATE::POSITION, _float3{ +1.2f * _fR, -3.2f * _fR, +1.5f * _fR });

    m_pBodyPartPivotTransforms[BP_LWING]->  Set_State(STATE::POSITION, _float3{ -1.8f * _fR, -0.2f * _fR, -3.8f * _fR });
    m_pBodyPartPivotTransforms[BP_RWING]->  Set_State(STATE::POSITION, _float3{ -1.8f * _fR, -0.2f * _fR, +3.8f * _fR });

    return S_OK;
}

CMonster_Bat::CMonster_Bat(LPDIRECT3DDEVICE9 pGraphic_Deivce)
    : CMonster(pGraphic_Deivce, _float2{26 * g_fDot, 28 * g_fDot })
{
}

CMonster_Bat::CMonster_Bat(const CMonster_Bat& Prototype)
    : CMonster(Prototype)
{
}

HRESULT CMonster_Bat::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Bat::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    m_pWingTexture->SetAnimationDuration(BATANIMDURATION);

    m_fChaseSpeed   = 5.f;
    m_fMoveSpeed    = 4.f;

    return S_OK;
}

void CMonster_Bat::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CMonster_Bat::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    _float _theta;
    if (m_eState == EDimensionState::TOPDIM || m_eState == EDimensionState::TOTOPDIM) {
        auto _vPos = m_pTransformCom->Get_WorldState(STATE::POSITION);
        _vPos.z = 0.8 * _vPos.z + 0.2 * (min(m_fChaseDist, 4.f) * 3.f/4.f + 1.f) / 4.f * -1.f * BATHEIGHT;

        m_pTransformCom->Set_State(STATE::POSITION, _vPos);

        m_pBodyTransform->Rotation(_float3{ -1.f,0.f,0.f }, 4 * (D3DX_PI / 18.f));

        _theta = atan2(m_vChaseDir.y, m_vChaseDir.x);
        m_pBodyPartPivotTransforms[BP_BODY]->Rotation({ 0.f, -1.f, 0.f }, _theta);
    }
    else {
        auto _vPos = m_pTransformCom->Get_WorldState(STATE::POSITION);
        _vPos.z = 0.8 * _vPos.z + 0.2 * -BATHEIGHT;
        m_pTransformCom->Set_State(STATE::POSITION, _vPos);

        m_pBodyTransform->Rotation(_float3{ -1.f,0.f,0.f }, 1 * (D3DX_PI / 18.f));

        _theta = ((m_bIsRight) ? 1 : -1) * D3DX_PI / 18.f - D3DX_PI * 0.5f;
    }
    m_pBodyPartPivotTransforms  [BP_BODY]   ->Rotation({ 0.f, -1.f, 0.f }, _theta);
    m_pBodyPartTextureTransforms[BP_RLEG]   ->Rotation({ 0.f, -1.f, 0.f }, _theta + (D3DX_PI / 18.f) * +2.0f);
    m_pBodyPartTextureTransforms[BP_LLEG]   ->Rotation({ 0.f, -1.f, 0.f }, _theta + (D3DX_PI / 18.f) * -2.0f);
    m_pBodyPartTextureTransforms[BP_RWING]  ->Rotation({ 0.f, -1.f, 0.f }, _theta + (D3DX_PI / 18.f) * -6.5f);
    m_pBodyPartTextureTransforms[BP_LWING]  ->Rotation({ 0.f, -1.f, 0.f }, _theta + (D3DX_PI / 18.f) * +6.5f);
    m_pBodyPartTextureTransforms[BP_REAR]   ->Rotation({ 0.f, -1.f, 0.f }, _theta + (D3DX_PI / 18.f) * -2.0f);
    m_pBodyPartTextureTransforms[BP_LEAR]   ->Rotation({ 0.f, -1.f, 0.f }, _theta + (D3DX_PI / 18.f) * +2.0f);

    for (_uint i = 0; i < BP_END; i++) {
        m_pBodyPartTextureTransforms[i]->Set_State(STATE::POSITION, m_pBodyPartPivotTransforms[i]->Get_WorldState(STATE::POSITION));
        if (i == BP_LWING || i == BP_RWING || i == BP_LLEG || i == BP_RLEG || i == BP_LEAR || i == BP_REAR) continue;
        BillBoard(m_pBodyPartTextureTransforms[i]);
    }
}

void CMonster_Bat::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

void CMonster_Bat::Free()
{
    __super::Free();
    Safe_Release(m_pBodyTexture);
    Safe_Release(m_pWingTexture);

    Safe_Release(m_pBodyTransform);
    for (_uint i = 0; i < BP_END; i++) Safe_Release(m_pBodyPartPivotTransforms[i]);
    for (_uint i = 0; i < BP_END; i++) Safe_Release(m_pBodyPartTextureTransforms[i]);
}

HRESULT CMonster_Bat::Ready_Components()
{
    __super::Ready_Components();
    CHKFAIL(SetTransform());
    CHKFAIL(SetTexture());
    return S_OK;
}

void CMonster_Bat::OnCollision(FCollisionInfo _fCollisionInfo)
{
    if (_fCollisionInfo.strCollisionLayerTag == L"Player") static_cast<CToodee*>(_fCollisionInfo.pObject)->Set_IsDead();
    if (_fCollisionInfo.strCollisionLayerTag == L"Topdee") static_cast<CTopdee*>(_fCollisionInfo.pObject)->Set_IsDead();

    if (_fCollisionInfo.strCollisionLayerTag == L"Wall"     && m_eState == EDimensionState::TWODIM ||
        _fCollisionInfo.strCollisionLayerTag == L"FIX"      && m_eState == EDimensionState::TWODIM ||
        _fCollisionInfo.strCollisionLayerTag == L"KINETIC"  && m_eState == EDimensionState::TWODIM
        ) {
        auto _vPos = m_pTransformCom->Get_State(STATE::POSITION);
        if (_fCollisionInfo.collisionType == CCollider::CF_Left) {
            m_bIsRight = true;
            _vPos.x += _fCollisionInfo.vOverlapDepth.x;
        }
        if (_fCollisionInfo.collisionType == CCollider::CF_Right) {
            m_bIsRight = false;
            _vPos.x -= _fCollisionInfo.vOverlapDepth.x;
        }	
        if (_fCollisionInfo.collisionType == CCollider::CF_Top)		_vPos.y -= _fCollisionInfo.vOverlapDepth.y;
        if (_fCollisionInfo.collisionType == CCollider::CF_Bottom)	_vPos.y += _fCollisionInfo.vOverlapDepth.y;
        m_pTransformCom->Set_State(STATE::POSITION, _vPos);
    }
}
