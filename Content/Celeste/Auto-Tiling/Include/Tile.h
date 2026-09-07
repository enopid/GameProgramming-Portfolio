#pragma once
#include "Object.h"
#include "StaticSprite.h"
#include "AnimationSprite.h"
#include "DynamicSprite.h"

class CTile : public CObject
{
public:
	CTile(pos<int> _tileIdx, int _tileSize) : CObject(), m_TileIdx(_tileIdx), m_iTileSize(_tileSize) {
		m_pSprite = nullptr;
	};
	~CTile() {};

	void	Initialize() override{
		CObject::Initialize();
		GetTransform()->SetPos({ m_TileIdx.first * m_iTileSize + (m_iTileSize >> 1), m_TileIdx.second * m_iTileSize + (m_iTileSize >> 1) });
		AddComponent<CRendererComponent>();
		CreateTileSprite();
		SetCollision();
	}

	virtual void	CreateTileSprite()		PURE;
	virtual void	SetCollision()			PURE;

	pos<int>    GetTileIdx() const;
protected:
	pos<int>	m_TileIdx;
	int			m_iTileSize;
	CSprite*	m_pSprite;
};

