#pragma once

#include "VIBuffer.h"
#include "Asset.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
	friend class CComponent;
public:
	enum class EOPERATIONTYPE {
		RAISE,
		LOWER,
		FLATTERN,
		TARGETFLATTERN,
		SMOOTH,
		TARGETSLOPE,
		END
	};
	enum class ESPLATOPERATIONTYPE {
		RAISE,
		LOWER,
		ERASE,
		SMOOTH,
		END
	};

	typedef struct tagTerrainBufferDesc
	{
		//scale
		int		m_iNumVerticesX = { 2 };
		int		m_iNumVerticesZ = { 2 };
		
		//Height
		_tchar		m_sHeightMapAssetName[60] = L"";
		float		m_fHeightScale = {1000.f};

		//Splat Map
		_tchar		m_sSplatMapAssetName	[60] = L"";
		_tchar		m_sSplatTileAssetName[4][60]	= { L"" , L"" , L"" , L"" };
		_float4		m_vSplatScale					= { 1.f, 1.f, 1.f, 1.f };
		//Nav
		_tchar		m_sNavMapAssetName[60] = L"";
	} TERRAIN_BUFFER_DESC;
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype);
	virtual ~CVIBuffer_Terrain() = default;

	void Update(float fDeltatTime) override;
	virtual HRESULT Load_Asset() override;
	HRESULT Reload();

	HRESULT AdjustSize();
	void	CalcNormals();

	HRESULT CreateVB();
	HRESULT CreateIB();

public:
	_float Compute_Height(_fvector vPosition);

	void	GetHeightNames();
	HRESULT SaveHeightMap(const wstring& sFileName);
	HRESULT LoadHeightMap(const wstring& sFileName);

	void	GetSplatNames();
	HRESULT CreateSplatTexture	();
	HRESULT UpdateSplatTex		();
	HRESULT SaveSplatMap(const wstring& sFileName);
	HRESULT LoadSplatMap(const wstring& sFileName);

	void	GetTileNames();
	_float4 GetSplatScale();
	_float GetTrasitionScale() { return m_desc.m_fHeightScale; };
	ID3D11ShaderResourceView* GetSplatMap();

	void	GetNavNames();
	HRESULT SaveNavMap(const wstring& sFileName);
	HRESULT LoadNavMap(const wstring& sFileName);

	void	ApplyHeightBrush(_vector vBrushCenter, _uint iOperationType, _float fRadius, _float fSharpness, _float fStrength, _float fDir = 0.f, _float fSlope = 0.f);
	void	ApplySplatBrush	(_vector vBrushCenter, _uint iOperationType, _vector vWeight, _float fRadius, _float fSharpness, _float fStrength, _float fAmount0 = 0.f, _float fAmount1 = 0.f);
	void	ApplyNavBrush	(_vector vBrushCenter, _uint iOperationType, _float fRadius);
	const vector<class CCell*>* Get_Cell();
	SP_Asset	Get_TileAsset(_uint i);
public:
	virtual HRESULT Initialize_Prototype(void* pArg)	override;
	virtual HRESULT Initialize			(void* pDesc)	override;
	virtual HRESULT LateInitialize()				override;
	

private:
	TERRAIN_BUFFER_DESC	m_desc = {};
	_bool				m_bIsDynamicBuffer	= { false };
	_bool				m_bIsHeightDirty	= { false };
	_bool				m_bIsSplatDirty		= { false };
	_bool				m_bIsNavDirty		= { false };
	vector<_float3>		m_vecNormals;

	vector <_wstring>	m_vecHeightMapNames;
	vector <_wstring>	m_vecSplatMapNames;
	vector <_wstring>	m_vecNavMapNames;
	vector <_wstring>	m_vecTileNames;

	ImGuiTextFilter		m_filterHeightMap;
	_bool				m_bHeightMapListOpen = false;
	ImGuiTextFilter		m_filterSplatMap;
	_bool				m_bSplatMapListOpen = false;
	ImGuiTextFilter		m_filterNavMap;
	_bool				m_bNavMapListOpen = false;
	ImGuiTextFilter		m_filterTile[4];
	_bool				m_bTileListOpen[4] = { false, false, false, false };

	_float				m_fTileSize = 16.f;
	_float				m_fDeltaTime = 0.f;

	SP_Asset			m_spTileTextureAssets[4] = { nullptr,nullptr, nullptr, nullptr };
public:
	virtual SPComponent Clone(void* pDesc)override;
	virtual void Free() override;

	void	Render_Inspector()						override;
	HRESULT Save(void* _pDesc, _uint& _iSize) const	override;
	HRESULT Load(void* _pDesc)						override;
private:
	_uint					m_iNumCells = 0;
	vector<array<int,3>>	m_vecNeighbourCellIndices;
	vector<class CCell*>	m_vecCells;

	vector<_float4>				m_vecSplatMap;
	ID3D11Texture2D*			m_pSplatTexture = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;
};

NS_END