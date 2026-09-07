#include "Component.h"
#include "GameInstance.h"

#include "Transform.h"

CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice         { pDevice }
    , m_pContext        { pContext }
    , m_pGameInstance   { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent& Prototype)
    : m_pDevice         { Prototype.m_pDevice }
    , m_pContext        { Prototype.m_pContext }
    , m_sComPrototypeTag{ Prototype.m_sComPrototypeTag }
    , m_sComTag         { Prototype.m_sComTag }
    , m_bIsInitialAdd   { Prototype.m_bIsInitialAdd  }
    , m_pGameInstance   { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CComponent::Initialize_Prototype(void* pArg)
{
    return S_OK;
}

HRESULT CComponent::Initialize(void* pDesc)
{
    if (pDesc) {
        CHKFAIL(Load(pDesc))
    }

    return S_OK;
}

void CComponent::SetGameObject(CGameObject* _pGameObject)
{
    m_pOwnerObj = _pGameObject;
}

CGameObject* CComponent::GetGameObject()
{
    return m_pOwnerObj;
}

void CComponent::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}

void CComponent::SetName(const _wstring& sNewNameTag)
{
    m_sComTag = sNewNameTag;
}

void CComponent::SetTypeName(const _wstring& sNewPrototypeTag)
{
    m_sComPrototypeTag = sNewPrototypeTag;
}

void CComponent::SetInitialAdd()
{
    m_bIsInitialAdd = true;
}

_bool CComponent::IsActive()
{
    if (m_pOwnerObj)
        return m_pOwnerObj->IsSectionActive() && m_bIsActive;

    return m_bIsActive;
}

HRESULT CComponent::SaveComponent(fstream& _fs, const CComponent* _pComponent)
{
    _fs.write((char*)&COMPONENTHEADER, sizeof(COMPONENTHEADER));

    wchar_t m_sComPrototypeTag  [MAXCOMNAMESIZE];
    wchar_t m_sComTag           [MAXCOMNAMESIZE];
    _uint   _iSize;
    _char   _pDesc              [MAXDESCSIZE];

    lstrcpynW(m_sComPrototypeTag,   _pComponent->m_sComPrototypeTag.c_str(),  MAXCOMNAMESIZE);
    lstrcpynW(m_sComTag,            _pComponent->m_sComTag.c_str(),           MAXCOMNAMESIZE);

    CHKFAIL(_pComponent->Save(_pDesc, _iSize))
    if (MAXDESCSIZE < _iSize) {
        wstring _sText = L"desc size is too big : " + to_wstring(_iSize) + L"Byte";
        MSG_BOXL(_sText.c_str());
        return E_FAIL;
    }

    _fs.write((char*)m_sComPrototypeTag,    MAXCOMNAMESIZE * sizeof(wchar_t));
    _fs.write((char*)m_sComTag,             MAXCOMNAMESIZE * sizeof(wchar_t));
    _fs.write((char*)&(_pComponent->m_bIsInitialAdd), sizeof(_bool));
    _fs.write((char*)&_iSize,   sizeof(_iSize));
    _fs.write(_pDesc,           _iSize);

    _fs.write((char*)&COMPONENTFOOTER, sizeof(COMPONENTFOOTER));
    return S_OK;
}

HRESULT CComponent::LoadComponent(fstream& _fs, SPComponent& spComponent)
{
    auto _sentinel = COMPONENTHEADER;
    _fs.read((char*)&_sentinel, sizeof(COMPONENTHEADER));
    if (_sentinel != COMPONENTHEADER) {
        MSG_BOX("Fail to Load Component : Invalid Comp Header!");
        return E_FAIL;
    }

    wchar_t _sComPrototypeTag  [MAXCOMNAMESIZE];
    wchar_t _sComTag           [MAXCOMNAMESIZE];
    _bool   _bIsInitialAdd = false;
    _uint   _iSize;
    _char   _pDesc              [MAXDESCSIZE];
    _fs.read((char*)_sComPrototypeTag,      MAXCOMNAMESIZE * sizeof(wchar_t));
    _fs.read((char*)_sComTag,               MAXCOMNAMESIZE * sizeof(wchar_t));
    _fs.read((char*)&_bIsInitialAdd,         sizeof(_bool));
    _fs.read((char*)&_iSize,                sizeof(_uint));
    _fs.read((char*)_pDesc,                 _iSize);
    if (MAXDESCSIZE < _iSize) {
        wstring _sText = L"desc size is too big : " + to_wstring(_iSize) + L"Byte";
        MSG_BOXL(_sText.c_str());
        return E_FAIL;
    }

    _fs.read((char*)&_sentinel, sizeof(COMPONENTFOOTER));
    if (_sentinel != COMPONENTFOOTER) {
        MSG_BOX("Fail to Load Component : Invalid Comp Footer!");
        return E_FAIL;
    }

    //Add Prototype
    spComponent = CGameInstance::GetInstance()->Clone_ComponentPrototype(_sComPrototypeTag, _pDesc);
    if (!spComponent) {
        wstring _sMSG = L"Fail to find Component Prototype : ";
        _sMSG += _sComPrototypeTag;
        MSG_BOXL(_sMSG.c_str());
        return E_FAIL;
    }
    spComponent->Load(_pDesc);
    spComponent->m_sComTag            = _sComTag;
    spComponent->m_sComPrototypeTag   = _sComPrototypeTag;
    spComponent->m_bIsInitialAdd      = _bIsInitialAdd;

    return S_OK;
}
