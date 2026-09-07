#pragma once
#include "Object.h"
class CBoundary : public CObject
{
	constexpr static double ONSCREENBOUNDARYSPEED	= 15 * PIXEL;
	constexpr static double OFFSCREENBOUNDARYSPEED	= 60 * PIXEL;
	constexpr static double BUBBLEPOPCOOLDOWN		= 1.0f;
	constexpr static int	MAXBUBBLEPARTICLECNT		= 100;
	constexpr static int	MAXBUBBLEPOPPARTICLECNT		= 10;
public:
	CBoundary();
	~CBoundary();

	void	Initialize() override;
	void	Update(double _dDeltaTime) override;
	void	LateUpdate(double _dDeltaTime) override;
	void	Release() override;

private:
	void CreateSprite();
	void SetCollision();
	void OnCollision(FCollisionInfo _collisionInfo);
	double	m_curSpeed;
	
	void CreateBubble();

private:
	void SetKeyContext();
	UniqueKeyContext m_pKeyContext;
	bool m_bGimmicState;
};

