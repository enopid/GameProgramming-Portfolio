#include "Asset.h"
#include "GameInstance.h"
#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"
#include "CustomFont.h"

CAsset* CAsset::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto _pDesc = static_cast<ASSET_DESC*>(pArg)->m_eAssetType; 
	CAsset* pInstance = nullptr;
	switch (_pDesc)
	{
	case Engine::EASSETTYPE::TEXTURE:
		pInstance = new CTextureAsset(pDevice, pContext);
		break;
	case Engine::EASSETTYPE::VFX:
		pInstance = new CTextureAsset(pDevice, pContext);
		break;
	case Engine::EASSETTYPE::UITEXTURE:
		pInstance = new CTextureAsset(pDevice, pContext);
		break;
	case Engine::EASSETTYPE::MODEL:
		pInstance = new CModelAsset(pDevice, pContext);
		break;
	case Engine::EASSETTYPE::SOUND:
		break;
	case Engine::EASSETTYPE::FONT:
		pInstance = new CFontAsset(pDevice, pContext);
		break;
	default:
		break;
	}

	if (pInstance && FAILED(pInstance->Initialize(pArg)))
	{
		wstring _msg = L"Failed to Create : Asset";
		MSG_BOXL(_msg.c_str());
		if (pInstance) pInstance->Release();
	}

	return pInstance;	
}

CAsset::CAsset(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:	m_pDevice		(pDevice),
		m_pContext		(pContext),
		m_pGameInstance	(CGameInstance::GetInstance())
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
	Safe_AddRef(m_pGameInstance);
}

void CAsset::Release()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}

HRESULT CAsset::Initialize(void* pArg)
{
	auto _pDesc = static_cast<ASSET_DESC*>(pArg);
	
	m_eAssetType	= _pDesc->m_eAssetType;
	m_sAssetAddress = _pDesc->m_sAssetAddress;
	m_sAssetName	= _pDesc->m_sAssetName;
	m_iAssetCnt		= _pDesc->m_iAssetCnt;

	return S_OK;
}

//
// Texture Asset
//

CTextureAsset::CTextureAsset(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAsset(pDevice, pContext)
{

}


ID3D11ShaderResourceView* CTextureAsset::operator[](size_t idx)
{
	if (m_iAssetCnt <= idx) return nullptr;
	return m_vecSRVs[idx];
}

void CTextureAsset::Release()
{
	__super::Release();
	for (auto pSRV : m_vecSRVs) Safe_Release(pSRV);
}

HRESULT CTextureAsset::Initialize(void* pArg)
{
	CHKFAIL(__super::Initialize(pArg));

	for (size_t i = 0; i < m_iAssetCnt; i++)
	{
		ID3D11ShaderResourceView*	pSRV		= { nullptr };
		ID3D11Texture2D*			pTexture	= { nullptr };

		_tchar		szEXT[MAX_PATH] = {};

		_wsplitpath_s(m_sAssetAddress.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		_tchar		szFullPath[MAX_PATH] = {};
		wsprintf(szFullPath, m_sAssetAddress.c_str(), i);

		HRESULT		hr = {};

		if (false == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice, szFullPath, reinterpret_cast<ID3D11Resource**>(&pTexture), &pSRV);
		else if (false == lstrcmp(szEXT, TEXT(".DDS")))
			hr = CreateDDSTextureFromFile(m_pDevice, szFullPath, reinterpret_cast<ID3D11Resource**>(&pTexture), &pSRV);
		else if (false == lstrcmp(szEXT, TEXT(".tga")))
			return E_FAIL;
		else
			hr = CreateWICTextureFromFile(m_pDevice, szFullPath, reinterpret_cast<ID3D11Resource**>(&pTexture), &pSRV);

		if (FAILED(hr)) 
			return E_FAIL;

		TEXTURE_INFO _info;
		D3D11_TEXTURE2D_DESC _desc;
		pTexture->GetDesc(&_desc);
		_info.iHeight	= _desc.Height;
		_info.iWidth	= _desc.Width;
		_info.iFrame	= m_iAssetCnt;

		m_vecSRVs.push_back(pSRV);
		m_vecInfos.push_back(_info);

		Safe_Release(pTexture);
	}
	
	return S_OK;
}

//
// Texture Asset
//

CModelAsset::CModelAsset(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAsset(pDevice, pContext)
{
}

void CModelAsset::Release()
{
	__super::Release();
	for (auto& pAnimation : m_vecAnimations) Safe_Release(pAnimation);
	m_vecAnimations.clear();
}

HRESULT CModelAsset::Initialize(void* pArg)
{
	CHKFAIL(__super::Initialize(pArg));

	if (
		m_sAssetName.rfind(L"Enemy", 0)		== 0 ||
		m_sAssetName.rfind(L"Player", 0)		== 0 ||
		m_sAssetName.rfind(L"Partner",		0)	== 0 ||
		m_sAssetName.rfind(L"Weapon", 0)		== 0 ||
		m_sAssetName.rfind(L"NPC",	0)			== 0 
	)
		m_modelInfo.m_bIsCharacter = true;
	else
		m_modelInfo.m_bIsCharacter = false;

	CHKFAIL(LoadModel());
	//Model File Load + Add Mesh / MeshInfo

	return S_OK;
}

HRESULT CModelAsset::LoadModel()
{
	std::filesystem::path   _path = m_sAssetAddress;

	ios_base::openmode      _mode = ios::in | ios::binary;
	fstream _fs;
	_fs.open(_path, _mode);
	if (!_fs.is_open()) {
		wstring _sMSG = _path;
		_sMSG = L"Fail to create read file : ( " + _sMSG + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	Load(_fs);

	_fs.close();

	return S_OK;
}

HRESULT CModelAsset::Load(fstream& _fs)
{
	auto _sentinel = MODELHEADER;
	_fs.read((char*)&_sentinel, sizeof(MODELHEADER));
	if (_sentinel != MODELHEADER) {
		MSG_BOX("Fail to Load Asset : Invalid Model Header!");
		return E_FAIL;
	}

	_fs.read((char*)&m_modelInfo.iNumMeshs, sizeof(m_modelInfo.iNumMeshs));

	for (size_t i = 0; i < m_modelInfo.iNumMeshs; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, true, m_modelInfo.m_bIsCharacter);
		if (FAILED(pMesh->LoadMesh(_fs))) {
			Safe_Release(pMesh);
			return E_FAIL;
		}
		else {
			shared_ptr<CMesh> _spMesh(
				pMesh,
				[](CMesh* p) { Safe_Release(p); }
			);
			m_vecMeshs.push_back(_spMesh);
		}
	}

	_fs.read((char*)&_sentinel, sizeof(MODELFOOTER));
	if (_sentinel != MODELFOOTER) {
		MSG_BOX("Fail to Load Asset : Invalid Model Footer!");
		return E_FAIL;
	}
	m_iAssetCnt = m_modelInfo.iNumMeshs;

	if (_fs.peek() == EOF) {
		return S_OK;
	}
	m_modelInfo.m_bIsSkeletal = true;

	_fs.read((char*)&_sentinel, sizeof(BONEHEADER));
	if (_sentinel != BONEHEADER) {
		MSG_BOX("Fail to Load Asset : Invalid Bone Header!");
		return E_FAIL;
	}

	_fs.read((char*)&m_modelInfo.iRootBoneIdx, sizeof(m_modelInfo.iRootBoneIdx));
	m_modelInfo.iNumBones		 = LoadVector(_fs, m_vecBoneDescs);

	_fs.read((char*)&_sentinel, sizeof(BONEFOOTER));
	if (_sentinel != BONEFOOTER) {
		MSG_BOX("Fail to Load Asset : Invalid Bone Footer!");
		return E_FAIL;
	}

	_fs.read((char*)&_sentinel, sizeof(ANIMATIONHEADER));
	if (_sentinel != ANIMATIONHEADER) {
		MSG_BOX("Fail to Load Asset : Invalid Animation Header!");
		return E_FAIL;
	}

	_fs.read((char*)&m_modelInfo.iNumAnimations, sizeof(m_modelInfo.iNumAnimations));
	m_vecAnimations.clear();
	m_vecAnimations.reserve(m_modelInfo.iNumAnimations);
	for (size_t i = 0; i < m_modelInfo.iNumAnimations; i++)
	{
		m_vecAnimations.push_back(CAnimation::Create(_fs));
	}
	
	_fs.read((char*)&_sentinel, sizeof(ANIMATIONFOOTER));
	if (_sentinel != ANIMATIONFOOTER) {
		MSG_BOX("Fail to Load Asset : Invalid Animation Footer!");
		return E_FAIL;
	}

	return S_OK;
}

vector<BONE_DESC> CModelAsset::GetBoneDescs()
{
	return m_vecBoneDescs;
}

vector<class CAnimation*> CModelAsset::GetAnimations()
{
	vector<CAnimation*> _vecAnimations;
	for (auto pAnimation : m_vecAnimations) {
		_vecAnimations.push_back(pAnimation->Clone());
	}
	return _vecAnimations;
}

weak_ptr<CMesh> CModelAsset::operator[](size_t idx)
{
	if (m_iAssetCnt <= idx) return weak_ptr<CMesh>();
	return m_vecMeshs[idx];
}

//
//Font
//
CFontAsset::CFontAsset(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAsset(pDevice, pContext)
{
}

void CFontAsset::Release()
{
	__super::Release();
	Safe_Release(m_pFont);
}

HRESULT CFontAsset::Initialize(void* pArg)
{
	CHKFAIL(__super::Initialize(pArg))
	m_pFont = CCustomFont::Create(m_pDevice, m_pContext, m_sAssetAddress.c_str());

	return S_OK;
}

HRESULT CFontAsset::Draw(_wstring sText, const RECT& vRect, _fvector vColor, _float fScale, ETEXTALIGN eAlign, float fLineSpacing, _bool bWordWrap)
{
	m_pFont->Draw(sText, vRect, vColor, fScale, eAlign, fLineSpacing, bWordWrap);

	return S_OK;
}
