#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAsset abstract
{
    friend class CAsset_Manager;
public:
    typedef struct tagAssetDesc {
        EASSETTYPE  m_eAssetType    = {};
        _wstring    m_sAssetAddress = {};
        _wstring    m_sAssetName    = {};
        _uint       m_iAssetCnt     = {0};
    } ASSET_DESC;
    
    static CAsset* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    _wstring GetAssetName() { return m_sAssetName; };
protected:
    CAsset(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CAsset() = default;

    virtual void Release()                  PURE;
    virtual HRESULT Initialize(void* pArg)  PURE;
protected:
    EASSETTYPE  m_eAssetType    = {};
    _wstring    m_sAssetAddress = {};
    _wstring    m_sAssetName    = {};
    _uint       m_iAssetCnt     = {};
protected:
    ID3D11Device*           m_pDevice       = { nullptr };
    ID3D11DeviceContext*    m_pContext      = { nullptr };
    class CGameInstance*    m_pGameInstance = { nullptr };
};

class ENGINE_DLL CTextureAsset final : public CAsset
{
    friend class CAsset;
public:
    typedef struct tagTextureInfo {
        _uint       iWidth   = { 0 };
        _uint       iHeight  = { 0 };
        _uint       iFrame   = { 0 };
    } TEXTURE_INFO;

private:
    CTextureAsset(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CTextureAsset() = default;

    virtual void Release()                  override;
    virtual HRESULT Initialize(void* pArg)  override;
public:
    TEXTURE_INFO GetInfo(_uint idx) { return m_vecInfos[idx]; }
    ID3D11ShaderResourceView* operator[] (size_t idx);
private:
    vector<ID3D11ShaderResourceView*>	m_vecSRVs;
    vector<TEXTURE_INFO>                m_vecInfos;
};

class ENGINE_DLL CFontAsset final : public CAsset
{
    friend class CAsset;
private:
    CFontAsset(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CFontAsset() = default;

    virtual void    Release()                  override;
    virtual HRESULT Initialize(void* pArg)  override;
public:
    HRESULT Draw(_wstring sText, const RECT& vRect, _fvector vColor, _float fScale, ETEXTALIGN eAlign, float fLineSpacing, _bool bWordWrap);
private:
    class CCustomFont*  m_pFont;
};

class ENGINE_DLL CModelAsset final : public CAsset
{
public:
    const static _uint MODELHEADER      = 0x900DCAFE;
    const static _uint MODELFOOTER      = 0xBAADCAFE;

    const static _uint BONEHEADER       = 0xF00DCAFE;
    const static _uint BONEFOOTER       = 0x5AADCAFE;

    const static _uint ANIMATIONHEADER  = 0xD009CAFE;
    const static _uint ANIMATIONFOOTER  = 0xCAA4CAFE;

    friend class CAsset;
public:
    // Mesh Num
    // BBSize
    // Animation Info
    typedef struct tagModelInfo {
        _bool       m_bIsSkeletal   = { false };
        _uint       iRootBoneIdx    = { 0 };
        _uint       iNumMeshs       = { 0 };
        _uint       iNumBones       = { 0 };
        _uint       iNumAnimations  = { 0 };
        _float3     vModelBoundingBoxSize = { 0.f, 0.f, 0.f };
        _bool       m_bIsCharacter  = false;
    } Model_INFO;

private:
    CModelAsset (ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CModelAsset() = default;

    virtual void    Release()               override;
    virtual HRESULT Initialize(void* pArg)  override;
    HRESULT         LoadModel();
    HRESULT         Load(fstream& _fs);
public:
    Model_INFO GetInfo() { return m_modelInfo; }
    vector<BONE_DESC>               GetBoneDescs();;
    vector<class CAnimation*>       GetAnimations();

    weak_ptr<class CMesh> operator[] (size_t idx);
private:
    vector<shared_ptr<class CMesh>>	m_vecMeshs;
    vector<BONE_DESC>	            m_vecBoneDescs;
    vector<class CAnimation*>		m_vecAnimations;
    Model_INFO                      m_modelInfo;
};


using SP_Asset          = shared_ptr<CAsset>;
using SP_ModelAsset     = shared_ptr<CModelAsset>;
using SP_TextureAsset   = shared_ptr<CTextureAsset>;
using WP_Asset          = weak_ptr<CAsset>;

NS_END


