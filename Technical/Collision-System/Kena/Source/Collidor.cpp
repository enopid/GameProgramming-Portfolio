#include "Collidor.h"
#include "GameInstance.h"
#include "Model.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "VIBuffer.h"
#include "Bone.h"
#include "ImGuizmo.h"
#include "Collision_Manager.h"

CCollidor::CCollidor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
    m_bIsUpdateComponent = true;
    m_iLateInitializePriority = 1;
}

CCollidor::CCollidor(const CCollidor& Prototype)
    :   CComponent(Prototype),
        m_desc(Prototype.m_desc)
{
    m_bIsUpdateComponent = true;
    m_pBounding[0] = m_pBounding[1] = m_pBounding[2] = nullptr;
    m_iLateInitializePriority = 1;
}

HRESULT CCollidor::SetBoneDrivenSetting(_uint idx)
{
    if (!m_wpModel.lock()) {
        auto wpComp = m_pOwnerObj->Get_Component<CModel>();
        m_wpModel = ConvertWPComponent<CModel>(wpComp);
    }
    switch (ECOLLIDER(m_desc.m_collidorDesc[idx].m_eCollidorType))
    {
    case Engine::ECOLLIDER::AABB:
        m_pBounding[idx] = CBounding_AABB::Create(&m_desc.m_collidorDesc[idx].m_boundingInfo.m_AABBDesc);
        break;
    case Engine::ECOLLIDER::OBB:
        m_pBounding[idx] = CBounding_OBB::Create(&m_desc.m_collidorDesc[idx].m_boundingInfo.m_OBBDesc);
        break;
    case Engine::ECOLLIDER::SPHERE:
        m_pBounding[idx] = CBounding_Sphere::Create(&m_desc.m_collidorDesc[idx].m_boundingInfo.m_SphereDesc);
        break;
    case Engine::ECOLLIDER::CAPSULE:
        m_pBounding[idx] = CBounding_Capsule::Create(&m_desc.m_collidorDesc[idx].m_boundingInfo.m_CapsuleDesc);
        break;
    case Engine::ECOLLIDER::NONE:
        break;
    case Engine::ECOLLIDER::MESH:
    case Engine::ECOLLIDER::END:
        MSG_BOX("Fail to create custom collidor : Invalid Collidor Type");
        return E_FAIL;
        break;
    default:
        break;
    }
    return S_OK;
}

HRESULT CCollidor::SetBakedSetting(_uint idx)
{
    if (!m_wpModel.lock()) {
        auto wpComp = m_pOwnerObj->Get_Component<CModel>();
        m_wpModel = ConvertWPComponent<CModel>(wpComp);
    }

    if (auto _spModel = m_wpModel.lock()) {
        if (_spModel->Get_NumMeshes() > m_desc.m_collidorDesc[idx].m_iMeshIdx && m_desc.m_collidorDesc[idx].m_iMeshIdx > 0) {
            auto& _vecVertices = _spModel->GetMeshRenderer(m_desc.m_collidorDesc[idx].m_iMeshIdx)->GetBuffer().lock()->GetVertices();
            switch (ECOLLIDER(m_desc.m_collidorDesc[idx].m_eCollidorType))
            {
            case Engine::ECOLLIDER::AABB:
                m_pBounding[idx] = CBounding_AABB::Create(_vecVertices);
                break;
            case Engine::ECOLLIDER::OBB:
                m_pBounding[idx] = CBounding_OBB::Create(_vecVertices);
                break;
            case Engine::ECOLLIDER::SPHERE:
                m_pBounding[idx] = CBounding_Sphere::Create(_vecVertices);
                break;
            case Engine::ECOLLIDER::CAPSULE:
                m_pBounding[idx] = CBounding_Sphere::Create(_vecVertices);
                break;
            case Engine::ECOLLIDER::NONE:
                break;
            case Engine::ECOLLIDER::MESH:
            case Engine::ECOLLIDER::END:
                MSG_BOX("Fail to bake collidor : Invalid Collidor Type");
                return E_FAIL;
                break;
            default:
                break;
            }
        }
    }
    else {
        MSG_BOX("Fail to bake collidor : Cant Find Model");
        return E_FAIL;
    }
    return S_OK;
}

HRESULT CCollidor::SetCustomSetting(_uint idx)
{
    switch (ECOLLIDER(m_desc.m_collidorDesc[idx].m_eCollidorType))
    {
    case Engine::ECOLLIDER::AABB:
        m_pBounding[idx] = CBounding_AABB::Create(&m_desc.m_collidorDesc[idx].m_boundingInfo.m_AABBDesc);
        break;
    case Engine::ECOLLIDER::OBB:
        m_pBounding[idx] = CBounding_OBB::Create(&m_desc.m_collidorDesc[idx].m_boundingInfo.m_OBBDesc);
        break;
    case Engine::ECOLLIDER::SPHERE:
        m_pBounding[idx] = CBounding_Sphere::Create(&m_desc.m_collidorDesc[idx].m_boundingInfo.m_SphereDesc);
        break;
    case Engine::ECOLLIDER::CAPSULE:
        m_pBounding[idx] = CBounding_Capsule::Create(&m_desc.m_collidorDesc[idx].m_boundingInfo.m_CapsuleDesc);
        break;
    case Engine::ECOLLIDER::NONE:
        break;
    case Engine::ECOLLIDER::MESH:
    case Engine::ECOLLIDER::END:
        MSG_BOX("Fail to create custom collidor : Invalid Collidor Type");
        return E_FAIL;
        break;
    default:
        break;
    }
    return S_OK;
}

void CCollidor::Update(float fDeltaTime)
{
    __super::Update(fDeltaTime);
    auto _worldMatrix = m_pOwnerObj->Get_MainTransform().lock()->GetWorldMatrix();
    for (size_t i = 0; i < 3; i++)
    {
        if (m_pBounding[i]) {
            m_pBounding[i]->Update(_worldMatrix);
            if (m_desc.m_collidorDesc->m_eBoundingSrcType == ENUM_TO_UINT(EBOUNDINGSOURCE::BONEDRIVEN)) {
                //본하나시 center만 조정
                //본2개일시 center 및 길이 및 방향
                
                m_pBounding[i]->UpdateWithBone(
                    m_wpModel.lock()->FindBoneWithIdx(m_desc.m_collidorDesc[i].m_iSrcBoneIdx),
                    m_wpModel.lock()->FindBoneWithIdx(m_desc.m_collidorDesc[i].m_iDstBoneIdx)
                );
            }

            if (in(m_desc.m_iCollidorIdx, { 1,2,3,17 }))    m_pBounding[i]->SetColor(Colors::DarkGreen);
            if (in(m_desc.m_iCollidorIdx, { 4,5,6 }))       m_pBounding[i]->SetColor(Colors::Red);
            if (in(m_desc.m_iCollidorIdx, { 10,11,12 }))    m_pBounding[i]->SetColor(Colors::Yellow);
            if (in(m_desc.m_iCollidorIdx, { 13,14,15 }))    m_pBounding[i]->SetColor(Colors::LightYellow);
            if (in(m_desc.m_iCollidorIdx, { 18,19,20 }))    m_pBounding[i]->SetColor(Colors::LightBlue);

            if (IsActive() || m_pGameInstance->GetCurrentLevelTag()==L"Level_Editor") m_pBounding[i]->AddDebugRenderCall(1 << (i + 1));
        }
    }
}

HRESULT CCollidor::Initialize_Prototype(void* pArg)
{
    CHKFAIL(__super::Initialize_Prototype(pArg))

    return S_OK;
}

HRESULT CCollidor::Initialize(void* pDesc)
{
    CHKFAIL(__super::Initialize(pDesc))
    return S_OK;
}

HRESULT CCollidor::LateInitialize()
{
    for (size_t i = 0; i < 3; i++) SetCollidor(i);

    m_pGameInstance->Add_Collidor(ConvertWPComponent<CCollidor>(m_wpComponent));
    return S_OK;
}

HRESULT CCollidor::SetCollidor(const size_t& i)
{

    Safe_Release(m_pBounding[i]);

    switch (EBOUNDINGSOURCE(m_desc.m_collidorDesc[i].m_eBoundingSrcType))
    {
    case Engine::EBOUNDINGSOURCE::CUSTOM:
        CHKFAIL(SetCustomSetting(i))
            break;
    case Engine::EBOUNDINGSOURCE::BONEDRIVEN:
        CHKFAIL(SetBoneDrivenSetting(i))
        break;
    case Engine::EBOUNDINGSOURCE::BAKED:
        CHKFAIL(SetBakedSetting(i))
            break;
    default:
        break;
    }
    if (m_pBounding[i]) m_pBounding[i]->SetLocalMatrix(&m_desc.m_collidorDesc[i].m_LocalMatrix);

    return S_OK;
}

const CCollidor::COLLIDOR_DESC* CCollidor::GetDesc()
{
    return &m_desc;
}

_vector CCollidor::Get_Center()
{
    return m_pBounding[0]->GetCenter();
}

_bool CCollidor::Check_Intersection(WPCollidor _wpCollidor, ETESTLVL eLevel)
{
    auto _pTargetBounding = _wpCollidor.lock()->m_pBounding;
    switch (eLevel)
    {
    case Engine::CCollidor::BROAD:
        if (!m_pBounding[2] || !_pTargetBounding[2]) return true;
        return m_pBounding[2]->Intersect(_pTargetBounding[2]);
        break;
    case Engine::CCollidor::MID:
        if (!m_pBounding[1] || !_pTargetBounding[1]) return true;
        return m_pBounding[1]->Intersect(_pTargetBounding[1]);
        break;
    case Engine::CCollidor::NARROW:
        if (!m_pBounding[0] || !_pTargetBounding[0]) return true;
        return m_pBounding[0]->Intersect(_pTargetBounding[0]);
        break;
    case Engine::CCollidor::END:
    default:return false;
        break;
    }
    return false;
}

void CCollidor::Compute_Manifolds(WPCollidor _wpCollidor)
{
    auto _pBounding = _wpCollidor.lock()->m_pBounding[0];
    
    if (_pBounding) {
        CONTACTMF_DESC _mfDesc = m_pBounding[0]->GetManifold(_pBounding);
        m_mapCurManifolds.insert({ _wpCollidor.lock().get(), _mfDesc});
    }
}

void CCollidor::Clear_Manifolds()
{
    m_mapPrevManifolds = move(m_mapCurManifolds);
    m_mapCurManifolds.clear();
}

void CCollidor::Set_Func(EKEYACTIONSTATE eState, function<void(const CONTACTMF_DESC&, CCollidor*)> func)
{
    if (!in(eState, { EKEYACTIONSTATE::ENTER, EKEYACTIONSTATE::STAY, EKEYACTIONSTATE::EXIT })) return;
    m_Funcs[ENUM_TO_UINT(eState)].push_back(func);
}

void CCollidor::Execute_Func()
{
    for (auto& [_pCollidor, _pMFDesc] : m_mapCurManifolds)
    {
        if (m_mapPrevManifolds.count(_pCollidor)) {
            for (auto& pFunc : m_Funcs[ENUM_TO_UINT(EKEYACTIONSTATE::STAY)]) pFunc(_pMFDesc, _pCollidor);
        }
        else {
            for (auto& pFunc : m_Funcs[ENUM_TO_UINT(EKEYACTIONSTATE::ENTER)]) pFunc(_pMFDesc, _pCollidor);
        }
    }
    for (auto& [_pCollidor, _pMFDesc] : m_mapPrevManifolds)
    {
        if (!m_mapCurManifolds.count(_pCollidor)) {
            for (auto& pFunc : m_Funcs[ENUM_TO_UINT(EKEYACTIONSTATE::EXIT)]) pFunc(_pMFDesc, _pCollidor);
        }
    }
    //Enter Prev에 있고 현재에만 존재
    //Stay  둘다 존재
    //Exit  Prev만 존재
}

void CCollidor::TestSolver()
{
    
    if (!in(m_desc.m_iCollidorIdx, {1,2,3, 17})) return;
    _vector _vDist = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    _vector _vCommonDist = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    int iCommonCount(0);
    for (auto& [_pCollidor, _desc] : m_mapCurManifolds) {
        _int _iColIdx       =  m_desc.m_iCollidorIdx;
        _int _iTargetColIdx = _pCollidor->GetDesc()->m_iCollidorIdx;
        if (!(in(_iColIdx, { 1,2,3, 17 }) && in(_iTargetColIdx, { 1,2,3, 17 }))) continue;
        
        _float fRatio = 1.f;
        if (_iColIdx == _iTargetColIdx) {
            fRatio = 0.5f;
            if (_iColIdx == 17) fRatio = 0.f;
        }
        else if (_iColIdx == 1  &&  in(_iTargetColIdx, {2})) fRatio = 0.f;
        else if (_iColIdx == 2  &&  in(_iTargetColIdx, {-1})) fRatio = 0.f;
        else if (_iColIdx == 3  &&  in(_iTargetColIdx, {1,2})) fRatio = 0.f;
        else if (_iColIdx == 17 &&  in(_iTargetColIdx, {1,2,3})) fRatio = 0.f;

        for (int i = 0; i < _desc.contactCount; i++)
        {
            if (_iTargetColIdx!=17)
                _vDist -= XMVector3Normalize(XMVectorSetY(XMLoadFloat4(&_desc.contacts[i].vNormal), 0.f)) * _desc.contacts[i].fPenetration * fRatio;
            else {
                _vCommonDist -= XMVector3Normalize(XMLoadFloat4(&_desc.contacts[i].vNormal)) * _desc.contacts[i].fPenetration * fRatio;
                iCommonCount++;
            }
                
        }
    }
    if (iCommonCount>1) _vCommonDist = _vCommonDist / iCommonCount;
    _vDist = XMVectorSetW(_vDist + _vCommonDist, 1.f);
    m_pOwnerObj->Get_MainTransform().lock()->Translate(_vDist, false);
}

_bool CCollidor::RayCast(RAY ray, _float& fOutDist)
{
    if (m_pBounding[0] && m_pBounding[0]->IntersectRay(ray, fOutDist)) return true;
    return false;
}

SPComponent CCollidor::Clone(void* pDesc)
{
    return CloneBase<CCollidor>(pDesc);
}

void CCollidor::Free()
{
    __super::Free();
    for (size_t i = 0; i < 3; i++) Safe_Release(m_pBounding[i]);

}

void CCollidor::Render_Inspector()
{
    string _sText ="None";
    if (m_desc.m_iCollidorIdx< CCollision_Manager::GetNames().size()) {
        _sText = CCollision_Manager::GetNames()[m_desc.m_iCollidorIdx];
    }
    ImGui::Text(_sText.c_str());
    
    ImGui::InputInt("CollisionIdx", &m_desc.m_iCollidorIdx);

    if (ImGui::CollapsingHeader("NarrowDesc")) {
        ImGui::PushID(0);
        Render_SingleCollidor(0);
        ImGui::PopID();
    }
    if (ImGui::CollapsingHeader("MidDesc")) {
        ImGui::PushID(1);
        Render_SingleCollidor(1);
        ImGui::PopID();
    }
    if (ImGui::CollapsingHeader("BroadDesc")) {
        ImGui::PushID(2);
        Render_SingleCollidor(2);
        ImGui::PopID();
    }
}

void CCollidor::Render_SingleCollidor(_uint idx)
{
    //Transform
    m_desc.m_collidorDesc[idx].m_LocalMatrix;
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&m_desc.m_collidorDesc[idx].m_LocalMatrix), matrixTranslation, matrixRotation, matrixScale);
    if (ImGui::Button("Clear")) {
        matrixScale[0] = matrixScale[1] = matrixScale[2] = 1.f;
        matrixRotation[0] = matrixRotation[1] = matrixRotation[2] = 0.f;
        matrixTranslation[0] = matrixTranslation[1] = matrixTranslation[2] = 0.f;
    }
    ImGui::InputFloat3("Position",  matrixTranslation);
    ImGui::InputFloat3("Scale",     matrixScale);
    ImGui::InputFloat3("Rotation",  matrixRotation);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(&m_desc.m_collidorDesc[idx].m_LocalMatrix));
    
    //Collidor Type
    static const char* arrCollidorTags[] = { "AABB", "OBB", "SPHERE", "CAPSULE", "MESH", "NONE" };

    if (ImGui::Combo("Collidor Type", &m_desc.m_collidorDesc[idx].m_eCollidorType, arrCollidorTags, 6)) {
        Safe_Release(m_pBounding[idx]);
        m_desc.m_collidorDesc[idx].m_boundingInfo.m_CapsuleDesc = CBounding_Capsule::CAPSULE_DESC {};
        SetCollidor(idx);
    }
    if (m_pBounding[idx]) { 
        m_pBounding[idx]->Render_Inspector(reinterpret_cast<CBounding::BOUNDING_DESC*>(&m_desc.m_collidorDesc[idx].m_boundingInfo));
    }

    //Source Type
    static const char* arrBoundingSrcTags[] = { "CUSTOM", "BONEDRIVEN", "BAKED" };
    if (ImGui::Combo("Source Type", &m_desc.m_collidorDesc[idx].m_eBoundingSrcType, arrBoundingSrcTags, 3)) {
        Safe_Release(m_pBounding[idx]);
        m_desc.m_collidorDesc[idx].m_boundingInfo.m_CapsuleDesc = CBounding_Capsule::CAPSULE_DESC{};
        SetCollidor(idx);
    }
    if      (m_desc.m_collidorDesc[idx].m_eBoundingSrcType == ENUM_TO_UINT(EBOUNDINGSOURCE::BONEDRIVEN)) {
        ImGui::PushID("Src");
        m_wpModel.lock()->Render_BoneCombo(&m_desc.m_collidorDesc[idx].m_iSrcBoneIdx);
        ImGui::PopID();
        ImGui::PushID("Dst");
        m_wpModel.lock()->Render_BoneCombo(&m_desc.m_collidorDesc[idx].m_iDstBoneIdx);
        ImGui::PopID();
    }
    else if (m_desc.m_collidorDesc[idx].m_eBoundingSrcType == ENUM_TO_UINT(EBOUNDINGSOURCE::BAKED)) {
        if (m_wpModel.lock()->Get_NumMeshes() > 0)
            if (ImGui::DragInt("MeshIdx", &m_desc.m_collidorDesc[idx].m_iMeshIdx, 1.f, 0.f, m_wpModel.lock()->Get_NumMeshes())) {
                Safe_Release(m_pBounding[idx]);
                m_desc.m_collidorDesc[idx].m_boundingInfo.m_CapsuleDesc = CBounding_Capsule::CAPSULE_DESC{};
                SetCollidor(idx);
            }
    }
}

HRESULT CCollidor::Save(void* _pDesc, _uint& _iSize) const
{
    static_assert(is_trivially_copyable_v<COLLIDOR_DESC>);
    _iSize = sizeof(COLLIDOR_DESC);
    memcpy(_pDesc, &m_desc, _iSize);

    return S_OK;
}

HRESULT CCollidor::Load(void* _pDesc)
{
    m_desc = *(reinterpret_cast<COLLIDOR_DESC*>(_pDesc));

    return S_OK;
}