#include "Monster_Pig.h"
#include "Collider.h"
#include "Transform.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

CMonster_Pig* CMonster_Pig::Create(LPDIRECT3DDEVICE9 pGraphic_Deivce)
{

    CMonster_Pig* pInstance = new CMonster_Pig(pGraphic_Deivce);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CMonster_Pig");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMonster_Pig::Clone(void* pArg)
{

    CMonster_Pig* pInstance = new CMonster_Pig(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CMonster_Pig");
        Safe_Release(pInstance);
    }

    return pInstance;


}

void CMonster_Pig::Move(_float _deltaTime)
{
    auto _vTargetPos = m_pTransformCom->Get_WorldState(STATE::POSITION) + ((m_bIsRight) ? 1 : -1) * m_fMoveSpeed * _deltaTime * _float3(1.f, 0.f, 0.f);
    _vTargetPos.y += m_fYSpeed * _deltaTime + 0.5f * m_fGravity * _deltaTime * _deltaTime;
    m_fYSpeed += m_fGravity * _deltaTime;

    m_pTransformCom->Set_State(STATE::POSITION, _vTargetPos);
}

HRESULT CMonster_Pig::TwoDimRender()
{

    if (FAILED(m_pToodeeTransform->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pToodeeTexture->Bind_AnimTexture()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Pig::TopDimRender()
{
    CHKFAIL(m_pVIBufferCom->Bind_Buffers())

    for (_uint i = 0; i < BP_END; i++) {
        CHKFAIL(m_pBodyPartTextureTransforms[i]->Bind_Resource())
        switch (i)
        {
        case Client::CMonster_Pig::BP_BODY:
            CHKFAIL(m_pTopdeeTexture->Bind_Texture(0))
            break;
        case Client::CMonster_Pig::BP_NOSE:
            CHKFAIL(m_pTopdeeTexture->Bind_Texture(4))
            break;
        case Client::CMonster_Pig::BP_TAIL:
            CHKFAIL(m_pTopdeeTexture->Bind_Texture(5))
            break;
        case Client::CMonster_Pig::BP_LEG:
            CHKFAIL(m_pLegTexture->Bind_AnimTexture())
            break;
        case Client::CMonster_Pig::BP_LEYE:
            CHKFAIL(m_pTopdeeTexture->Bind_Texture(2))
            break;
        case Client::CMonster_Pig::BP_REYE:
            CHKFAIL(m_pTopdeeTexture->Bind_Texture(3))
            break;
        case Client::CMonster_Pig::BP_LEAR:
        case Client::CMonster_Pig::BP_REAR:
            CHKFAIL(m_pTopdeeTexture->Bind_Texture(1))
            break;
        case Client::CMonster_Pig::BP_END:
            break;
        default:
            break;
        }
        CHKFAIL(m_pVIBufferCom->Render())
    }


    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0x10);
    // Texture Calc
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

    // Alpha Blend (Lower Transparency)

    m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(150, 0, 0, 0));

    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

    CHKFAIL(m_pTopdeeShadowTransform->Bind_Resource())
    CHKFAIL(m_pShadowTexture->Bind_Texture(0))
    CHKFAIL(m_pVIBufferCom->Render())
    

    return S_OK;
}

HRESULT CMonster_Pig::SetTexture()
{
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("kPrototype_Component_Texture_Pig_Toodee"),
        TEXT("Com_TextureToodee"), reinterpret_cast<CComponent**>(&m_pToodeeTexture))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_Pig_Body"),
        TEXT("Com_TextureBody"), reinterpret_cast<CComponent**>(&m_pTopdeeTexture))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_Pig_Leg"),
        TEXT("Com_TextureLeg"), reinterpret_cast<CComponent**>(&m_pLegTexture))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STAGE_STATIC), TEXT("Prototype_Component_Texture_Pig_Shadow"),
        TEXT("Com_TextureShadow"), reinterpret_cast<CComponent**>(&m_pShadowTexture))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster_Pig::SetTransform()
{

    CTransform::TRANSFORM_DESC		TransformDesc{};

    TransformDesc.parent = m_pTransformCom;
    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
        TEXT("Com_DetectorTransform"), reinterpret_cast<CComponent**>(&m_pDetectorTransform), &TransformDesc)))
        return E_FAIL;
    m_pDetectorTransform->SetParent(&TransformDesc);

    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
        TEXT("Com_ToodeeTransform"), reinterpret_cast<CComponent**>(&m_pToodeeTransform), &TransformDesc)))
        return E_FAIL;
    m_pToodeeTransform->Scale(26 * g_fDot, 28 * g_fDot, 1.f);
    m_pToodeeTransform->SetParent(&TransformDesc);

    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
        TEXT("Com_TopdeeTransform"), reinterpret_cast<CComponent**>(&m_pTopdeeTransform), &TransformDesc)))
        return E_FAIL;
    m_pTopdeeTransform->SetParent(&TransformDesc);

    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
        TEXT("Com_ShadowTransform"), reinterpret_cast<CComponent**>(&m_pTopdeeShadowTransform), &TransformDesc)))
        return E_FAIL;
    m_pTopdeeShadowTransform->SetParent(&TransformDesc);

    for (_uint i = 0; i < BP_END; i++)
    {
        if (i==BP_BODY) TransformDesc.parent = m_pTopdeeTransform;
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
    m_pTopdeeShadowTransform->              Scale(23 * g_fDot, 22 * g_fDot, 1.f);
    m_pToodeeTransform->                    Scale(26 * g_fDot, 28 * g_fDot, 1.f);
    m_pBodyPartTextureTransforms[BP_LEG]->  Scale(26 * g_fDot, 28 * g_fDot, 1.f);
    for (_uint i = 0; i < BP_END; i++) {
        if (i == BP_LEG) continue;
        m_pBodyPartTextureTransforms[i]->Scale(19 * g_fDot, 18 * g_fDot, 1.f);
    }

    //Adjust Location
    //m_pBodyPartPivotTransforms

    m_pTopdeeTransform->Rotation(_float3{ -1.f,0.f,0.f }, 4*(D3DX_PI / 18.f));
    _float _fR = 3.6f * g_fDot;
    //m_pBodyPartPivotTransforms[BP_BODY]->   Set_State(STATE::POSITION, _float3{ +0.0f * _fR, +0.0f * _fR, +0.0f * _fR });
    m_pBodyPartPivotTransforms[BP_LEG]->    Set_State(STATE::POSITION, _float3{ -0.0f * _fR, -0.1f * _fR, +0.0f * _fR });

    m_pBodyPartPivotTransforms[BP_NOSE]->   Set_State(STATE::POSITION, _float3{ +3.2f * _fR, +0.0f * _fR, +0.0f * _fR });
    m_pBodyPartPivotTransforms[BP_TAIL]->   Set_State(STATE::POSITION, _float3{ -3.5f * _fR, +0.0f * _fR, +0.0f * _fR });
    m_pBodyPartPivotTransforms[BP_LEYE]->   Set_State(STATE::POSITION, _float3{ +1.4f * _fR, +0.5f * _fR, -1.4f * _fR });
    m_pBodyPartPivotTransforms[BP_REYE]->   Set_State(STATE::POSITION, _float3{ +1.4f * _fR, +0.5f * _fR, +1.4f * _fR });
    m_pBodyPartPivotTransforms[BP_LEAR]->   Set_State(STATE::POSITION, _float3{ +0.5f * _fR, +1.1f * _fR, -1.5f * _fR });
    m_pBodyPartPivotTransforms[BP_REAR]->   Set_State(STATE::POSITION, _float3{ +0.5f * _fR, +1.1f * _fR, +1.5f * _fR });
    
    m_pTopdeeShadowTransform->Set_State(STATE::POSITION, _float3{ 1.5f * _fR, .5f * _fR, +0.48f });


    m_pDetectorTransform->Set_State(STATE::POSITION, _float3{ 13 * g_fDot, -14 * g_fDot, 0.f });
    return S_OK;
}

CMonster_Pig::CMonster_Pig(LPDIRECT3DDEVICE9 pGraphic_Deivce)
    : CMonster(pGraphic_Deivce, _float2{26 * g_fDot, 28 * g_fDot })
{
}

CMonster_Pig::CMonster_Pig(const CMonster_Pig& Prototype)
    : CMonster(Prototype)
{
}

HRESULT CMonster_Pig::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_Pig::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    m_pToodeeTexture->SetAnimationDuration(TOODEEANIMDURATION);
    m_pLegTexture->SetAnimationDuration(TOPDEEANIMDURATION);
    
    m_fChaseSpeed = 5.f;
    m_fMoveSpeed = 5.f;
    return S_OK;
}

void CMonster_Pig::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
    
    if (m_bNoFloor && m_fYSpeed>=-0.1f) {
        m_bIsRight ^= true;
    }

    if (!m_bIsRight && m_eState == EDimensionState::TWODIM) {
        if (!m_bFlip){
            m_pTransformCom->Scale(-1.f, 1.f, 1.f);
            m_bFlip = true;
        }
    }
    else {
        if (m_bFlip) {
            m_pTransformCom->Scale(-1.f, 1.f, 1.f);
            m_bFlip = false;
        }
    }
}

void CMonster_Pig::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);


    auto _theta = atan2(m_vChaseDir.y, m_vChaseDir.x);
    m_pBodyPartPivotTransforms[BP_BODY]->Rotation({0.f, -1.f, 0.f}, _theta);
    m_pBodyPartTextureTransforms[BP_TAIL]->Rotation({ 0.f, -1.f, 0.f }, _theta + (D3DX_PI / 18.f) * +0);
    m_pBodyPartTextureTransforms[BP_REAR]->Rotation({ 0.f, -1.f, 0.f }, _theta + (D3DX_PI / 18.f) * -2);
    m_pBodyPartTextureTransforms[BP_LEAR]->Rotation({ 0.f, -1.f, 0.f }, _theta + (D3DX_PI / 18.f) * +2);
    for (_uint i = 0; i < BP_END; i++) {
        m_pBodyPartTextureTransforms[i]->Set_State(STATE::POSITION, m_pBodyPartPivotTransforms[i]->Get_WorldState(STATE::POSITION));
        if (i == BP_TAIL || i == BP_REAR || i == BP_LEAR) continue;
        BillBoard(m_pBodyPartTextureTransforms[i]);
    }
}

void CMonster_Pig::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    m_bNoFloor = true;
}

void CMonster_Pig::Free()
{
    __super::Free();
    Safe_Release(m_pDetectorTransform);
    Safe_Release(m_pGroundCollidor);

    Safe_Release(m_pToodeeTexture);
    Safe_Release(m_pTopdeeTexture);
    Safe_Release(m_pShadowTexture);
    Safe_Release(m_pLegTexture);

    Safe_Release(m_pToodeeTransform);
    Safe_Release(m_pTopdeeTransform);
    Safe_Release(m_pTopdeeShadowTransform);
    for (_uint i = 0; i < BP_END; i++) Safe_Release(m_pBodyPartPivotTransforms[i]);
    for (_uint i = 0; i < BP_END; i++) Safe_Release(m_pBodyPartTextureTransforms[i]);
}

HRESULT CMonster_Pig::Ready_Components()
{
    __super::Ready_Components();

    CHKFAIL(SetTransform());
    CHKFAIL(SetTexture());

    /* Com_Collider */
    CCollider::FColliderDesc ColliderDesc{};
    ColliderDesc.bInitialActive = true;
    ColliderDesc.bIsTrigger = false;
    ColliderDesc.func = [=](FCollisionInfo _fCollisionInfo) { OnGroundCollision(_fCollisionInfo); };
    ColliderDesc.iLayerLevelIndex = 0;
    ColliderDesc.pObject = this;
    ColliderDesc.pTransform = m_pDetectorTransform;
    ColliderDesc.strCollisionLayerTag = L"Detector";

    FRectColliderInfo _collisionInfo;
    _collisionInfo.fHeight  = 1;
    _collisionInfo.fWidth   = 1;
    _collisionInfo.vCenter = _float2{ 0.f, 0.f };
    ColliderDesc.colliderInfo = _collisionInfo;

    if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Collider"),
        TEXT("Com_Collider1"), reinterpret_cast<CComponent**>(&m_pGroundCollidor), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

void CMonster_Pig::OnCollision(FCollisionInfo _fCollisionInfo)
{
    __super::OnCollision(_fCollisionInfo);

    if (_fCollisionInfo.strCollisionLayerTag == L"Wall" ||
        _fCollisionInfo.strCollisionLayerTag == L"FIX" ||
        _fCollisionInfo.strCollisionLayerTag == L"KINETIC"
        ) {
        if (_fCollisionInfo.collisionType == CCollider::CF_Bottom)	m_fYSpeed = 0.f;
    }

    if (_fCollisionInfo.strCollisionLayerTag == L"Wall"     && m_eState == EDimensionState::TWODIM ||
        _fCollisionInfo.strCollisionLayerTag == L"FIX"      && m_eState == EDimensionState::TWODIM ||
        _fCollisionInfo.strCollisionLayerTag == L"KINETIC"  && m_eState == EDimensionState::TWODIM
        ) {
        if (_fCollisionInfo.collisionType == CCollider::CF_Left && _fCollisionInfo.vOverlapDepth.x > 0.01f) {
            m_bIsRight = true;
        }
        if (_fCollisionInfo.collisionType == CCollider::CF_Right && _fCollisionInfo.vOverlapDepth.x > 0.01f) {
            m_bIsRight = false;
        }
    }
}

void CMonster_Pig::OnGroundCollision(FCollisionInfo _fCollisionInfo)
{

    if (_fCollisionInfo.strCollisionLayerTag == L"Wall" && m_eState == EDimensionState::TWODIM ||
        _fCollisionInfo.strCollisionLayerTag == L"FIX" && m_eState == EDimensionState::TWODIM ||
        _fCollisionInfo.strCollisionLayerTag == L"KINETIC" && m_eState == EDimensionState::TWODIM
        ) {
        
        m_bNoFloor = false;
    }
}

HRESULT CMonster_Pig::Reset()
{
    CHKFAIL(__super::Reset());
    m_fYSpeed = 0.f;
    return S_OK;
}
