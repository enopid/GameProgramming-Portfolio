#pragma once
#include "Object.h"
class CBackground : public CObject
{
	inline static double BGSENSITIVITY[3] = { 0.6f, 0.7f, 0.8f };

public:
	CBackground(int _type) : CObject(), m_iBGType(_type) {
		m_pSprite = nullptr;
	};
	~CBackground() {};

	void Update(double _dDeltaTime) override;
	void	Initialize() override {
		CObject::Initialize();
		AddComponent<CRendererComponent>();
		CreateTileSprite();
	}

	void SetOffset();

	void	CreateTileSprite();

protected:
	int			m_iBGType;
	pos<int>	m_worldPos;
	CSprite*	m_pSprite;
	bool        m_bGimmicState;
	
};

