#include "pch.h"
#include "RemotePlayer.h"

#include "StageManager.h"
#include "NetworkManager.h"
#include "SceneManager.h"
#include "Scene.h"

#include "StaticSprite.h"
#include "AnimationSprite.h"
#include "DynamicSprite.h"
#include "EmptySprite.h"

#include "Bang.h"

CRemotePlayer::CRemotePlayer() = default;
CRemotePlayer::~CRemotePlayer() = default;

void CRemotePlayer::Initialize()
{
	CObject::Initialize();

	AddComponent<CRendererComponent>();

	SetCollision();
	SetHair();
	SetPlayerSprite();
}

void CRemotePlayer::Update(double _dDeltaTime)
{
	CObject::Update(_dDeltaTime);
	m_pHair->SetActive(m_PlayerState != PS_ONDEAD);
	
	m_SnapShots.first.m_dElapsed  += _dDeltaTime;
	m_SnapShots.second.m_dElapsed += _dDeltaTime;

	if (m_SnapShots.first.m_dElapsed < FRENDERDELAY )	{
		m_bUsingDeadReckoning = false;
		SnapShot();
	}
	else		{
		double _dElapsed = min(m_SnapShots.first.m_dElapsed - FRENDERDELAY, FRAME * 10);
		m_bUsingDeadReckoning = true;
		DeadReckoning		(_dElapsed);
		InterpolatePredict(_dDeltaTime);
	}

	GetTransform()->SetPos(m_posRender);
}

void CRemotePlayer::InterpolatePredict(double _dDeltaTime)
{
	static int ITHRESHOLD = 2 * PIXEL;
	float fRatio = 1.0 - exp(-m_fCorrectionSpeed * _dDeltaTime);

	m_fCorrectionSpeed = 10.f * (1.0 - exp(-10.f * _dDeltaTime)) + m_fCorrectionSpeed * (exp(-10.f * _dDeltaTime));

	int dx = abs(m_posRender.first  - m_posPredict.first);
	int dy = abs(m_posRender.second - m_posPredict.second);
	
	if (dx <= ITHRESHOLD)
		m_posRender.first	= m_posPredict.first;
	else
		m_posRender.first	= m_posPredict.first  * (fRatio)+m_posRender.first  * (1.f - fRatio);
	if (dy <= ITHRESHOLD)
		m_posRender.second  = m_posPredict.second;
	else
		m_posRender.second  = m_posPredict.second * (fRatio)+m_posRender.second * (1.f - fRatio);
}

void CRemotePlayer::SnapShot()
{
	const double duration = m_SnapShots.second.m_dElapsed - m_SnapShots.first.m_dElapsed;

	if (duration <= 0.0) {
		m_posPredict = m_SnapShots.first.m_posNetwork;
		return;
	}

	double t = (m_SnapShots.second.m_dElapsed - FRENDERDELAY) / duration;

	t = std::clamp(t, 0.0, 1.0);

	const double t2 = t * t;
	const double t3 = t2 * t;

	// Hermite Basis
	const double h00 = 2.0 * t3 - 3.0 * t2 + 1.0;
	const double h10 = t3 - 2.0 * t2 + t;
	const double h01 = -2.0 * t3 + 3.0 * t2;
	const double h11 = t3 - t2;


	constexpr double EPS = 0.001;

	if (abs(m_SnapShots.first.m_posNetwork.first - m_SnapShots.second.m_posNetwork.first) <= EPS)
	{
		m_posPredict.first = m_SnapShots.second.m_posNetwork.first;
	}
	else {
		m_posPredict.first =
			h00 * m_SnapShots.second.m_posNetwork.first +
			h10 * m_SnapShots.second.m_curVelocity.first * duration +
			h01 * m_SnapShots.first.m_posNetwork.first +
			h11 * m_SnapShots.first.m_curVelocity.first * duration;
	}

	if (abs(m_SnapShots.first.m_posNetwork.second - m_SnapShots.second.m_posNetwork.second) <= EPS)
	{
		m_posPredict.second = m_SnapShots.second.m_posNetwork.second;
	}
	else {
		m_posPredict.second =
			h00 * m_SnapShots.second.m_posNetwork.second +
			h10 * m_SnapShots.second.m_curVelocity.second * duration +
			h01 * m_SnapShots.first.m_posNetwork.second +
			h11 * m_SnapShots.first.m_curVelocity.second * duration;
	}

	m_posRender = m_posPredict;
}

void CRemotePlayer::LateUpdate(double _dDeltaTime)
{
	CObject::LateUpdate(_dDeltaTime);
}

void CRemotePlayer::Release()
{
	CObject::Release();
}

void CRemotePlayer::ApplyNetworkState(const PlayerStateData& state)
{
	m_posNetwork					= { state.posX,		state.posY		};
	m_curVelocity					= { state.velocityX,state.velocityY };
	m_curAcceleration				= { state.accX,		state.accY		};
	m_curDirection					= { state.facingX , state.facingY	};
	m_SnapShots						= { 
		FSnapShot{m_curVelocity, m_posNetwork, (state.RTT + CNetworkManager::Instance().GetPingMs()) * 0.001 * 0.5},
		m_SnapShots.first};

	if		(state.facingX > 0)	m_bIsLeftSide = true;
	else if (state.facingX < 0) m_bIsLeftSide = false;
	if (m_PlayerState != (EPlayerState)state.iPlayerState) {
		m_fCorrectionSpeed = 40.f;
	}
	m_PlayerState = (EPlayerState)state.iPlayerState;
}

void CRemotePlayer::DeadReckoning(double _dDeltaTime)
{
	m_posPredict = m_SnapShots.first.m_posNetwork;
	GetTransform()->SetPos(m_posPredict);
	m_RemainderPos.first	= m_curVelocity.first  * _dDeltaTime + m_curAcceleration.first  * _dDeltaTime * _dDeltaTime * 0.5f;
	m_RemainderPos.second	= m_curVelocity.second * _dDeltaTime + m_curAcceleration.second * _dDeltaTime * _dDeltaTime * 0.5f;
	Move();
}
void CRemotePlayer::Move() {
	int sx = (m_RemainderPos.first > 0) ? PIXEL : -PIXEL;
	int sy = (m_RemainderPos.second > 0) ? PIXEL : -PIXEL;
	int dx = (int)floor(abs(m_RemainderPos.first));
	int dy = (int)floor(abs(m_RemainderPos.second));

	while (dx >= PIXEL && UnitMove(sx, 0)) { dx -= PIXEL; m_RemainderPos.first -= sx; };
	if (dx > PIXEL) m_RemainderPos.first = 0;
	while (dy >= PIXEL && UnitMove(0, sy)) { dy -= PIXEL; m_RemainderPos.second -= sy; };
	if (dy > PIXEL) m_RemainderPos.second = 0;
}

bool CRemotePlayer::UnitMove(int _sx, int _sy)
{
	m_posPredict.first  += _sx;
	m_posPredict.second += _sy;

	GetTransform()->SetPos(m_posPredict);

	bool bIsCollision = CCollisionManager::Instance().CheckPlayerMoveCollision(this);
	if (!bIsCollision) return true;

	m_posPredict.first -= _sx;
	m_posPredict.second -= _sy;
	GetTransform()->SetPos(m_posPredict);

	return false;
}


void CRemotePlayer::SetCollision()
{
	AddComponent<CCollisionComponent>();
	auto _pCollisionComponent = GetComponent<CCollisionComponent>();
	_pCollisionComponent->SetRenderable(true);
	_pCollisionComponent->SetSize({ 10 * PIXEL, 11 * PIXEL });
	float SpriteHalfSize = 32 * PIXEL * 0.5;
	float BBSize = 11 * PIXEL;
	_pCollisionComponent->SetPivot({ 0.5f, (BBSize - SpriteHalfSize) / BBSize });
}

void CRemotePlayer::SetHair()
{
	m_pHair = CSceneManager::Instance().GetScene()->CreateObject(HAIR, new CBang);
	m_pHair->SetReverse(&m_bIsLeftSide);
	m_pHair->SetParent(this);
}

void CRemotePlayer::SetPlayerSprite()
{
	auto _pRenderComopnent = GetComponent<CRendererComponent>();
	if (!_pRenderComopnent) return;

	auto pPlayerSprite = new CDynamicSprite();
	m_pSprite = pPlayerSprite;
	pPlayerSprite->SetReverse(&m_bIsLeftSide);
	pPlayerSprite->SetCroppedSize({ 32,32 });
	pPlayerSprite->SetScale(PIXEL);

	_pRenderComopnent->SetSprite(pPlayerSprite);

	auto pPlayerGroundSprite	= CreatePlayerGroundSprite();
	auto pPlayerAirSprite		= CreatePlayerAirSprite();
	auto pPlayerWALLSprite		= CreatePlayerWallSprite();
	auto pPlayerBoostSprite		= CreatePlayerBoostSprite();
	auto pPlayerDeadSprite		= CreatePlayerDeadSprite();

	pPlayerSprite->AddState(pPlayerGroundSprite);
	pPlayerSprite->AddState(pPlayerAirSprite);
	pPlayerSprite->AddState(pPlayerWALLSprite);
	pPlayerSprite->AddState(pPlayerBoostSprite);
	pPlayerSprite->AddState(pPlayerDeadSprite);

	pPlayerSprite->AddTransition(-1, 0, [&]() {return m_PlayerState == PS_ONGROUND; });
	pPlayerSprite->AddTransition(-1, 0, [&]() {return m_PlayerState == PS_ONDASH; });
	pPlayerSprite->AddTransition(-1, 1, [&]() {return m_PlayerState == PS_ONAIR; });
	pPlayerSprite->AddTransition(-1, 2, [&]() {return m_PlayerState == PS_ONWALL; });
	pPlayerSprite->AddTransition(-1, 3, [&]() {return m_PlayerState == PS_ONBOOST; });
	pPlayerSprite->AddTransition(-1, 4, [&]() {return m_PlayerState == PS_ONDEAD; });
	pPlayerGroundSprite->SetStateIdx(0);
}

CDynamicSprite* CRemotePlayer::CreatePlayerGroundSprite()
{
	auto pPlayerGroundSprite = new CDynamicSprite;
	pPlayerGroundSprite->Initialize();
	{
		auto time_scale = 0.7f;
		auto WALKL = new CAnimationSprite(true);
		WALKL->Load(PLAYERROCOMOTIONIMGPATH, L"Walk.png");
		WALKL->Initialize();
		WALKL->CreateTimer(1 * time_scale);

		auto IDLEL = new CAnimationSprite(true);
		IDLEL->Load(PLAYERROCOMOTIONIMGPATH, L"Idle.png");
		IDLEL->Initialize();
		IDLEL->CreateTimer(1 * time_scale);

		auto FLIPRL = new CAnimationSprite(true);
		FLIPRL->Load(PLAYERROCOMOTIONIMGPATH, L"Flip2.png");
		FLIPRL->Initialize();
		FLIPRL->CreateTimer(2.2 * time_scale);

		pPlayerGroundSprite->AddState(WALKL);
		pPlayerGroundSprite->AddState(IDLEL);
		pPlayerGroundSprite->AddTransitionState(FLIPRL);
	}
	pPlayerGroundSprite->AddTransition(0, 1, [&]() {return m_curDirection.first == +0; });
	pPlayerGroundSprite->AddTransition(1, 0, [&]() {return m_curDirection.first != +0; });

	pPlayerGroundSprite->SetStateIdx(1);

	return pPlayerGroundSprite;
}

CDynamicSprite* CRemotePlayer::CreatePlayerWallSprite()
{
	auto pPlayerWallSprite = new CDynamicSprite;
	pPlayerWallSprite->Initialize();
	{
		auto time_scale = 0.7f;
		auto IDLE = new CStaticSprite(true);
		IDLE->Load(PLAYERROCOMOTIONIMGPATH, L"ClimbIdle.png");
		IDLE->Initialize();

		auto CLIMB = new CAnimationSprite(true);
		CLIMB->Load(PLAYERROCOMOTIONIMGPATH, L"Climb.png");
		CLIMB->Initialize();
		CLIMB->CreateTimer(2.2 * time_scale);

		pPlayerWallSprite->AddState(IDLE);
		pPlayerWallSprite->AddState(CLIMB);
	}
	pPlayerWallSprite->AddTransition(0, 1, [&]() {return m_curDirection.second != +0; });
	pPlayerWallSprite->AddTransition(1, 0, [&]() {return m_curDirection.second == +0; });

	pPlayerWallSprite->SetStateIdx(0);

	return pPlayerWallSprite;
}

CDynamicSprite* CRemotePlayer::CreatePlayerAirSprite()
{
	auto pPlayerAirSprite = new CDynamicSprite;
	pPlayerAirSprite->Initialize();
	{
		auto time_scale = 1.0f;
		auto JUMPL = new CAnimationSprite(true);
		JUMPL->Load(PLAYERROCOMOTIONIMGPATH, L"JumpSlow.png");
		JUMPL->Initialize();
		JUMPL->CreateTimer(1 * time_scale);

		auto FALLL = new CAnimationSprite(true);
		FALLL->Load(PLAYERROCOMOTIONIMGPATH, L"Fall.png");
		FALLL->Initialize();
		FALLL->CreateTimer(1 * time_scale);

		auto FALLPOSEL = new CAnimationSprite(true);
		FALLPOSEL->Load(PLAYERROCOMOTIONIMGPATH, L"FallPose.png");
		FALLPOSEL->Initialize();
		FALLPOSEL->CreateTimer(1 * time_scale);

		pPlayerAirSprite->AddState(JUMPL);
		pPlayerAirSprite->AddState(FALLL);
		pPlayerAirSprite->AddState(FALLPOSEL);
	}
	pPlayerAirSprite->AddTransition(0, 1, [&]() {return 1; }, true);

	return pPlayerAirSprite;
}

CSprite* CRemotePlayer::CreatePlayerBoostSprite()
{
	auto pPlayerBoostSprite = new CAnimationSprite;
	pPlayerBoostSprite->Load(PLAYERROCOMOTIONIMGPATH, L"Launch.png");
	pPlayerBoostSprite->Initialize();
	pPlayerBoostSprite->CreateTimer(0.7f);

	return pPlayerBoostSprite;
}

CSprite* CRemotePlayer::CreatePlayerDeadSprite()
{
	auto pPlayerDeadSprite = new CDynamicSprite;
	pPlayerDeadSprite->Initialize();
	{
		auto DEAD = new CAnimationSprite(true);
		DEAD->Load(PLAYERROCOMOTIONIMGPATH, L"Dead.png");
		DEAD->Initialize();
		DEAD->CreateTimer(0.5);

		auto EMPTYSPRITE = new CEmptySprite;

		pPlayerDeadSprite->AddState(DEAD);
		pPlayerDeadSprite->AddState(EMPTYSPRITE);
	}
	pPlayerDeadSprite->AddTransition(0, 1, [&]() {return 1; }, true);

	return pPlayerDeadSprite;
}
