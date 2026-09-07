#pragma once
#include "Object.h"

class CDynamicSprite;
class CBang;
class CStar;

class CPlayer : public CObject
{
	constexpr	static D3DCOLOR NORMALHAIRCOLOR = D3DCOLOR_ARGB(255, 255, 129, 255);
	constexpr	static D3DCOLOR DASHHAIRCOLOR	= D3DCOLOR_ARGB(255, 178, 67, 67);
	constexpr	static D3DCOLOR BLINCKHAIRCOLOR = D3DCOLOR_ARGB(255, 255, 255, 255);

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
    CPlayer();
	void Reset();
    ~CPlayer();

	void	Initialize() override;
	void	SetCollision();
	void	Update(double _dDeltaTime) override;

	void Dead();
	
	void	LateUpdate(double _dDeltaTime) override;
	void	Release() override;

	void	SetKeyContext();
	void	SetDirectionKeyContext();

	bool IsDash() const { return m_PlayerState == PS_ONDASH; }
	void Abracadabra()			{
		if (m_PlayerState != PS_ONDEAD) CSoundManager::Instance().PlayPlayerSound(L"predeath.wav");
		m_PlayerState = PS_ONDEAD;
	}
	void SetOnIcelWall() { m_bOnIceWall = true; }
	void SetOnlift()	 { m_bOnLift	= true; }
	bool IsOnLift()		 { return m_bOnLift; }
	bool IsUp()			{ return m_curVelocity.second < 0; }
	void Boost(pos<int> _direction, double _duration, bool _bZoom = false);
	void WingardiumLeviosa();
	void SetOnAir()			{ 
		m_PlayerState = PS_ONAIR; 
		m_pDashTimer->SetActive(false);
		m_bOnFeather = false;

		m_bOnBoost = false;
	}

	void SetLiftAcceleration(pos<double> _acc)		{ m_liftAcceleration	= _acc; }
	void SetLiftVelocity	(pos<double> _velocity)	{ m_liftVelocity		= _velocity; }
	pos<double> GetLiftVelocity()	{ return m_liftVelocity; }
	void SetLiftBoost		(pos<double> _boost)	{ m_liftBoost			= _boost; }
	//void Move(pos<int>    _offset)					{ m_curPos.first += _offset.first; m_curPos.second += _offset.second;}
	void Move(pos<int>    _offset) { m_RemainderPos.first += _offset.first; m_RemainderPos.second += _offset.second; Move(); }
	void SetVelocity(pos<double> _velocity) { 
		m_curVelocity.first  = _velocity.first; 
		m_curVelocity.second = _velocity.second; 
	}
	pos<int>	GetDirection() { return m_curDirection; }
	pos<int>	GetNetworkPosition() const { 
		return { m_curPos.first, m_curPos.second }; }
	pos<double> GetNetworkVelocity() const {
		return { m_curVelocity.first + m_liftVelocity.first, m_curVelocity.second + m_liftVelocity.second };
	}
	pos<double> GetNetworkAcc() const	   { return m_curAcceleration; }
	int			GetNetworkState() const { return static_cast<int>(m_PlayerState); }

	void SetPos(pos<int> _pos)						{ m_curPos = _pos; }
	bool IsOnWall()		 { return m_PlayerState == PS_ONWALL; }
	void RefillDash()	 { 
		m_iDashCnt = 2;
		RefillStamina();
	}
	void ReSpawn(pos<int> _respawnPos);
	void Spring() { 
		m_curVelocity.second = -300 * PIXEL; 
		CSoundManager::Instance().PlayObjectSound(L"Object_Spring.wav", 0.5f);
	}
	void NormalJump(double _yboost = 0);
//physics
private:
	//MOVE
	constexpr static double ONGROUNDXACC		= +10.83 * PIXEL / FRAME;
	constexpr static double ONAIRXACC			= +10.83 * PIXEL / FRAME;
	constexpr static double ONGROUNDXRACC		= -06.66 * PIXEL / FRAME;
	constexpr static double ONAIRXRACC			= -04.33 * PIXEL / FRAME;
	constexpr static double ONGROUNDXFRIC		= -10.83 * PIXEL / FRAME;
	constexpr static double ONAIRXFRIC			= -16.67 * PIXEL / FRAME;

	constexpr static double XMAXVELOCITY		= +90	 * PIXEL;

	//JUMP
	constexpr static double YFALLACC			= +15	 * PIXEL / FRAME;
	constexpr static double YDELAYFALLACC		= +7.5	 * PIXEL / FRAME;
	constexpr static double YFALLRACC			= -5	 * PIXEL / FRAME;
	constexpr static double YFASTFALLACC		= +5	 * PIXEL / FRAME;

	constexpr static double YDELAYFALLVELOCITY	= +40	 * PIXEL;
	constexpr static double YFASTFALLVELOCITY	= +160	 * PIXEL;
	constexpr static double YMAXFALLVELOCITY	= +240	 * PIXEL;

	constexpr static double YJUMPVELOCITY		= -105	 * PIXEL ;
	constexpr static double LONGJUMPTIME		= 0.2;
	constexpr static double COYOTETIME			= 0.1;
	constexpr static double JUMPBUFFERTIME		= 0.2;

	//WALL
	constexpr static double YWALLUPVELOCITY		= -45	 * PIXEL;
	constexpr static double YWALLDOWNVELOCITY	= +80	 * PIXEL;

	constexpr static double XWALLJUMPVELOCITY	= +130	 * PIXEL;
	constexpr static double WALLJUMPTIME		= 10	 * FRAME;

	constexpr static double MAXSTAMINA			= 110;
	constexpr static double TIREDSTAMINA		= 20;
	constexpr static double EXHAUSTEDSTAMINA	= 0;

	constexpr static double HOLDSTAMINA			= 10;
	constexpr static double JUMPSTAMINA			= 27.5;
	constexpr static double CLIMBSTAMINA		= 35.45;

	constexpr static double TIREDBLINKDURATION	= 0.25f;

	//Dash
	constexpr static double DASHSPEED			= 240	 * PIXEL;
	constexpr static double DASHTIME			= 12	 * FRAME;
	constexpr static double FREEZETIME			= 3	     * FRAME;

	//Boost
	constexpr static double BOOSTSPEED = 240 * PIXEL;
	
	//Feather
	constexpr static double FEATHERMAXSPEED = 120 * PIXEL;
	constexpr static double FEATHERACC		= 40   * PIXEL / FRAME;
	constexpr static double FEATHERFRIC		= 1.5 * PIXEL / FRAME;
	constexpr static double FEATHERTIME		= 2.5f;
	
	//LiftVelocity
	constexpr static double XMAXLIFTVELOCITY = +250 * PIXEL;
	constexpr static double YMAXLIFTVELOCITY = -260 * PIXEL;


	void	TransitPlayerState();

	void	UpdateKinematics(double _dDeltaTime);
	void	SetXAcceleration(double _dDeltaTime);
	void	SetYAcceleration(double _dDeltaTime);
	void	SetXVelocity(double _dDeltaTime);
	void	SetYVelocity(double _dDeltaTime);
	void	Move();
	bool	UnitMove(int _sx, int _sy);

	//Common
	EPlayerState		m_PlayerState;
	pos<double>			m_curAcceleration;
	pos<double>			m_curVelocity;
	pos<double>			m_RemainderPos;

	pos<int>			m_curPos;
	bool				m_bOnGround;

	//Jump
	void				Jump();
	void CreateDustParticle();
	void				CreateLongJumpTimer();
	void				CreateCoyoteTimeTimer();
	void				CreateJumpBufferTimeTimer();
	UniqueTimer			m_plongJumpTimer;
	UniqueTimer			m_pCoyoteTimeTimer;
	UniqueTimer			m_pJumpBufferTimer;

	//Wall
	void				CreateWallJumpTimer();
	UniqueTimer			m_pWallJumpTimer;
	int					m_iContactWallSide;
	int					m_iWallJumpDir;

	void				TiredBlink();
	void				CreateTiredTimer();
	UniqueTimer			m_pTiredTimer;
	void				RefillStamina() { m_dStamina = MAXSTAMINA; }
	double				m_dStamina;

	//Dash
	void				Dash();
	void SetDashDirection();
	void				CreateDashTimer();
	UniqueTimer			m_pDashTimer;
	pos<int>			m_dashDirection;
	bool				m_bDashDirFlag = false;

	int					m_iDashCnt;

	//Boost
	void				CreateBoostTimer();
	UniqueTimer			m_pBoostTimer;
	pos<int>			m_boostDirection;
	bool				m_bOnBoost;

	//Feather
	void				CreateFeatherTimer();
	UniqueTimer			m_pFeatherTimer;
	pos<int>			m_FeatherDirection;
	bool				m_bOnFeather;

	//Lift
	pos<double>			m_liftAcceleration;
	pos<double>			m_liftVelocity;
	pos<double>			m_liftBoost;
	bool				m_bOnLift;

	void				CreateFootSoundTimer();
	UniqueTimer			m_pFootSoundTimer;
private:
	void	OnCollision(FCollisionInfo _CollisionInfo);
	
	bool	m_bOnIceWall;
private:
	void	SetHair();
	void	SetStar();

	void CreatePlayerSprite();
	CDynamicSprite*		CreatePlayerGroundSprite();
	CDynamicSprite*		CreatePlayerWallSprite();
	CDynamicSprite*		CreatePlayerAirSprite();
	CSprite*			CreatePlayerBoostSprite();
	CSprite*			CreatePlayerDeadSprite();

private:
	UniqueKeyContext	m_KeyContext;

	bool				m_bIsLeftSide;

	pos<int>			m_curDirection;

	CBang*				m_pHair;
	CStar*				m_pStar;
	CSprite*			m_pSprite;
};

