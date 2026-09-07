#pragma once

#include "Object.h"

#include "../ServerCore/Protocol/CelesteProtocol.h"

#include <memory>
#include <array>

class CDynamicSprite;
class CBang;
class CStar;

class CRemotePlayer : public CObject
{
	constexpr static D3DCOLOR RemoteBodyColor = D3DCOLOR_ARGB(190, 100, 220, 255);
	constexpr static D3DCOLOR RemoteHairColor = D3DCOLOR_ARGB(220, 255, 129, 255);
	constexpr static float FRENDERDELAY = 6 * FRAME; //100ms

	enum EPlayerState {
		PS_NONE,
		PS_ONGROUND,
		PS_ONAIR,
		PS_ONWALL,
		PS_ONDASH,
		PS_ONWALLJUMP,
		PS_ONDEAD,
		PS_ONBOOST,
		PS_ONFEATHER,
		PS_END
	};
public:
	CRemotePlayer();
	~CRemotePlayer();

	struct FSnapShot {
		pos<double>					m_curVelocity	= { 0.0,0.0};
		pos<int>					m_posNetwork	= { 0,	0  };
		double						m_dElapsed		= 0.0;
	};

public://network
	void ApplyNetworkState(const PlayerStateData& state);
	double GetSnapshotElapsedMs() const { return m_SnapShots.first.m_dElapsed * 1000.0; }
	bool IsUsingDeadReckoning() const { return m_bUsingDeadReckoning; }
private:
	void DeadReckoning(double _dDeltaTime);
	void InterpolatePredict(double _dDeltaTime);
	void SnapShot();
private:
	void Move();
	bool UnitMove(int _sx, int _sy);
public://object
	void	Initialize	()						override;
	void	Update		(double _dDeltaTime)	override;
	void	LateUpdate	(double _dDeltaTime)	override;
	void	Release		()						override;
private:
	void				SetCollision();
	void				SetHair();
	void				SetPlayerSprite();
	CDynamicSprite*		CreatePlayerGroundSprite();
	CDynamicSprite*		CreatePlayerWallSprite();
	CDynamicSprite*		CreatePlayerAirSprite();
	CSprite*			CreatePlayerBoostSprite();
	CSprite*			CreatePlayerDeadSprite();
private:
	class CSprite*				m_pSprite		= nullptr;
	CBang*						m_pHair			= nullptr;

	EPlayerState				m_PlayerState;
	pos<int>					m_curDirection;

	pos<double>					m_curAcceleration;
	pos<double>					m_curVelocity;
	pos<double>					m_RemainderPos;

	pos<int>					m_posRender			= { 0, 0 };
	pos<int>					m_posNetwork		= { 0, 0 };
	pos<int>					m_posPredict		= { 0, 0 };
	pos<FSnapShot>				m_SnapShots;
	float						m_fRTT = 0.0;

	float						m_fCorrectionSpeed	= 10.0;
	bool						m_bIsLeftSide		= true;
	bool						m_bUsingDeadReckoning = false;
};
