#include "Ozraeen_Part.h"
#include "Ozraeen.h"
#include "Transform.h"

COzraeen_Part::COzraeen_Part(LPDIRECT3DDEVICE9 pGraphic_Device) : CBoss_Part(pGraphic_Device)
{
}

COzraeen_Part::COzraeen_Part(const COzraeen_Part& Prototype) : CBoss_Part(Prototype)
{
}

HRESULT COzraeen_Part::SetParent(CBoss* ppGameObject)
{
    if (FAILED(__super::SetParent(ppGameObject))) return E_FAIL;
    m_pBaseTransform = static_cast<CTransform*>(ppGameObject->Get_Component(L"Com_Transform"));
    return S_OK;
}

HRESULT COzraeen_Part::SetParentTransform(CTransform* pTransformCom)
{
    if (!pTransformCom) return E_FAIL;
    m_pParentTransform = pTransformCom;
    return S_OK;
}

void COzraeen_Part::InheritPositon_Begin()
{
    //BillBoard();
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, true);

    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_vOffset = m_pParentTransform->Get_WorldState(STATE::POSITION);
    m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) + m_vOffset);
}

void COzraeen_Part::InheritPositon_End()
{
    m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION) - m_vOffset);
    m_vOffset = _float3{ 0.f,0.f, 0.f };

    m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void COzraeen_Part::BillBoard()
{

    _float4x4		ViewMatrix{};
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
    D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

    _float3		vMyScale = m_pTransformCom->Get_Scaled();

    m_pTransformCom->Set_State(STATE::RIGHT,    *reinterpret_cast<_float3*>(&ViewMatrix.m[0]));
    m_pTransformCom->Set_State(STATE::UP,       *reinterpret_cast<_float3*>(&ViewMatrix.m[1]));
    m_pTransformCom->Set_State(STATE::LOOK,     *reinterpret_cast<_float3*>(&ViewMatrix.m[2]));

    m_pTransformCom->Scale(vMyScale.x, vMyScale.y, vMyScale.z);
}

void COzraeen_Part::Flip()
{
    if(!m_bFlip) m_pTransformCom->Final_Scale(-1.f, 1.f, 1.f);
    m_bFlip = true;
}

