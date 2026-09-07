#include "VIBuffer_Terrain.h"
#include "GameInstance.h"
#include "Cell.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer ( pDevice, pContext )
{
	m_bIsUpdateComponent	= true;
	m_bIsDynamicBuffer		= true;
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
    :	CVIBuffer		( Prototype ),
		m_desc			( Prototype.m_desc),
		m_vecNormals	( Prototype.m_vecNormals)
{
	m_bIsUpdateComponent	= true;
	m_bIsDynamicBuffer		= true;
}

void CVIBuffer_Terrain::Update(float fDeltatTime)
{
	m_fDeltaTime = fDeltatTime;
	__super::Update(fDeltatTime);
	if (m_pGameInstance->GetCurrentLevelTag() == L"Level_Editor") {
		for (size_t i = 0; i < m_iNumCells; i++)
		{
			m_vecCells[i]->Render();
		}
	}
	if (!m_bIsDynamicBuffer) return;
	if (m_bIsHeightDirty) {
		CalcNormals();
		// ���� ����
		D3D11_MAPPED_SUBRESOURCE mapped;
		if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
			MSG_BOX("Fail to Map Dynamic Buffer");
			return;
		}

		VTXNORTEX* pVertices = reinterpret_cast<VTXNORTEX*>(mapped.pData);

		for (size_t i = 0; i < m_desc.m_iNumVerticesZ; i++) for (size_t j = 0; j < m_desc.m_iNumVerticesX; j++) {
			int iIndex = i * m_desc.m_iNumVerticesX + j;
			pVertices[iIndex].vPosition = m_vecVTXPositions[iIndex];
			pVertices[iIndex].vNormal = m_vecNormals[iIndex];
			pVertices[iIndex].vTexcoord = { (_float)j / (m_desc.m_iNumVerticesX - 1), (_float)i / (m_desc.m_iNumVerticesZ - 1) };
		}

		m_pContext->Unmap(m_pVB, 0);
		m_bIsHeightDirty = false;
	}

	if (m_bIsSplatDirty) {
		int rw = 2048;
		int rh = 2048;
		std::vector<uint8_t> block(rw * rh * 4);
		for (size_t i = 0; i < block.size(); i += 4)
		{
			block[i + 0] = clamp(_int(m_vecSplatMap[i/4].x * 255), 0, 255);
			block[i + 1] = clamp(_int(m_vecSplatMap[i/4].y * 255), 0, 255);
			block[i + 2] = clamp(_int(m_vecSplatMap[i/4].z * 255), 0, 255);
			block[i + 3] = clamp(_int(m_vecSplatMap[i/4].w * 255), 0, 255);
		}


		D3D11_BOX box{};
		box.left = 0;
		box.right = 2047;
		box.top = 0;
		box.bottom = 2047;
		box.front = 0;
		box.back = 1;

		m_pContext->UpdateSubresource(
			m_pSplatTexture, 0, &box,
			block.data(),
			rw * 4,         // row pitch
			0
		);
		m_bIsSplatDirty = false;
	}
}

HRESULT CVIBuffer_Terrain::Load_Asset()
{
	Reload();
	for (size_t i = 0; i < 4; i++) { m_pGameInstance->LoadAsset(m_spTileTextureAssets[i], m_desc.m_sSplatTileAssetName[i]); }
	LoadHeightMap(m_desc.m_sHeightMapAssetName);
	LoadSplatMap(m_desc.m_sSplatMapAssetName);
	for (auto pCell : m_vecCells) pCell->Reload();
	LoadNavMap(m_desc.m_sNavMapAssetName);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Reload()
{
	AdjustSize();
	CreateVB();
	CreateIB();
	CreateSplatTexture();
	return S_OK;
}

HRESULT CVIBuffer_Terrain::AdjustSize()
{
	m_iNumVertices	= m_desc.m_iNumVerticesX * m_desc.m_iNumVerticesZ;
	m_iNumIndices	= (m_desc.m_iNumVerticesX - 1) * (m_desc.m_iNumVerticesZ - 1) * 2 * 3;

	m_vecVTXPositions.resize(m_iNumVertices);
	m_vecNormals.resize		(m_iNumVertices, {0.f,1.f,0.f});
	m_vecIndices.resize		(m_iNumIndices);

	for (size_t i = 0; i < m_desc.m_iNumVerticesZ; i++) for (size_t j = 0; j < m_desc.m_iNumVerticesX; j++) {
		m_vecVTXPositions[i * m_desc.m_iNumVerticesX + j] = { j - (m_desc.m_iNumVerticesX - 1) * 0.5f, 0.f, i - (m_desc.m_iNumVerticesZ - 1) * 0.5f };
		m_vecVTXPositions[i * m_desc.m_iNumVerticesX + j].x *= m_fTileSize;
		m_vecVTXPositions[i * m_desc.m_iNumVerticesX + j].z *= m_fTileSize;
	}
		

	_uint iNumIndices = 0;
	for (size_t i = 0; i < m_desc.m_iNumVerticesZ - 1; i++) for (size_t j = 0; j < m_desc.m_iNumVerticesX - 1; j++) {
		_uint		iIndex = i * m_desc.m_iNumVerticesX + j;
		_uint		iIndices[4] = {
			iIndex + m_desc.m_iNumVerticesX,
			iIndex + m_desc.m_iNumVerticesX + 1,
			iIndex + 1,
			iIndex,
		};
		
		m_vecIndices[iNumIndices++] = iIndices[0];
		m_vecIndices[iNumIndices++] = iIndices[1];
		m_vecIndices[iNumIndices++] = iIndices[2];

		m_vecIndices[iNumIndices++] = iIndices[0];
		m_vecIndices[iNumIndices++] = iIndices[2];
		m_vecIndices[iNumIndices++] = iIndices[3];
	}

	//Cell
	m_iNumCells = (m_desc.m_iNumVerticesZ - 1) * (m_desc.m_iNumVerticesX - 1) * 2;
	m_vecNeighbourCellIndices.clear();
	m_vecNeighbourCellIndices.resize(m_iNumCells);
	for (_uint i = 0; i < m_iNumCells; i++)
	{
		if (i % 2) { //�Ʒ���
			m_vecNeighbourCellIndices[i][0] = i - 1;
			m_vecNeighbourCellIndices[i][1] = (i / (2 * (m_desc.m_iNumVerticesX - 1))) ? (i - 2 * m_desc.m_iNumVerticesX + 1) : -1;
			m_vecNeighbourCellIndices[i][2] = (i % (2 * (m_desc.m_iNumVerticesX - 1))) ? (i - 3) : -1;
		}
		else {
			m_vecNeighbourCellIndices[i][0] = ((i / (2 * (m_desc.m_iNumVerticesX - 1))) != m_desc.m_iNumVerticesZ - 1) ? (i + 2 * m_desc.m_iNumVerticesX - 1) : -1;
			m_vecNeighbourCellIndices[i][1] = ((i % (2 * (m_desc.m_iNumVerticesX - 1))) != 2 * (m_desc.m_iNumVerticesX - 1)-1) ? (i + 3) : -1;
			m_vecNeighbourCellIndices[i][2] = i + 1;
		}
	}
	for (size_t i = 0; i < m_vecCells.size(); i++) Safe_Release(m_vecCells[i]);
	m_vecCells.clear();
	m_vecCells.resize(m_iNumCells, nullptr);
	for (_uint i = 0; i < m_iNumCells; i++) {
		CCell::CELL_DESC _desc;
		_desc.m_iIndex			= i;
		_desc.m_iVertexIndices[0] = m_vecIndices[3 * i + 0];
		_desc.m_iVertexIndices[1] = m_vecIndices[3 * i + 1];
		_desc.m_iVertexIndices[2] = m_vecIndices[3 * i + 2];
		memcpy(_desc.m_iNeighbourCellIndices, m_vecNeighbourCellIndices[i].data(), sizeof(_desc.m_iNeighbourCellIndices));
		_desc.m_pVecVTXNormals		= &m_vecNormals;
		_desc.m_pVecVTXPositions	= &m_vecVTXPositions;
		m_vecCells[i] = CCell::Create(m_pDevice, m_pContext, &_desc);
	}
	return S_OK;
}

void CVIBuffer_Terrain::CalcNormals()
{
	_uint	iNumIndices = { 0 };
	m_vecNormals.resize(m_iNumVertices, { 0.f,0.f,0.f });
	while (iNumIndices < m_iNumIndices) {
		_uint indices[3];

		indices[0] = m_vecIndices[iNumIndices++];
		indices[1] = m_vecIndices[iNumIndices++];
		indices[2] = m_vecIndices[iNumIndices++];

		_float3 vNormal;
		_vector p0 = XMVectorSetW(XMLoadFloat3(&m_vecVTXPositions[indices[0]]), 1.f);
		_vector p1 = XMVectorSetW(XMLoadFloat3(&m_vecVTXPositions[indices[1]]), 1.f);
		_vector p2 = XMVectorSetW(XMLoadFloat3(&m_vecVTXPositions[indices[2]]), 1.f);

		XMStoreFloat3(&vNormal, XMVector3Normalize(XMVector3Cross(p1 - p0, p2 - p0)));

		for (_uint i = 0; i < 3; i++) {
			m_vecNormals[indices[i]].x += vNormal.x;
			m_vecNormals[indices[i]].y += vNormal.y;
			m_vecNormals[indices[i]].z += vNormal.z;
		}
	};

	for (size_t i = 0; i < m_iNumVertices; i++) XMStoreFloat3(&m_vecNormals[i], XMVector3Normalize(XMLoadFloat3(&m_vecNormals[i])));
}

HRESULT CVIBuffer_Terrain::CreateVB()
{
	Safe_Release(m_pVB);
	D3D11_BUFFER_DESC	VBDesc	= {};
	VBDesc.ByteWidth			= m_iVertexStride * m_iNumVertices;
	VBDesc.Usage				= (m_bIsDynamicBuffer) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	VBDesc.CPUAccessFlags		= (m_bIsDynamicBuffer) ? D3D11_CPU_ACCESS_WRITE : 0;
	VBDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	VBDesc.MiscFlags			= 0;
	VBDesc.StructureByteStride	= m_iVertexStride;

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];

	for (size_t i = 0; i < m_desc.m_iNumVerticesZ; i++) for (size_t j = 0; j < m_desc.m_iNumVerticesX; j++) {
		_uint index = i* m_desc.m_iNumVerticesX + j;
		pVertices[index].vPosition	= m_vecVTXPositions[index];
		pVertices[index].vNormal	= m_vecNormals[index];
		pVertices[index].vTexcoord  = { (_float)j / (m_desc.m_iNumVerticesX - 1), (_float)i / (m_desc.m_iNumVerticesZ - 1) };
	}

	D3D11_SUBRESOURCE_DATA	InitialVertexData{};
	InitialVertexData.pSysMem = pVertices;
	
	CHKFAIL(m_pDevice->CreateBuffer(&VBDesc, &InitialVertexData, &m_pVB))

	delete[] pVertices;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::CreateIB()
{
	Safe_Release(m_pIB);
	D3D11_BUFFER_DESC	IBDesc = {};
	IBDesc.ByteWidth			= m_iIndexStride * m_iNumIndices;
	IBDesc.Usage				= D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags		= 0;
	IBDesc.MiscFlags			= 0;
	IBDesc.StructureByteStride	= 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = {};

	for (size_t i = 0; i < m_iNumIndices; i++)
		pIndices[i] = m_vecIndices[i];


	D3D11_SUBRESOURCE_DATA	InitialIndexData{};
	InitialIndexData.pSysMem = pIndices;

	CHKFAIL((m_pDevice->CreateBuffer(&IBDesc, &InitialIndexData, &m_pIB)))

	delete[] pIndices;

	return S_OK;
}

_float CVIBuffer_Terrain::Compute_Height(_fvector vPosition)
{
	_uint iSizeX = (m_desc.m_iNumVerticesX - 1);
	_uint iSizeY = (m_desc.m_iNumVerticesZ - 1);

	auto _x = (XMVectorGetX(vPosition) + m_fTileSize * iSizeX * 0.5f) / m_fTileSize;
	auto _y = (XMVectorGetZ(vPosition) + m_fTileSize * iSizeY * 0.5f) / m_fTileSize;

	_uint iXIndice = clamp(_int(_x), 0, int(iSizeX) - 1);
	_uint iYIndice = clamp(_int(_y), 0, int(iSizeY) - 1);

	auto _dx = _x - iXIndice;
	auto _dy = _y - iYIndice;

	_uint iCellIndex = iXIndice * 2 + iYIndice * (iSizeX * 2);
	if (_dx + _dy < 1) iCellIndex++;

	return m_vecCells[iCellIndex]->Compute_Height(vPosition);
}

void CVIBuffer_Terrain::GetHeightNames()
{
	m_vecHeightMapNames.clear();
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.HeightMapPath");
	for (const auto& entry : filesystem::recursive_directory_iterator(_path))
	{
		m_vecHeightMapNames.push_back(entry.path().filename().replace_extension());
	}
}

HRESULT CVIBuffer_Terrain::SaveHeightMap(const wstring& sFileName)
{
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.HeightMapPath");
	std::filesystem::path   _tmpPath = _path;
	_path		+= sFileName + L".HEIGHT";
	_tmpPath	+= L"HeightSave";
	_tmpPath	+= L".tmp";

	ios_base::openmode      _mode = ios::out | ios::binary | ios::trunc;

	fstream _fs;
	_fs.open(_tmpPath, _mode);
	if (!_fs.is_open()) {
		wstring _sMSG = _path;
		_sMSG = L"Fail to create save file : ( " + _sMSG + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	if (m_iNumVertices != m_desc.m_iNumVerticesX * m_desc.m_iNumVerticesZ) {
		wstring _sMSG = _path;
		_sMSG = L"Fail to create save file : ( " + _sMSG + L" ) : Invalid vertice size";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}


	_fs.write((char*)&m_desc.m_iNumVerticesX,	sizeof(m_desc.m_iNumVerticesX));
	_fs.write((char*)&m_desc.m_iNumVerticesZ,	sizeof(m_desc.m_iNumVerticesZ));
	_fs.write((char*)&m_fTileSize,				sizeof(m_fTileSize));

	for (size_t i = 0; i < m_iNumVertices; i++) _fs.write((char*)&m_vecVTXPositions[i].y, sizeof(m_vecVTXPositions[i].y));

	_fs.close();

	filesystem::rename(_tmpPath, _path);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::LoadHeightMap(const wstring& sFileName)
{

	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.HeightMapPath");
	_path += sFileName + L".HEIGHT";

	ios_base::openmode      _mode = ios::in | ios::binary;

	fstream _fs;
	_fs.open(_path, _mode);
	if (!_fs.is_open()) {
		wstring _sMSG = _path;
		_sMSG = L"Fail to create save file : ( " + _sMSG + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}


	int _iNumVerticesX, _iNumVerticesZ;
	_float _fTIleSize;
	_fs.read((char*)&_iNumVerticesX, sizeof(m_desc.m_iNumVerticesX));
	_fs.read((char*)&_iNumVerticesZ, sizeof(m_desc.m_iNumVerticesZ));
	_fs.read((char*)&_fTIleSize, sizeof(m_fTileSize));

	if (_iNumVerticesX == m_desc.m_iNumVerticesX && _iNumVerticesZ == m_desc.m_iNumVerticesZ) {
		for (size_t i = 0; i < m_iNumVertices; i++) _fs.read((char*)&m_vecVTXPositions[i].y, sizeof(m_vecVTXPositions[i].y));
	}

	_fs.close();

	m_bIsHeightDirty = true;

	return S_OK;
}

void CVIBuffer_Terrain::GetSplatNames()
{
	m_vecSplatMapNames.clear();
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.SplatMapPath");
	for (const auto& entry : filesystem::recursive_directory_iterator(_path))
	{
		m_vecSplatMapNames.push_back(entry.path().filename().replace_extension());
	}
}

HRESULT CVIBuffer_Terrain::CreateSplatTexture()
{
	Safe_Release(m_pSplatTexture);
	Safe_Release(m_pSRV);

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width	= 2048;
	desc.Height = 2048;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;   // ���÷�
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	CHKFAIL(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pSplatTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	CHKFAIL(m_pDevice->CreateShaderResourceView(m_pSplatTexture, &srvDesc, &m_pSRV));
	m_vecSplatMap.clear();
	m_vecSplatMap.resize(2048 * 2048, _float4{1.f, 0.f, 0.f, 0.f});

	int rw = 2048;
	int rh = 2048;
	std::vector<uint8_t> block(rw * rh * 4);
	for (size_t i = 0; i < block.size(); i+=4)
	{
		block[i] = 255;
	}


	D3D11_BOX box{};
	box.left = 0;
	box.right = 2047;
	box.top = 0;
	box.bottom = 2047;
	box.front = 0;
	box.back = 1;

	m_pContext->UpdateSubresource(
		m_pSplatTexture, 0, &box,
		block.data(),
		rw * 4,         // row pitch
		0
	);
	return S_OK;
}

HRESULT CVIBuffer_Terrain::SaveSplatMap(const wstring& sFileName)
{
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.SplatMapPath");
	std::filesystem::path   _tmpPath = _path;
	_path += sFileName + L".SPLAT";
	_tmpPath += L"SplatSave";
	_tmpPath += L".tmp";

	ios_base::openmode      _mode = ios::out | ios::binary | ios::trunc;

	fstream _fs;
	_fs.open(_tmpPath, _mode);
	if (!_fs.is_open()) {
		wstring _sMSG = _path;
		_sMSG = L"Fail to create save file : ( " + _sMSG + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	for (size_t i = 0; i < 2048*2048; i++) _fs.write((char*)&m_vecSplatMap[i], sizeof(m_vecSplatMap[i]));

	_fs.close();

	filesystem::rename(_tmpPath, _path);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::LoadSplatMap(const wstring& sFileName)
{
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.SplatMapPath");
	_path += sFileName + L".SPLAT";

	ios_base::openmode      _mode = ios::in | ios::binary;

	fstream _fs;
	_fs.open(_path, _mode);
	if (!_fs.is_open()) {
		wstring _sMSG = _path;
		_sMSG = L"Fail to create load file : ( " + _sMSG + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	for (size_t i = 0; i < 2048 * 2048; i++) _fs.read((char*)&m_vecSplatMap[i], sizeof(m_vecSplatMap[i]));

	_fs.close();

	m_bIsSplatDirty = true;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(void* pArg)
{
	CHKFAIL(__super::Initialize_Prototype(pArg))

	m_iNumVertexBuffers = 1;
	m_iVertexStride		= sizeof(VTXNORTEX);
	m_iIndexStride		= 4;
	m_eIndexFormat		= DXGI_FORMAT_R32_UINT;
	m_ePrimitive		= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;



    return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	CHKFAIL(__super::Initialize(pArg))
    return S_OK;
}

HRESULT CVIBuffer_Terrain::LateInitialize()
{
	CHKFAIL(__super::LateInitialize())
	return S_OK;
}

void CVIBuffer_Terrain::GetTileNames()
{
	m_vecTileNames.clear();
	auto _vecNames = m_pGameInstance->GetAssetNames(EASSETTYPE::TEXTURE);
	for (auto sName : _vecNames) {
		if (sName.substr(0,5) == L"Tile_")  m_vecTileNames.push_back(sName);
	}
}

_float4 CVIBuffer_Terrain::GetSplatScale()
{
	_float4 vScale = m_desc.m_vSplatScale;
	vScale.x *= m_desc.m_iNumVerticesX;
	vScale.y *= m_desc.m_iNumVerticesX;
	vScale.z *= m_desc.m_iNumVerticesX;
	vScale.w *= m_desc.m_iNumVerticesX;
	return vScale;
}

ID3D11ShaderResourceView* CVIBuffer_Terrain::GetSplatMap()
{
	return m_pSRV;
}

void CVIBuffer_Terrain::GetNavNames()
{

	m_vecNavMapNames.clear();
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.NavMapPath");
	for (const auto& entry : filesystem::recursive_directory_iterator(_path))
	{
		m_vecNavMapNames.push_back(entry.path().filename().replace_extension());
	}
}

HRESULT CVIBuffer_Terrain::SaveNavMap(const wstring& sFileName)
{
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.NavMapPath");
	std::filesystem::path   _tmpPath = _path;
	_path += sFileName + L".NAV";
	_tmpPath += L"NAVSave";
	_tmpPath += L".tmp";

	ios_base::openmode      _mode = ios::out | ios::binary | ios::trunc;

	fstream _fs;
	_fs.open(_tmpPath, _mode);
	if (!_fs.is_open()) {
		wstring _sMSG = _path;
		_sMSG = L"Fail to create save file : ( " + _sMSG + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	_fs.write((char*)&m_iNumCells, sizeof(m_iNumCells));
	for (size_t i = 0; i < m_iNumCells; i++) {
		auto _iNavType = m_vecCells[i]->Get_Type();
		_fs.write((char*)&_iNavType, sizeof(_iNavType));
	}

	_fs.close();

	filesystem::rename(_tmpPath, _path);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::LoadNavMap(const wstring& sFileName)
{
	std::filesystem::path   _path = CGameInstance::GetInstance()->ReadConfig<wstring>(L"Asset.Paths.NavMapPath");
	_path += sFileName + L".NAV";

	ios_base::openmode      _mode = ios::in | ios::binary;

	fstream _fs;
	_fs.open(_path, _mode);
	if (!_fs.is_open()) {
		wstring _sMSG = _path;
		_sMSG = L"Fail to create load file : ( " + _sMSG + L" )";
		MSG_BOXL(_sMSG.c_str());
		return E_FAIL;
	}

	_uint _numCell;
	_fs.read((char*)&_numCell, sizeof(_numCell));
	if (_numCell != m_iNumCells) return E_FAIL;
	for (size_t i = 0; i < m_iNumCells; i++) {
		_int _iNavType;
		_fs.read((char*)&_iNavType, sizeof(_iNavType));
		m_vecCells[i]->Set_Type(CCell::ECELLTYPE(_iNavType));
	}

	_fs.close();

	return S_OK;
}

void CVIBuffer_Terrain::ApplyHeightBrush(_vector vBrushCenter, _uint iOperationType, _float fRadius, _float fSharpness, _float fStrength, _float fDir, _float fSlope)
{
	vector<pair<_uint, _float>> vecSmoothPts;
	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		_vector vVTXPos = XMLoadFloat3(&m_vecVTXPositions[i]);
		_vector vDeltaPos =  XMVectorSetY(vVTXPos - vBrushCenter, 0.f);
		auto _fDist = XMVectorGetX(XMVector3Length(vDeltaPos));
		
		auto _fRatio = max(1.f - _fDist / fRadius, 0.f);
		if (_fRatio <= 0.f) continue;
		_fRatio = pow(_fRatio, 1.f - fSharpness);

		switch (EOPERATIONTYPE(iOperationType))
		{
		case Engine::CVIBuffer_Terrain::EOPERATIONTYPE::RAISE:
			m_vecVTXPositions[i].y += fStrength * _fRatio;
			break;
		case Engine::CVIBuffer_Terrain::EOPERATIONTYPE::LOWER:
			m_vecVTXPositions[i].y -= fStrength * _fRatio;
			break;
		case Engine::CVIBuffer_Terrain::EOPERATIONTYPE::FLATTERN:
			m_vecVTXPositions[i].y = lerp(m_vecVTXPositions[i].y, fStrength, _fRatio);
			break;
		case Engine::CVIBuffer_Terrain::EOPERATIONTYPE::TARGETFLATTERN:
			m_vecVTXPositions[i].y = lerp(m_vecVTXPositions[i].y, XMVectorGetY(vBrushCenter), _fRatio);
			break;
		case Engine::CVIBuffer_Terrain::EOPERATIONTYPE::SMOOTH:
			{
				float avg		= 0.f;
				float avgCnt	= 0;
				_uint _x(i%m_desc.m_iNumVerticesX), _y(i/m_desc.m_iNumVerticesX);
				for (auto [_dx, _dy] : { pair{-1,-1}, pair{-1,0}, pair{-1,1}, pair{0,-1}, pair{0,1}, pair{1,-1}, pair{1,0}, pair{1,1} })
				{
					_uint _nx(_x + _dx), _ny(_y + _dy);
					if (_nx < 0 || _nx >= m_desc.m_iNumVerticesX || _ny < 0 || _ny >= m_desc.m_iNumVerticesZ) continue;
					avg += m_vecVTXPositions[_nx + _ny * m_desc.m_iNumVerticesX].y;
					avgCnt++;
				}
				if (avgCnt) {
					avg /= avgCnt;
					vecSmoothPts.push_back({ i, lerp(m_vecVTXPositions[i].y, avg, _fRatio) });
				}
			}
			break;
		case Engine::CVIBuffer_Terrain::EOPERATIONTYPE::TARGETSLOPE:
			//fAmount0�� ���� ����	[-HALF_PI,HALF_PI]
			//fAmount1�� ����			[0,2PI]
			{
				
				_vector _vDir = XMVectorSet(cos(XMConvertToRadians(fDir)), 0.f, sin(XMConvertToRadians(fDir)), 0.f);
				m_vecVTXPositions[i].y = lerp(
				m_vecVTXPositions[i].y, 
				XMVectorGetY(vBrushCenter) + XMVectorGetX(XMVector3Dot(_vDir, vDeltaPos) * tan(XMConvertToRadians(fSlope))),
				_fRatio);
			}
			break;
		case Engine::CVIBuffer_Terrain::EOPERATIONTYPE::END:
			break;
		default:
			break;
		}
	}
	for (auto& [_idx, _height] : vecSmoothPts) m_vecVTXPositions[_idx].y = _height;

	m_bIsHeightDirty = true;
}

void CVIBuffer_Terrain::ApplySplatBrush(_vector vBrushCenter, _uint iOperationType, _vector vWeight, _float fRadius, _float fSharpness, _float fStrength, _float fNoiseSrt, _float fNoiseDst)
{
	fStrength *= 0.05f;
	vector<pair<_uint, _vector>> vecSmoothPts;
	auto iSizeX = 2048;
	auto iSizeY = 2048;
	
	_vector vMinPos = vBrushCenter - XMVectorSet(fRadius, 0.f, fRadius, 0.f);
	_vector vMaxPos = vBrushCenter + XMVectorSet(fRadius, 0.f, fRadius, 0.f);
	int minX = floor((2 * XMVectorGetX(vMinPos) + (m_desc.m_iNumVerticesX-1) * m_fTileSize ) * iSizeX / ((m_desc.m_iNumVerticesX-1) * m_fTileSize * 2));
	int maxX = ceil ((2 * XMVectorGetX(vMaxPos) + (m_desc.m_iNumVerticesX-1) * m_fTileSize ) * iSizeX / ((m_desc.m_iNumVerticesX-1) * m_fTileSize * 2));
	int minZ = floor((2 * XMVectorGetZ(vMinPos) + (m_desc.m_iNumVerticesZ-1) * m_fTileSize ) * iSizeY / ((m_desc.m_iNumVerticesZ-1) * m_fTileSize * 2));
	int maxZ = ceil ((2 * XMVectorGetZ(vMaxPos) + (m_desc.m_iNumVerticesZ-1) * m_fTileSize ) * iSizeY / ((m_desc.m_iNumVerticesZ-1) * m_fTileSize * 2));
	minX = clamp(minX, 0, iSizeX - 1);
	maxX = clamp(maxX, 0, iSizeX - 1);
	minZ = clamp(minZ, 0, iSizeY - 1);
	maxZ = clamp(maxZ, 0, iSizeY - 1);

	for (size_t i = minX; i <= maxX; i++) for (size_t j = minZ; j <= maxZ; j++)
	{
		int iIndex = i + j * iSizeX;
		
		_vector vPixelPos	= XMVectorSet(
		((m_desc.m_iNumVerticesX-1) * m_fTileSize) * ((i + 0.5f) / (iSizeX - 1) - 0.5f),
		0.f,
		((m_desc.m_iNumVerticesZ-1) * m_fTileSize) * ((j + 0.5f) / (iSizeY - 1) - 0.5f),
		0.f);

		_vector vDeltaPos	= XMVectorSetY(vPixelPos - vBrushCenter, 0.f);
		auto _fDist			= XMVectorGetX(XMVector3Length(vDeltaPos));

		auto _fRatio = max(1.f - _fDist / fRadius, 0.f);
		if (_fRatio <= 0.f) continue;
		_fRatio = pow(_fRatio, 1.f - fSharpness);

		_vector _vPrevValue = XMLoadFloat4(&m_vecSplatMap[iIndex]);
		_vector _vNextValue = XMLoadFloat4(&m_vecSplatMap[iIndex]);
		switch (ESPLATOPERATIONTYPE(iOperationType))
		{
		case Engine::CVIBuffer_Terrain::ESPLATOPERATIONTYPE::RAISE:
			_vNextValue = _vPrevValue + vWeight * _fRatio * fStrength;
			break;
		case Engine::CVIBuffer_Terrain::ESPLATOPERATIONTYPE::LOWER:
			_vNextValue = _vPrevValue - vWeight * _fRatio * fStrength;
			break;
		case Engine::CVIBuffer_Terrain::ESPLATOPERATIONTYPE::ERASE:
			_vNextValue = _vPrevValue - vWeight * _fRatio * fStrength;
			break;
		case Engine::CVIBuffer_Terrain::ESPLATOPERATIONTYPE::SMOOTH:
		{
			_vector avg = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			float avgCnt = 0;
			_uint _x(i), _y(j);
			for (auto [_dx, _dy] : { pair{-1,-1}, pair{-1,0}, pair{-1,1}, pair{0,-1}, pair{0,1}, pair{1,-1}, pair{1,0}, pair{1,1} }) {
				_uint _nx(_x + _dx), _ny(_y + _dy);
				if (_nx < 0 || _nx >= iSizeX || _ny < 0 || _ny >= iSizeY) continue;
				avg += XMLoadFloat4(&m_vecSplatMap[_nx + _ny * iSizeX]);
				avgCnt++;
			}
			if (avgCnt) {
				avg /= avgCnt;
				_vNextValue = lerp(_vPrevValue, avg, _fRatio);
			}
		}
			break;
		}
		_vNextValue = XMVector4Normalize(XMVectorClamp(_vNextValue, XMVectorSet(0.01f, 0.01f, 0.01f, 0.01f), XMVectorSet(1.f, 1.f, 1.f, 1.f)));
		vecSmoothPts.push_back({ iIndex, _vNextValue });
	}
	for (auto& [_idx, _vWeight] : vecSmoothPts) {
		XMStoreFloat4(&m_vecSplatMap[_idx], _vWeight);
	}

	//Update
	int rw = maxX - minX + 1;
	int rh = maxZ - minZ + 1;
	if (minX >= maxX || minZ >= maxZ) return;
	std::vector<uint8_t> block(rw * rh * 4);

	for (size_t i = 0; i < rw; i++) for (size_t j = 0; j < rh; j++) {
		block[i * 4 + 0 + j * rw * 4] = min(m_vecSplatMap[minX + i + (minZ+j) * iSizeX].x * 256, 255);
		block[i * 4 + 1 + j * rw * 4] = min(m_vecSplatMap[minX + i + (minZ+j) * iSizeX].y * 256, 255);
		block[i * 4 + 2 + j * rw * 4] = min(m_vecSplatMap[minX + i + (minZ+j) * iSizeX].z * 256, 255);
		block[i * 4 + 3 + j * rw * 4] = min(m_vecSplatMap[minX + i + (minZ+j) * iSizeX].w * 256, 255);
	}

	D3D11_BOX box{};
	box.left	= minX;
	box.right	= maxX + 1;
	box.top		= minZ;
	box.bottom	= maxZ + 1;
	box.front	= 0;
	box.back	= 1;

	m_pContext->UpdateSubresource(
		m_pSplatTexture, 0, &box,
		block.data(),
		rw * 4,         // row pitch
		0
	);
}

void CVIBuffer_Terrain::ApplyNavBrush(_vector vBrushCenter, _uint iOperationType, _float fRadius)
{
	_uint iSizeX		= (m_desc.m_iNumVerticesX - 1);
	_uint iSizeY		= (m_desc.m_iNumVerticesZ - 1);

	auto _x = (XMVectorGetX(vBrushCenter) + m_fTileSize * iSizeX * 0.5f) / m_fTileSize;
	auto _y = (XMVectorGetZ(vBrushCenter) + m_fTileSize * iSizeY * 0.5f) / m_fTileSize;

	_uint iXIndice		= clamp(_int(_x), 0, int(iSizeX) - 1);
	_uint iYIndice		= clamp(_int(_y), 0, int(iSizeY) - 1);

	auto _dx = _x - iXIndice;
	auto _dy = _y - iYIndice;

	_uint iCellIndex = iXIndice * 2 + iYIndice * (iSizeX * 2);
	if (_dx + _dy < 1) iCellIndex++;
		
	m_vecCells[iCellIndex]->Set_Type(CCell::ECELLTYPE(iOperationType));
}

const vector<class CCell*>* CVIBuffer_Terrain::Get_Cell()
{
	return &m_vecCells;
}

SP_Asset CVIBuffer_Terrain::Get_TileAsset(_uint i)
{
	if (i > 4) return nullptr;
	return m_spTileTextureAssets[i];
}

SPComponent CVIBuffer_Terrain::Clone(void* pDesc)
{
	return CloneBase<CVIBuffer_Terrain>(pDesc);
}

void CVIBuffer_Terrain::Free()
{
    __super::Free();
	for (size_t i = 0; i < m_vecCells.size(); i++) Safe_Release(m_vecCells[i]);
	m_vecCells.clear();
	Safe_Release(m_pSplatTexture);
	Safe_Release(m_pSRV);
}

void CVIBuffer_Terrain::Render_Inspector()
{
	__super::Render_Inspector();

	GetHeightNames();
	GetSplatNames();
	GetNavNames();
	GetTileNames();
	
	//Adjust Size
	ImGui::Text("Size :");
	ImGui::SameLine(200); 
	ImGui::PushItemWidth(60.0f);
	ImGui::DragInt("X Size", &m_desc.m_iNumVerticesX, 1.f, 2, 100);
	ImGui::SameLine();
	ImGui::DragInt("Z Size", &m_desc.m_iNumVerticesZ, 1.f, 2, 100);
	ImGui::PopItemWidth();
	
	//ReBake
	if (ImGui::Button("Rebake")) {
		Reload();
	}

	//HeightMap
	ImGui::PushID(0);
	if (ImGui::Button("Load")) {
		LoadHeightMap(m_desc.m_sHeightMapAssetName);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		string _sName = m_filterHeightMap.InputBuf;
		SaveHeightMap(s2ws(_sName));
		lstrcpy(m_desc.m_sHeightMapAssetName, s2ws(_sName).c_str());
	}
	ImGui::SameLine();
	m_filterHeightMap.Draw("Search");
	if (ImGui::IsItemActive())									m_bHeightMapListOpen = true;
	if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) m_bHeightMapListOpen = false;
	if (m_bHeightMapListOpen) {
		for (auto name : m_vecHeightMapNames)
		{
			auto _sName = ws2s(name);
			if (!m_filterHeightMap.PassFilter(_sName.c_str()))
				continue;

			if (ImGui::Selectable(_sName.c_str()))
			{
				m_filterHeightMap.Clear();
				strcpy_s(m_filterHeightMap.InputBuf, _sName.c_str());
				m_filterHeightMap.Build();
				lstrcpy(m_desc.m_sHeightMapAssetName, s2ws(_sName).c_str());
				m_bHeightMapListOpen = false;
			}
		}
	}
	ImGui::PopID();
	//SplatMap
	ImGui::PushID(1);
	if (ImGui::Button("Load")) {
		LoadSplatMap(m_desc.m_sSplatMapAssetName);
		m_bIsSplatDirty = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		string _sName = m_filterSplatMap.InputBuf;
		SaveSplatMap(s2ws(_sName));
		lstrcpy(m_desc.m_sSplatMapAssetName, s2ws(_sName).c_str());
	}
	ImGui::SameLine();
	m_filterSplatMap.Draw("Search");
	if (ImGui::IsItemActive())									m_bSplatMapListOpen = true;
	if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) m_bSplatMapListOpen = false;
	if (m_bSplatMapListOpen) {
		for (auto name : m_vecSplatMapNames)
		{
			auto _sName = ws2s(name);
			if (!m_filterSplatMap.PassFilter(_sName.c_str()))
				continue;

			if (ImGui::Selectable(_sName.c_str()))
			{
				m_filterSplatMap.Clear();
				strcpy_s(m_filterSplatMap.InputBuf, _sName.c_str());
				m_filterSplatMap.Build();
				lstrcpy(m_desc.m_sSplatMapAssetName, s2ws(_sName).c_str());
				m_bSplatMapListOpen = false;
			}
		}
	}
	ImGui::PopID();
	//NavMap
	ImGui::PushID(2);
	if (ImGui::Button("Load")) {
		LoadNavMap(m_desc.m_sNavMapAssetName);
		m_bIsNavDirty = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		string _sName = m_filterNavMap.InputBuf;
		SaveNavMap(s2ws(_sName));
		lstrcpy(m_desc.m_sNavMapAssetName, s2ws(_sName).c_str());
	}
	ImGui::SameLine();
	m_filterNavMap.Draw("Search");
	if (ImGui::IsItemActive())									m_bNavMapListOpen = true;
	if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) m_bNavMapListOpen = false;
	if (m_bNavMapListOpen) {
		for (auto name : m_vecSplatMapNames)
		{
			auto _sName = ws2s(name);
			if (!m_filterNavMap.PassFilter(_sName.c_str()))
				continue;

			if (ImGui::Selectable(_sName.c_str()))
			{
				m_filterNavMap.Clear();
				strcpy_s(m_filterNavMap.InputBuf, _sName.c_str());
				m_filterNavMap.Build();
				lstrcpy(m_desc.m_sNavMapAssetName, s2ws(_sName).c_str());
				m_bNavMapListOpen = false;
			}
		}
	}
	ImGui::PopID();

	//Tile
	ImGui::PushID(2);
	for (size_t i = 0; i < 4; i++)
	{
		ImGui::PushID(i);
		m_filterTile[i].Draw("Search");
		if (ImGui::IsItemActive())									m_bTileListOpen[i] = true;
		if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered()) m_bTileListOpen[i] = false;
		if (m_bTileListOpen[i]) {
			for (auto name : m_vecTileNames)
			{
				auto _sName = ws2s(name);
				if (!m_filterTile[i].PassFilter(_sName.c_str()))
					continue;

				if (ImGui::Selectable(_sName.c_str()))
				{
					m_filterTile[i].Clear();
					strcpy_s(m_filterTile[i].InputBuf, _sName.c_str());
					m_filterTile[i].Build();
					lstrcpy(m_desc.m_sSplatTileAssetName[i], s2ws(_sName).c_str());
					m_pGameInstance->LoadAsset(m_spTileTextureAssets[i], m_desc.m_sSplatTileAssetName[i]);
					m_bTileListOpen[i] = false;
				}
			}
		}
		ImGui::PopID();
	}
	ImGui::PopID();
	ImGui::DragFloat4	("TileScale",	reinterpret_cast<_float*>(&m_desc.m_vSplatScale), 0.01f);
	ImGui::DragFloat	("Transition",	&m_desc.m_fHeightScale, 0.005f,0.001f, 0.99f);
}

HRESULT CVIBuffer_Terrain::Save(void* _pDesc, _uint& _iSize) const
{
	static_assert(is_trivially_copyable_v<TERRAIN_BUFFER_DESC>);

	_iSize = sizeof(TERRAIN_BUFFER_DESC);

	memcpy(_pDesc, &m_desc, _iSize);
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Load(void* _pDesc)
{
	m_desc = *reinterpret_cast<TERRAIN_BUFFER_DESC*>(_pDesc);
	for (size_t i = 0; i < 4; i++) {
		strcpy_s(m_filterTile[i].InputBuf, ws2s(m_desc.m_sSplatTileAssetName[i]).c_str());
		m_filterTile[i].Build();
	}
	strcpy_s(m_filterHeightMap.InputBuf,ws2s(m_desc.m_sHeightMapAssetName).c_str());
	m_filterHeightMap.Build();
	strcpy_s(m_filterSplatMap.InputBuf, ws2s(m_desc.m_sSplatMapAssetName).c_str());
	m_filterSplatMap.Build(); 
	m_bIsSplatDirty = true;
	strcpy_s(m_filterNavMap.InputBuf,	ws2s(m_desc.m_sNavMapAssetName).c_str());
	m_filterNavMap.Build();
	m_bIsNavDirty = true;

	return S_OK;
}
