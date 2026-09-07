#include "Bounding.h"
#include "Bone.h"
#include "GameInstance.h"

CBounding::CBounding()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CBounding::AddDebugRenderCall(_uint iDebugMask)
{
    return S_OK;
}

HRESULT CBounding::Render_Inspector(CBounding::BOUNDING_DESC* pDesc)
{
    return S_OK;
}

void CBounding::SetLocalMatrix(_float4x4* pMatrix)
{
    m_pLocalMatrix = pMatrix;
}

void CBounding::UpdateWithBone(CBone* pSrcBone, CBone* pDstBone)
{
    if (pSrcBone) {
        auto _pDesc = pSrcBone->GetDesc();
        auto m = XMLoadFloat4x4(&_pDesc.m_CombinedTransformationMatrix) * XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
        XMStoreFloat3(
            &m_vSrcBonePos,
            m.r[3]
        );
    }
    if (pDstBone) {
        auto _pDesc = pDstBone->GetDesc();
        auto m = XMLoadFloat4x4(&_pDesc.m_CombinedTransformationMatrix) * XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
        XMStoreFloat3(
            &m_vDstBonePos,
            m.r[3]
        );
    }
}

void CBounding::SetColor(XMVECTORF32 eColor)
{
    m_eColor = eColor;
}

void CBounding::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}

void CBounding::Update(_fmatrix WorldMatrix)
{
    XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}
