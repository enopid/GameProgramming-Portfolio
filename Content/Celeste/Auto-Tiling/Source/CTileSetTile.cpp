#include "pch.h"
#include "CTileSetTile.h"

void CTileSetTile::SetTileSetIdx(pos<int> _tileSetIdx)
{
	m_TileSetIdx = _tileSetIdx;
	m_pSprite->SetCropOffset({ m_TileSetIdx.first * 8, m_TileSetIdx.second * 8 });
}

pos<int> CTileSetTile::GetTileSetIdx() const
{
	return m_TileSetIdx;
}

int CTileSetTile::GetTileType() const
{
	return m_iTileType;
}

void CTileSetTile::Initialize()
{
	CTile::Initialize();
}

void CTileSetTile::Update(double _dDeltaTime)
{
	CTile::Update(_dDeltaTime);
}

void CTileSetTile::LateUpdate(double _dDeltaTime)
{
	CTile::LateUpdate(_dDeltaTime); 
}

void CTileSetTile::Release()
{
	CTile::Release();
}

void CTileSetTile::CreateTileSprite()
{
	SAFE_DELETE(m_pSprite)
	auto _pSprite = new CStaticSprite;
	switch (m_iTileType) {
		case 0:
			_pSprite->Load(TILESETIMGPATH, L"CoreBGTileSet.png");
			break;
		case 1:
			_pSprite->Load(TILESETIMGPATH, L"CoreTileSet.png");
			break;
		case 2:
			_pSprite->Load(TILESETIMGPATH, L"DirtTileSet.png");
			break;
		default:
			_pSprite->Load(TILESETIMGPATH, L"template.png");
			break;
	}

	_pSprite->SetPivot({ 0.5, 0.5 });
	_pSprite->SetCroppedSize({ 8, 8 });
	_pSprite->SetInScreenSize({ m_iTileSize, m_iTileSize });
	_pSprite->SetCropOffset({ m_TileSetIdx.first * 8, m_TileSetIdx.second * 8});
	
	m_pSprite = _pSprite;
	GetComponent<CRendererComponent>()->SetSprite(m_pSprite);
}

void CTileSetTile::SetCollision()
{
	if (m_iTileType >= 1) {
		if (m_TileSetIdx.first >= 4) return;
		AddComponent<CCollisionComponent>();
		auto _pCollisionComponent = GetComponent<CCollisionComponent>();
		_pCollisionComponent->SetSize({ m_iTileSize, m_iTileSize });
		_pCollisionComponent->SetPivot({ 0.5, +0.5 });
		_pCollisionComponent->SetRenderable(true);
	}
}
