#pragma once

#include "Asset.h"
#include "Base.h"


//sound
//texture
//mesh
//Font
//Animation
//UI

NS_BEGIN(Engine)

class ENGINE_DLL CAsset_Manager : public CBase
{
    inline static wstring TEXTUREFOLDERPATH = L"../Bin/Resources/Textures/";
private:
    CAsset_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CAsset_Manager()   = default;

public:
    HRESULT Initialize();
    static CAsset_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    void ReplaceAddressDigit(const filesystem::path& sInPath, _wstring& sOutPath, _wstring& sOutName);
public:
    HRESULT LoadAsset   (SP_Asset& _spAsset, _wstring  _sName);
    vector<_wstring> GetAssetNames(EASSETTYPE eAssetType);
    virtual void Free() override;
private:
    void InitTextureTable   (std::filesystem::path& root);
    void InitVFXTextureTable(std::filesystem::path& root);
    void InitUITextureTable(std::filesystem::path& root);
    void InitModelTable     (std::filesystem::path& root);
    void InitFontTable      (std::filesystem::path& root);
    HRESULT CreateAsset (SP_Asset& _spAsset, CAsset::ASSET_DESC _assetDesc, _bool isStatic = false);
private:
    ID3D11Device*           m_pDevice       = { nullptr };
    ID3D11DeviceContext*    m_pContext      = { nullptr };
    class CGameInstance*    m_pGameInstance = { nullptr };
private:
    map<_wstring, CAsset::ASSET_DESC>   m_AssetDescTable;
    map<_wstring, WP_Asset>             m_pAssetTable;
    list<SP_Asset>                      m_pStaticAssets;
};

NS_END