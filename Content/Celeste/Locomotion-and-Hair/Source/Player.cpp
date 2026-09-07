#include "pch.h"
#include "Player.h"
#include "Bang.h"
#include "RendererComponent.h"
#include "CollisionComponent.h"
#include "SemiBlock.h"

#include "StaticSprite.h"
#include "AnimationSprite.h"
#include "DynamicSprite.h"
#include "EmptySprite.h"

#include "DashLine.h"
#include "Dust.h"
#include "Scene.h"
#include "Star.h"

CPlayer::CPlayer() : CObject()
{
	m_pHair = nullptr;
	m_curDirection		= { 0,0 };
	m_bOnBoost = m_bOnFeather = false;
	Reset();
}

void CPlayer::Reset()
{
	m_curPos			= { 0,0 };
	m_curAcceleration	= { 0.0, 0.0 };
	m_curVelocity		= { 0.0, 0.0 };
	m_RemainderPos		= { 0.0, 0.0 };


	m_liftAcceleration	= { 0.0, 0.0 };
	m_liftVelocity		= { 0.0, 0.0 };
	m_liftBoost			= { 0.0, 0.0 };
}

CPlayer::~CPlayer()
{
	Release();
}

void CPlayer::Initialize()
{
	CObject::Initialize();

	AddComponent<CRendererComponent>();
	SetCollision();

	SetHair();
	SetStar();

	CreatePlayerSprite();

	SetKeyContext();


	CreateLongJumpTimer();
	CreateCoyoteTimeTimer();
	CreateJumpBufferTimeTimer();
	CreateWallJumpTimer();
	CreateDashTimer();
	CreateBoostTimer();
	CreateFeatherTimer();
	CreateTiredTimer();

	CreateFootSoundTimer();

	ReSpawn(CStageManager::Instance().GetStageInfo().RespawnPos);
	
	m_PlayerState	= PS_ONAIR;
	m_bIsLeftSide	= true;
	m_iWallJumpDir	= 0;
}

void CPlayer::SetCollision()
{
	AddComponent<CCollisionComponent>();
	auto _pCollisionComponent = GetComponent<CCollisionComponent>();
	_pCollisionComponent->SetRenderable(true);
	_pCollisionComponent->SetSize({ 10 * PIXEL, 11 * PIXEL });
	float SpriteHalfSize = 32 * PIXEL * 0.5;
	float BBSize = 11 * PIXEL;
	_pCollisionComponent->SetPivot({ 0.5f, (BBSize - SpriteHalfSize) / BBSize });

	_pCollisionComponent->SetOntriggerFunc(bind(&CPlayer::OnCollision, this, placeholders::_1));
}

void CPlayer::SetHair()
{
	m_pHair = CSceneManager::Instance().GetScene()->CreateObject(HAIR, new CBang);
	m_pHair->SetReverse(&m_bIsLeftSide);
	m_pHair->SetParent(this);
}

void CPlayer::SetStar()
{
	m_pStar = CSceneManager::Instance().GetScene()->CreateObject(HAIR, new CStar);
	m_pStar->SetParent(this);
}

void CPlayer::Update(double _dDeltaTime)
{
	CObject::Update(_dDeltaTime);

	if (m_bDashDirFlag && !CTimeManager::Instance().IsFreeze()) {
		SetDashDirection();
		m_bDashDirFlag = false;
	}
		

	m_pFootSoundTimer->SetActive(false);
	if (m_PlayerState == PS_ONGROUND) {
		RefillStamina();
		m_pCoyoteTimeTimer->Restart();
		if (m_curDirection.first != 0) {
			m_pFootSoundTimer->SetActive(true);
		}
	}
	TransitPlayerState();
	if (!CTimeManager::Instance().IsFreeze())
		UpdateKinematics(_dDeltaTime);

	if (m_PlayerState != PS_ONWALL) {
		if (m_curDirection.first > 0) m_bIsLeftSide = true;
		if (m_curDirection.first < 0) m_bIsLeftSide = false;
	}
	else {
		m_dStamina -= HOLDSTAMINA * _dDeltaTime;
		m_dStamina = max(m_dStamina, 0);
		m_bIsLeftSide = (m_iContactWallSide == 1) ? true : false;
	}


	if (m_iDashCnt == 2) m_pHair->SetHairColor(NORMALHAIRCOLOR);
	if (m_iDashCnt == 1) m_pHair->SetHairColor(DASHHAIRCOLOR);
	if (m_iDashCnt == 0) m_pHair->SetHairColor(NORMALHAIRCOLOR);

	m_iContactWallSide = 0;
	m_bOnGround = false;

	m_pStar->SetActive(m_bOnFeather);

	m_pHair->SetActive((!m_bOnFeather) && (m_PlayerState != PS_ONDEAD));
	GetComponent<CRendererComponent>()->SetActive(!m_bOnFeather);
	
	TiredBlink();
}

void CPlayer::Dead()
{
	m_pHair->SetActive(false);
	m_pDashTimer->SetActive(false);
	m_pFeatherTimer->SetActive(false);
	m_bOnFeather = false;

	m_pBoostTimer->SetActive(false);
	m_bOnBoost = false;
	
	CSoundManager::Instance().PlayObjectSound(L"death.wav");
	CRenderManager::Instance().StartFade(1, 0.4f, 
		[&]() {
			m_pHair->SetActive(true);
			m_bOnFeather = m_bOnBoost = false;
			ReSpawn(CStageManager::Instance().GetStageInfo().RespawnPos);
			CStageManager::Instance().ResetStage();
			CSoundManager::Instance().PlayObjectSound(L"revive.wav");
		}
		, 
		[&]() {
			m_PlayerState = PS_ONAIR;
		}
		);
}


void CPlayer::TransitPlayerState()
{
	if (m_curPos.second +5*PIXEL > CStageManager::Instance().GetStageInfo().MapSize.second*TILESIZE) {
		if (CStageManager::Instance().GetCurStage() != 40) {
			m_PlayerState = PS_ONDEAD;
		}
	}
	else if (m_PlayerState == PS_ONDEAD) {
		m_pDashTimer->SetActive(false);
		m_pFeatherTimer->SetActive(false);
		m_bOnFeather = false;

		m_pBoostTimer->SetActive(false);
		m_bOnBoost = false; 
	}
	else if (m_PlayerState == PS_ONBOOST) {
		if (!m_bOnBoost) 
			m_PlayerState = PS_ONAIR;
	}
	else if (m_PlayerState == PS_ONFEATHER) {
		if (!m_bOnFeather) m_PlayerState = PS_ONAIR;
	}
	else if (m_pDashTimer->IsActive()) {
		m_PlayerState = PS_ONDASH;
	}
	else if (!m_bOnIceWall && m_iContactWallSide && (m_curVelocity.second >= 0 || m_curDirection.second == +1) && CInputManager::Instance().IsKeyState('Z', EKeyActionState::Stay)) {
		m_PlayerState = PS_ONWALL;
	}
	else if (m_bOnGround){
		if (m_PlayerState!= PS_ONGROUND && !m_plongJumpTimer->IsActive()) {
			wstring ss = L"land_09_moltenrock_0";
			ss += to_wstring(1 + rand() % 5);
			ss += L".wav";
			CSoundManager::Instance().PlayPlayerSound(ss.c_str());

			m_PlayerState = PS_ONGROUND;
			if (m_pJumpBufferTimer->IsActive()) {
				Jump();
			}
		}
		m_PlayerState = PS_ONGROUND;
	}
	else {
		m_PlayerState = PS_ONAIR;
	}

	//if (m_curPos.second > CSceneManager::Instance())
}

void CPlayer::UpdateKinematics(double _dDeltaTime)
{
	if (!m_bOnLift) {
		m_curVelocity.first  += m_liftVelocity.first;
		m_curVelocity.second += m_liftVelocity.second;
		m_liftVelocity		= { 0,0 };
		m_liftAcceleration	= { 0,0 };
	}
	SetXAcceleration(_dDeltaTime);
	SetYAcceleration(_dDeltaTime);

	SetXVelocity(_dDeltaTime);
	SetYVelocity(_dDeltaTime);

	if (m_PlayerState == PS_ONFEATHER) {
		auto _speed		= sqrt(m_curVelocity.first * m_curVelocity.first + m_curVelocity.second * m_curVelocity.second);
		double _mult	= _speed / FEATHERMAXSPEED;
		if (_mult > 1.0f) {
			m_curVelocity.first  /= _mult;
			m_curVelocity.second /= _mult;
		}
	}

	bool _bLast = (CStageManager::Instance().GetCurStage() == 40);
	
	if (abs(m_curVelocity.first) < 20 *(2-_bLast)) m_curVelocity.first = 0;

	m_RemainderPos.first  += (m_curVelocity.first  + m_liftVelocity.first)  * _dDeltaTime + m_curAcceleration.first  * _dDeltaTime * _dDeltaTime * 0.5;
	m_RemainderPos.second += (m_curVelocity.second + m_liftVelocity.second) * _dDeltaTime + m_curAcceleration.second * _dDeltaTime * _dDeltaTime * 0.5;

	Move();

	//PrintDebug(m_curVelocity.first," ", m_curAcceleration.first, " ", m_curPos.first);
	//PrintDebug(m_curVelocity.second," ", m_curAcceleration.second, " ", m_curPos.second);
	//PrintDebug(m_curVelocity.first, " ", m_curVelocity.second);
	//PrintDebug(m_PlayerState);
	
	m_curPos.second = (m_curPos.second / PIXEL) * PIXEL;
	m_curPos.first  = (m_curPos.first  / PIXEL) * PIXEL;

	if (m_curPos.second > WINCY + 11 * PIXEL) {
		if (CStageManager::Instance().GetCurStage() != 40) return;
		m_curPos.second -= (WINCY + 11 * PIXEL * 2);
	}
}

void CPlayer::SetXVelocity(double _dDeltaTime)
{
	switch (m_PlayerState)
	{
	case CPlayer::PS_NONE:
		break;
	case CPlayer::PS_ONGROUND:
		m_curVelocity.first += m_curAcceleration.first * _dDeltaTime;
		break;
	case CPlayer::PS_ONAIR:
		m_curVelocity.first += m_curAcceleration.first * _dDeltaTime;
		if (m_pWallJumpTimer->IsActive()) {
			if (abs(m_curVelocity.first) < abs(XWALLJUMPVELOCITY))
				m_curVelocity.first = m_iWallJumpDir * XWALLJUMPVELOCITY;
		}
		break;
	case CPlayer::PS_ONWALL:
		m_curVelocity.first = 0;
		break;
	case CPlayer::PS_ONDASH:
		m_curVelocity.first = m_dashDirection.first * ((m_dashDirection.second!=0) ? sqrt(0.5) : 1) * DASHSPEED;
		break;
	case CPlayer::PS_ONWALLJUMP:
		break;
	case CPlayer::PS_ONBOOST:
		m_curVelocity.first = m_boostDirection.first * ((m_boostDirection.second != 0) ? sqrt(0.5) : 1) * BOOSTSPEED;
		break;
	case CPlayer::PS_ONFEATHER:
		m_curVelocity.first += m_curAcceleration.first * _dDeltaTime;
		break;
	case CPlayer::PS_ONDEAD:
		m_curVelocity.first = 0;
		break;
	case CPlayer::PS_END:
		break;
	default:
		break;
	}
	m_liftVelocity.first	+= m_liftAcceleration.first;
}

void CPlayer::SetYVelocity(double _dDeltaTime)
{
	switch (m_PlayerState)
	{
	case CPlayer::PS_NONE:
		break;
	case CPlayer::PS_ONGROUND:
	case CPlayer::PS_ONAIR:
		m_curVelocity.second += m_curAcceleration.second * _dDeltaTime;
		if (m_plongJumpTimer->IsActive()) {
			if (abs(m_curVelocity.second) < abs(YJUMPVELOCITY))
				m_curVelocity.second = YJUMPVELOCITY;
		}
		break;
	case CPlayer::PS_ONWALL:
		if (m_dStamina <= EXHAUSTEDSTAMINA) {
			m_curVelocity.second += m_curAcceleration.second * _dDeltaTime;
		}
		else if (m_curDirection.second == 1) {
			m_dStamina -= CLIMBSTAMINA * _dDeltaTime;
			m_dStamina = max(m_dStamina, 0);
			m_curVelocity.second = YWALLUPVELOCITY;
		}
		else if (m_curDirection.second == -1) {
			m_curVelocity.second = YWALLDOWNVELOCITY;
		}
		else
			m_curVelocity.second = 0;
		break;
	case CPlayer::PS_ONDASH:
		if (m_dashDirection.second < 0 || m_curVelocity.second > (-m_dashDirection.second * ((m_dashDirection.first != 0) ? sqrt(0.5) : 1) * DASHSPEED))
			m_curVelocity.second = -m_dashDirection.second * ((m_dashDirection.first != 0) ? sqrt(0.5) : 1) * DASHSPEED;
		break;
	case CPlayer::PS_ONWALLJUMP:
		break;
		break;
	case CPlayer::PS_ONBOOST:
		m_curVelocity.second = m_boostDirection.second * ((m_boostDirection.first != 0) ? sqrt(0.5) : 1) * BOOSTSPEED;
		break;
	case CPlayer::PS_ONFEATHER:
		m_curVelocity.second += m_curAcceleration.second * _dDeltaTime;
		break;
	case CPlayer::PS_ONDEAD:
		m_curVelocity.second = 0;
		break;
	case CPlayer::PS_END:
		break;
	default:
		break;
	}
	m_liftVelocity.second += m_liftAcceleration.second;
	m_liftVelocity.second = clamp(m_liftVelocity.second, YMAXLIFTVELOCITY, +0.0);
}

void CPlayer::SetXAcceleration(double _dDeltaTime)
{
	switch (m_PlayerState)
	{
	case CPlayer::PS_NONE:
		
		break;
	case CPlayer::PS_ONGROUND:
		if (abs(m_curVelocity.first) > XMAXVELOCITY) {
			if (sign(m_curVelocity.first) == sign(m_curDirection.first)) //정방향 키입력
				m_curAcceleration.first = sign(m_curDirection.first) * ONGROUNDXRACC;
			else												//역방향 or 미입력
				m_curAcceleration.first = sign(m_curVelocity.first) * ONGROUNDXFRIC;
		}
		else {
			if (sign(m_curVelocity.first)==0 || sign(m_curVelocity.first) == sign(m_curDirection.first))
				m_curAcceleration.first = sign(m_curDirection.first) * ONGROUNDXACC;
			else
				m_curAcceleration.first = sign(m_curVelocity.first) * ONGROUNDXFRIC;
		}
		break;
	case CPlayer::PS_ONAIR:
		if (abs(m_curVelocity.first) > XMAXVELOCITY) {
			if (sign(m_curVelocity.first) == sign(m_curDirection.first)) //정방향 키입력
				m_curAcceleration.first = sign(m_curDirection.first) * ONAIRXRACC;
			else
				m_curAcceleration.first = sign(m_curVelocity.first) * ONAIRXFRIC;
		}
		else {
			if (sign(m_curVelocity.first) == 0 || sign(m_curVelocity.first) == sign(m_curDirection.first))
				m_curAcceleration.first = sign(m_curDirection.first) * ONAIRXACC;
			else
				m_curAcceleration.first = sign(m_curVelocity.first) * ONAIRXFRIC;
		}
		break;
	case CPlayer::PS_ONWALL:
		m_curAcceleration.first = 0;
		break;
	case CPlayer::PS_ONDASH:
		m_curAcceleration.first = 0;
		break;
	case CPlayer::PS_ONWALLJUMP:
		break;
	case CPlayer::PS_ONBOOST:
		m_curAcceleration.first = 0;
		break;
	case CPlayer::PS_ONFEATHER:
		m_curAcceleration.first = FEATHERACC * m_curDirection.first * ((m_curDirection.second != 0) ? sqrt(0.5) : 1);
		break;
	case CPlayer::PS_ONDEAD:
		m_curAcceleration.first = 0;
		break;
	case CPlayer::PS_END:
		break;
	default:
		break;
	}
}

void CPlayer::SetYAcceleration(double _dDeltaTime)
{
	switch (m_PlayerState)
	{
	case CPlayer::PS_NONE:
		break;
	case CPlayer::PS_ONGROUND:
		m_curAcceleration.second = 0;
		break;
	case CPlayer::PS_ONWALL:
		if (m_dStamina > EXHAUSTEDSTAMINA) {
			m_curAcceleration.second = 0;
			break;
		}
	case CPlayer::PS_ONAIR:
		if (m_curVelocity.second < -YDELAYFALLVELOCITY) {
			m_curAcceleration.second = YFALLACC;
		}
		else if (m_curVelocity.second < YDELAYFALLVELOCITY) {
			if (1 || CInputManager::Instance().IsKeyState('C', EKeyActionState::Stay))
				m_curAcceleration.second = YDELAYFALLACC;
			else
				m_curAcceleration.second = YFALLACC;
		}
		else if (m_curVelocity.second < YFASTFALLVELOCITY) {
			m_curAcceleration.second = YFALLACC;
		}
		else if (m_curVelocity.second < YMAXFALLVELOCITY) {
			if (m_curDirection.second==-1)
				m_curAcceleration.second = YFASTFALLACC;
			else
				m_curAcceleration.second = YFALLRACC;
		}
		else {
			m_curAcceleration.second = YFALLRACC;
		}

		break;
	case CPlayer::PS_ONDASH:
		m_curAcceleration.second = 0;
		break;
	case CPlayer::PS_ONWALLJUMP:
		break;
	case CPlayer::PS_ONBOOST:
		m_curAcceleration.second = 0;
		break;
	case CPlayer::PS_ONFEATHER:
		m_curAcceleration.second = -FEATHERACC * m_curDirection.second * ((m_curDirection.first != 0) ? sqrt(0.5) : 1);
		break;
	case CPlayer::PS_ONDEAD:
		m_curAcceleration.second = 0;
		break;
	case CPlayer::PS_END:
		break;
	default:
		break;
	}
}

void CPlayer::Move()
{
	int sx = (m_RemainderPos.first  > 0) ? PIXEL : -PIXEL;
	int sy = (m_RemainderPos.second > 0) ? PIXEL : -PIXEL;
	int dx = (int)floor(abs(m_RemainderPos.first));
	int dy = (int)floor(abs(m_RemainderPos.second));

	while (dx >= PIXEL && UnitMove(sx, 0)) { dx-=PIXEL; m_RemainderPos.first  -= sx; };
	if (dx > PIXEL) m_RemainderPos.first  = 0;
	while (dy >= PIXEL && UnitMove(0, sy)) { dy-=PIXEL; m_RemainderPos.second -= sy; };
	if (dy > PIXEL) m_RemainderPos.second = 0;
}

bool CPlayer::UnitMove(int _sx, int _sy)
{
	m_curPos.first  += _sx;
	m_curPos.second += _sy;

	GetTransform()->SetPos(m_curPos);


	bool bIsCollision = CCollisionManager::Instance().CheckPlayerMoveCollision(this);
	if (!bIsCollision) return true;

	m_curPos.first  -= _sx;
	m_curPos.second -= _sy;
	GetTransform()->SetPos(m_curPos);
	return false;
}

void CPlayer::LateUpdate(double _dDeltaTime)
{
	CObject::LateUpdate(_dDeltaTime);
	m_bOnIceWall = false;
	
	m_bOnLift	 = false;
}

void CPlayer::Release()
{
	CObject::Release();
}

void CPlayer::SetKeyContext()
{
	m_KeyContext = CreateKeyContext();

	SetDirectionKeyContext();
	m_KeyContext->AddFunc('C', [&]() {Jump();}, EKeyActionState::Enter);
	m_KeyContext->AddFunc('X', [&]() {Dash(); }, EKeyActionState::Enter);
	m_KeyContext->AddFunc('C', [&]() {
		m_plongJumpTimer->SetActive(false);
		},
	EKeyActionState::Exit);

	m_KeyContext->SetActive(true);
}

void CPlayer::Jump()
{

	bool _bJump = false;
	if (m_PlayerState == PS_ONGROUND || m_pCoyoteTimeTimer->IsActive()) {

		NormalJump();

		CreateDustParticle();

		_bJump = true;

		CSoundManager::Instance().PlayPlayerSound(L"jump.wav");
	}
	else if (m_iContactWallSide) {

		
		if (m_PlayerState != PS_ONWALL || m_curDirection.first == -m_iContactWallSide) {
			m_iWallJumpDir = -m_iContactWallSide;
			m_pWallJumpTimer->Restart();

			CSoundManager::Instance().PlayPlayerSound((m_iContactWallSide) ? L"jump_wall_right.wav" : L"jump_wall_left.wav");
		}
		else if ((m_dStamina > 0)) {
			m_dStamina -= JUMPSTAMINA;
			m_dStamina = max(m_dStamina, 0);

			CSoundManager::Instance().PlayPlayerSound(L"jump.wav");
		}
		else {
			return;
		}
		m_curVelocity.second = YJUMPVELOCITY;
		m_plongJumpTimer->Restart();

		m_PlayerState = PS_ONAIR;


		CreateDustParticle();

		_bJump = true;
	}
	
	if (_bJump) {
		m_curVelocity.first  += m_liftBoost.first;
		m_curVelocity.second += m_liftBoost.second;
		m_liftBoost = { 0,0 };
	}
	else {
		m_pJumpBufferTimer->Restart();
	}
}

void CPlayer::NormalJump(double _yBoost)
{
	m_curVelocity.second = YJUMPVELOCITY + _yBoost;
	m_plongJumpTimer->Restart();
	m_PlayerState = PS_ONAIR;
}

void CPlayer::CreateDustParticle()
{
	auto _pos = GetComponent<CCollisionComponent>()->GetCenter();
	_pos.second += 5 * PIXEL;
	CParticle::CreateParticle<CDust>(_pos, 0);
}

void CPlayer::Dash()
{
	if (m_pDashTimer->IsActive() || m_iDashCnt <= 0) return;

	CTimeManager::Instance().Freeze(FREEZETIME);
	m_PlayerState = PS_ONDASH;
	m_iDashCnt--;
	m_pDashTimer->Restart();
	m_bOnFeather = false;
	m_bDashDirFlag = true;
}

void CPlayer::SetDashDirection()
{
	//PrintDebug(m_curDirection.first, " ", m_curDirection.second);

	m_dashDirection = 
	(m_curDirection == make_pair(0, 0)) ? 
	make_pair((m_bIsLeftSide) ? +1 : -1, 0) 
	: m_curDirection;

	//PrintDebug(m_dashDirection.first, " ", m_dashDirection.second);

	int _iType;
	if		(m_dashDirection == make_pair(+1, +0)) _iType = 0;
	else if (m_dashDirection == make_pair(+1, +1)) _iType = 1;
	else if (m_dashDirection == make_pair(+0, +1)) _iType = 2;
	else if (m_dashDirection == make_pair(-1, +1)) _iType = 3;
	else if (m_dashDirection == make_pair(-1, +0)) _iType = 4;
	else if (m_dashDirection == make_pair(-1, -1)) _iType = 5;
	else if (m_dashDirection == make_pair(+0, -1)) _iType = 6;
	else if (m_dashDirection == make_pair(+1, -1)) _iType = 7;
	CParticle::CreateParticle<CDashLine>(GetComponent<CCollisionComponent>()->GetCenter(), _iType);

	wstring ss;
	if (m_iDashCnt == 0) {
		if (m_dashDirection.first >= 0) ss = L"dash_pink_right.wav";
		else							ss = L"dash_pink_right.wav";
	}
	else {
		if (m_dashDirection.first >= 0) ss = L"dash_red_right.wav";
		else							ss = L"dash_red_right.wav";
	}
	CSoundManager::Instance().PlayPlayerSound(ss.c_str());
}

void CPlayer::SetDirectionKeyContext()
{
	auto fKeyLeftEnter = [&]() { m_curDirection.first = -1; };
	auto fKeyLeftStay = [&]() { if (m_curDirection.first == 0) m_curDirection.first = -1; };
	auto fKeyLeftExit = [&]() { if (m_curDirection.first == -1) m_curDirection.first = 0; };
	m_KeyContext->AddFunc(VK_LEFT, fKeyLeftEnter, EKeyActionState::Enter);
	m_KeyContext->AddFunc(VK_LEFT, fKeyLeftStay, EKeyActionState::Stay);
	m_KeyContext->AddFunc(VK_LEFT, fKeyLeftExit, EKeyActionState::Exit);

	auto fKeyRightEnter = [&]() { m_curDirection.first = +1; };
	auto fKeyRightStay = [&]() { if (m_curDirection.first == 0) m_curDirection.first = +1; };
	auto fKeyRightExit = [&]() { if (m_curDirection.first == +1) m_curDirection.first = 0; };
	m_KeyContext->AddFunc(VK_RIGHT, fKeyRightEnter, EKeyActionState::Enter);
	m_KeyContext->AddFunc(VK_RIGHT, fKeyRightStay, EKeyActionState::Stay);
	m_KeyContext->AddFunc(VK_RIGHT, fKeyRightExit, EKeyActionState::Exit);

	auto fKeyUpEnter = [&]() { m_curDirection.second = 1; };
	auto fKeyUpStay = [&]() { if (m_curDirection.second == 0) m_curDirection.second = 1; };
	auto fKeyUpExit = [&]() { if (m_curDirection.second == 1) m_curDirection.second = 0; };
	m_KeyContext->AddFunc(VK_UP, fKeyUpEnter, EKeyActionState::Enter);
	m_KeyContext->AddFunc(VK_UP, fKeyUpStay, EKeyActionState::Stay);
	m_KeyContext->AddFunc(VK_UP, fKeyUpExit, EKeyActionState::Exit);

	auto fKeyDownEnter = [&]() { m_curDirection.second = -1; };
	auto fKeyDownStay = [&]() { if (m_curDirection.second == 0) m_curDirection.second = -1; };
	auto fKeyDownExit = [&]() { if (m_curDirection.second == -1) m_curDirection.second = 0; };
	m_KeyContext->AddFunc(VK_DOWN, fKeyDownEnter, EKeyActionState::Enter);
	m_KeyContext->AddFunc(VK_DOWN, fKeyDownStay,  EKeyActionState::Stay);
	m_KeyContext->AddFunc(VK_DOWN, fKeyDownExit,  EKeyActionState::Exit);
}

void CPlayer::Boost(pos<int> _direction, double _duration, bool _bZoom)
{

	m_PlayerState = PS_ONBOOST;
	m_pBoostTimer->SetDuration(_duration);
	m_pBoostTimer->Restart();

	m_bOnBoost = true;

	m_boostDirection = _direction;
}

void CPlayer::WingardiumLeviosa()
{

	if (m_PlayerState != PS_ONFEATHER) {
		m_curVelocity = { 0,0 };
	}

	m_PlayerState = PS_ONFEATHER;
	m_pFeatherTimer->Restart();

	CSoundManager::Instance().PlayPlayerSound(L"feather_state_fast_loop.wav");


	m_bOnFeather = true;
}

void CPlayer::ReSpawn(pos<int> _respawnPos)
{
	Reset();
	_respawnPos.first  *= TILESIZE;
	_respawnPos.second *= TILESIZE;
	m_curPos = _respawnPos;
	m_pDashTimer->Reset();
	if (m_PlayerState!=PS_ONBOOST)
		m_PlayerState = PS_ONAIR;
	GetTransform()->SetPos(m_curPos);
	RefillDash();
	m_bOnFeather = false;
}

void CPlayer::CreateLongJumpTimer()
{
	m_plongJumpTimer = CreateTimerWithDuration(LONGJUMPTIME);
}

void CPlayer::CreateCoyoteTimeTimer()
{
	m_pCoyoteTimeTimer = CreateTimerWithDuration(COYOTETIME);
}

void CPlayer::CreateJumpBufferTimeTimer()
{
	m_pJumpBufferTimer = CreateTimerWithDuration(JUMPBUFFERTIME);
}

void CPlayer::CreateWallJumpTimer()
{
	m_pWallJumpTimer = CreateTimerWithDuration(WALLJUMPTIME);
}

void CPlayer::TiredBlink()
{
	auto _pSprite = m_pSprite->GetSprite();
	_pSprite->SetColor(D3DCOLOR_ARGB(255, 255, 255, 255));
	if (m_dStamina <= TIREDSTAMINA) {
		if (m_pTiredTimer->GetElapsedRatio() < 0.5) {
			_pSprite->SetColor(D3DCOLOR_ARGB(255, 255, 0, 0));
		}
	}
}

void CPlayer::CreateTiredTimer()
{
	m_pTiredTimer = CreateTimerWithDuration(TIREDBLINKDURATION, true);
}

void CPlayer::CreateDashTimer()
{
	auto _func = [&]() {
		if (m_dashDirection.second >= 0) {
			double mult = 160 * PIXEL;
			mult/= sqrt(m_curVelocity.first * m_curVelocity.first + m_curVelocity.second * m_curVelocity.second);
			m_curVelocity.first  *= mult;
			m_curVelocity.second *= mult;
			if (m_dashDirection.second < 0) m_curVelocity.second *= 0.75;
		}
		return;
	};
	m_pDashTimer = CreateTimerWithDuration(DASHTIME, false, _func,false);
}

void CPlayer::CreateBoostTimer()
{
	m_pBoostTimer = CreateTimerWithDuration(14.0f, false, [&]() {
		m_bOnBoost = false; 
	}, false);
}

void CPlayer::CreateFeatherTimer()
{
	m_pFeatherTimer = CreateTimerWithDuration(FEATHERTIME, false, [&]() {
		m_bOnFeather = false; 
		RefillDash();
	}, false);
}

void CPlayer::CreateFootSoundTimer()
{
	m_pFootSoundTimer = CreateTimerWithDuration(0.5f, true, [&]() {
		wstring ss = L"foot_09_moltenrock_0";
		ss += to_wstring(1 + rand() % 8);
		ss += L".wav";
		CSoundManager::Instance().PlayPlayerSound(ss.c_str());
	}, false);
}

void CPlayer::OnCollision(FCollisionInfo _CollisionInfo)
{
	if (_CollisionInfo.m_OBJID == TILE || _CollisionInfo.m_OBJID == BLOCK) {
		if (_CollisionInfo.m_Collisiontype == CF_Bottom) {
			Move(pos<int> {0, -_CollisionInfo.m_fY});
			if (m_PlayerState == PS_ONFEATHER) {
				m_curVelocity.second = -abs(m_curVelocity.second);
			}
			else if (m_PlayerState != PS_ONDEAD) {
				m_bOnGround = true;
				//m_PlayerState = PS_ONGROUND;
			}
		}
		else if (_CollisionInfo.m_Collisiontype == CF_Top) {
			if (m_PlayerState == PS_ONFEATHER) 
				m_curVelocity.second = +abs(m_curVelocity.second);
		}
		else if (_CollisionInfo.m_Collisiontype == CF_Left) {
			Move(pos<int> {+_CollisionInfo.m_fX, 0});
			if (m_PlayerState == PS_ONFEATHER)
				m_curVelocity.first = +abs(m_curVelocity.first);
			m_iContactWallSide = -1;
		}
		else if (_CollisionInfo.m_Collisiontype == CF_Right) {
			Move(pos<int> {-_CollisionInfo.m_fX, 0});
			if (m_PlayerState == PS_ONFEATHER)
				m_curVelocity.first = -abs(m_curVelocity.first);
			m_iContactWallSide = +1;
		}
	}
	if (_CollisionInfo.m_OBJID == TILEBLOCK) {
		if (dynamic_cast<CSemiBlock*>(_CollisionInfo.m_pObject) != nullptr) {
			Move(pos<int> {0, -_CollisionInfo.m_fY});
			if (m_PlayerState != PS_ONDEAD) {
				m_bOnGround = true;
				//m_PlayerState = PS_ONGROUND;
			} 
		}
	}
}

void CPlayer::CreatePlayerSprite()
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

CDynamicSprite* CPlayer::CreatePlayerGroundSprite()
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

CDynamicSprite* CPlayer::CreatePlayerWallSprite()
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

CDynamicSprite* CPlayer::CreatePlayerAirSprite()
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

CSprite* CPlayer::CreatePlayerBoostSprite()
{
	auto pPlayerBoostSprite = new CAnimationSprite;
	pPlayerBoostSprite->Load(PLAYERROCOMOTIONIMGPATH, L"Launch.png");
	pPlayerBoostSprite->Initialize();
	pPlayerBoostSprite->CreateTimer(0.7f);

	return pPlayerBoostSprite;
}

CSprite* CPlayer::CreatePlayerDeadSprite()
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
	pPlayerDeadSprite->SetExitFunc(0, bind(&CPlayer::Dead, this));

	return pPlayerDeadSprite;
}
