#pragma once
#include "Object.h"
class CHair : public CObject
{
public:
	CHair();
	~CHair();

	void	Initialize() override;
	void	Update(double _dDeltaTime) override;
	void	LateUpdate(double _dDeltaTime) override;
	void	Release() override;
	void	SetScale(double _scale);
	void	SetReverse(bool* _bReverse);
	void	SetHairColor(D3DCOLOR _color);
public:
	void CreateHairSprite();
	vector<pair<double, double>> vecHairOffset;

	CSprite* m_pSprite;
};

