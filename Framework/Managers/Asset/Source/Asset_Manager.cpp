#include "Asset_Manager.h"
#include "GameInstance.h"

CAsset_Manager::CAsset_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :   m_pDevice(pDevice),
        m_pContext(pContext),
        m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(pDevice);
    Safe_AddRef(pContext);
    Safe_AddRef(m_pGameInstance);
}

//Input_Manager
HRESULT CAsset_Manager::Initialize()
{
    wstring path;    
    filesystem::path root;
    PrintDividor(L"Start to Initialize Resource Manager");
    root = m_pGameInstance->ReadConfig(L"Asset.Paths.TexturePath", &path);
    InitTextureTable(root);

    root = m_pGameInstance->ReadConfig(L"Asset.Paths.VFXPath", &path);
    InitVFXTextureTable(root);

    root = m_pGameInstance->ReadConfig(L"Asset.Paths.UITexturePath", &path);
    InitUITextureTable(root);

    root = m_pGameInstance->ReadConfig(L"Asset.Paths.ModelPath", &path);
    InitModelTable(root);

    root = m_pGameInstance->ReadConfig(L"Asset.Paths.FontPath", &path);
    InitFontTable(root);

    PrintDividor(L"Initialize Resource Manager Successfully");
    return S_OK;
}

void CAsset_Manager::InitTextureTable(std::filesystem::path& root)
{
    PrintDividor(L"Start to Add Texture Files in Asset Table");
    CAsset::ASSET_DESC _desc;
    _uint iNumTextures(0);
    for (const auto& entry : filesystem::recursive_directory_iterator(root))
    {
        auto _ext = entry.path().extension();
        if (in(_ext, { L".png", L".jpg", L".dds", L".DDS" })) {
            _desc.m_eAssetType = EASSETTYPE::TEXTURE;
            ReplaceAddressDigit(entry.path(), _desc.m_sAssetAddress, _desc.m_sAssetName);
            _desc.m_iAssetCnt = 1;

            auto it = m_AssetDescTable.find(_desc.m_sAssetName);
            if (it != m_AssetDescTable.end()) {
                (it->second).m_iAssetCnt++;
            }
            else {
                PrintLog(L"Success to Add File in Asset Table : ", _desc.m_sAssetName);
                m_AssetDescTable.emplace(_desc.m_sAssetName, _desc);
                iNumTextures++;
            }
        }
    }
    PrintLog(L"Add ", iNumTextures, " Texture Files in Asset Table Successfully");
    PrintDividor(L"Add All Texture Files in Asset Table Successfully");
}
void CAsset_Manager::InitVFXTextureTable(std::filesystem::path& root)
{
    PrintDividor(L"Start to Add VFX Texture Files in Asset Table");
    CAsset::ASSET_DESC _desc;
    _uint iNumTextures(0);
    for (const auto& entry : filesystem::recursive_directory_iterator(root))
    {
        auto _ext = entry.path().extension();
        if (in(_ext, { L".png", L".jpg", L".dds", L".DDS" })) {
            _desc.m_eAssetType = EASSETTYPE::VFX;
            ReplaceAddressDigit(entry.path(), _desc.m_sAssetAddress, _desc.m_sAssetName);
            _desc.m_iAssetCnt = 1;

            auto it = m_AssetDescTable.find(_desc.m_sAssetName);
            if (it != m_AssetDescTable.end()) {
                (it->second).m_iAssetCnt++;
            }
            else {
                PrintLog(L"Success to Add File in Asset Table : ", _desc.m_sAssetName);
                m_AssetDescTable.emplace(_desc.m_sAssetName, _desc);
                iNumTextures++;
            }
        }
    }
    PrintLog(L"Add ", iNumTextures, "VFX  Texture Files in Asset Table Successfully");
    PrintDividor(L"Add All VFX Texture Files in Asset Table Successfully");
}
void CAsset_Manager::InitUITextureTable(std::filesystem::path& root)
{
    PrintDividor(L"Start to Add UI Texture Files in Asset Table");
    CAsset::ASSET_DESC _desc;
    _uint iNumTextures(0);
    for (const auto& entry : filesystem::recursive_directory_iterator(root))
    {
        auto _ext = entry.path().extension();
        if (in(_ext, { L".png", L".jpg", L".dds", L".DDS" })) {
            _desc.m_eAssetType = EASSETTYPE::UITEXTURE;
            ReplaceAddressDigit(entry.path(), _desc.m_sAssetAddress, _desc.m_sAssetName);
            _desc.m_iAssetCnt = 1;

            auto it = m_AssetDescTable.find(_desc.m_sAssetName);
            if (it != m_AssetDescTable.end()) {
                (it->second).m_iAssetCnt++;
            }
            else {
                PrintLog(L"Success to Add File in Asset Table : ", _desc.m_sAssetName);
                m_AssetDescTable.emplace(_desc.m_sAssetName, _desc);
                iNumTextures++;
            }
        }
    }
    PrintLog(L"Add ", iNumTextures, "UI  Texture Files in Asset Table Successfully");
    PrintDividor(L"Add All UI Texture Files in Asset Table Successfully");
}

void CAsset_Manager::InitModelTable(std::filesystem::path& root)
{
    PrintDividor(L"Start to Add Model Files in Asset Table");
    CAsset::ASSET_DESC _desc;
    _uint iNumModels(0);
    for (const auto& entry : filesystem::recursive_directory_iterator(root))
    {
        auto _ext = entry.path().extension();
        if (in(_ext, { L".MODEL"})) {
            _desc.m_eAssetType = EASSETTYPE::MODEL;
            _desc.m_sAssetAddress = entry.path();
            _desc.m_sAssetName = entry.path().filename().replace_extension();
            _desc.m_iAssetCnt = 1;

            auto it = m_AssetDescTable.find(_desc.m_sAssetName);
            if (it != m_AssetDescTable.end()) {
                PrintLog(L"Skip Duplicated File : ", _desc.m_sAssetName);
            }
            else {
                PrintLog(L"Success to Add File in Asset Table : ", _desc.m_sAssetName);
                m_AssetDescTable.emplace(_desc.m_sAssetName, _desc);
                iNumModels++;
            }
        }
    }
    PrintLog(L"Add ", iNumModels, " Texture Files in Asset Table Successfully");
    PrintDividor(L"Add All Model Files in Asset Table Successfully");
}

void CAsset_Manager::InitFontTable(std::filesystem::path& root)
{

    PrintDividor(L"Start to Add Font Files in Asset Table");
    CAsset::ASSET_DESC _desc;
    _uint iNumAddedModels   (0);
    _uint iNumSkippedModels (0);
    for (const auto& entry : filesystem::recursive_directory_iterator(root))
    {
        auto _ext = entry.path().extension();
        if (in(_ext, { L".spritefont" })) {
            _desc.m_eAssetType = EASSETTYPE::FONT;
            _desc.m_sAssetAddress   = entry.path();
            _desc.m_sAssetName      = entry.path().filename().replace_extension();
            _desc.m_iAssetCnt = 1;

            auto it = m_AssetDescTable.find(_desc.m_sAssetName);
            if (it != m_AssetDescTable.end()) {
                iNumSkippedModels++;
                PrintLog(L"Skip Duplicated File : ", _desc.m_sAssetName);
            }
            else {
                PrintLog(L"Success to Add File in Asset Table : ", _desc.m_sAssetName);
                m_AssetDescTable.emplace(_desc.m_sAssetName, _desc);
                iNumAddedModels++;
            }
        }
    }
    PrintLog(L"Skip ", iNumSkippedModels, " Font Files");
    PrintLog(L"Add ", iNumAddedModels, " Font Files in Asset Table Successfully");
    PrintDividor(L"Add All Font Files in Asset Table Successfully");
}

CAsset_Manager* CAsset_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAsset_Manager* pInstance = new CAsset_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CAsset_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAsset_Manager::ReplaceAddressDigit(const filesystem::path& InPath, _wstring& sOutPath, _wstring& sOutName)
{
    _wstring ext      = InPath.extension();
    _wstring fileName = InPath.stem();
    _wstring filePath = InPath.parent_path();

    int i = (int)fileName.size() - 1;

    while (i >= 0 && std::isdigit(fileName[i])) --i;

    if (i != (int)fileName.size() - 1) 
        fileName = fileName.substr(0, i + 1) + L"%d";
    
    sOutPath = filePath + L"/" + (fileName + ext);
    sOutName = fileName.substr(0, i + 1);
}


HRESULT CAsset_Manager::LoadAsset(SP_Asset& _pAsset, _wstring _sName)
{
    if (_sName.empty()) {
        _sName = L"Fail to Load Asset : Empty Asset Name (" + _sName + L")";
        MSG_BOXL(_sName.c_str());
        _pAsset = nullptr;
        return S_OK;
    }

    if (m_pAssetTable.find(_sName) == m_pAssetTable.end()) {
        auto it = m_AssetDescTable.find(_sName);
        if (it == m_AssetDescTable.end()) {
            _sName = L"Fail to Load Asset : Invalid Asset Name (" + _sName + L")";
            MSG_BOXL(_sName.c_str());
            return E_FAIL;
        }
        
        CHKFAIL(CreateAsset(_pAsset, it->second))
    }
    else {
        _pAsset = m_pAssetTable.find(_sName)->second.lock();
    }
    
    return S_OK;
}

void CAsset_Manager::Free() {
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}

HRESULT CAsset_Manager::CreateAsset(SP_Asset& _spAsset, CAsset::ASSET_DESC _assetDesc, _bool isStatic)
{
    _wstring    _sName      = _assetDesc.m_sAssetName;
    EASSETTYPE  _eType      = _assetDesc.m_eAssetType;

    CAsset* _pAsset = CAsset::Create(m_pDevice, m_pContext, &_assetDesc);
    
    _spAsset = shared_ptr<CAsset>( _pAsset, [=](CAsset* _pAsset) { 
        _pAsset->Release();
        m_pAssetTable.erase(_sName);
        delete _pAsset;
        }
    );

    WP_Asset _wpAsset = _spAsset;
    m_pAssetTable.emplace(_sName, _wpAsset);

    if (isStatic) m_pStaticAssets.push_back(_spAsset);

    return S_OK;
}

vector<_wstring> CAsset_Manager::GetAssetNames(EASSETTYPE eAssetType)
{
    vector<_wstring> vecNames;
    for (auto&[_name, _desc] : m_AssetDescTable)
    {
        if (_desc.m_eAssetType == eAssetType) vecNames.push_back(_name);
    }
    return vecNames;
}

