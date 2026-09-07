#include "pch.h"
#include "Boundary.h"
#include "Player.h"
#include "StaticSprite.h"
#include "CLavaBubble.h"
#include "BubblePop.h"

CBoundary::CBoundary() : CObject()
{
	m_bGimmicState = -1;
}

CBoundary::~CBoundary()
{
}

void CBoundary::Initialize()
{
	AddComponent<CRendererComponent>();
	CObject::Initialize();
	CreateSprite();
	SetCollision();
	GetTransform()->SetPos({WINCX>>1, WINCY});
	m_curSpeed = ONSCREENBOUNDARYSPEED;

	for (size_t i = 0; i < 100; i++) CreateBubble();
	SetKeyContext();
	SetActive(false);
}

void CBoundary::Update(double _dDeltaTime)
{
	bool _bGimmicState = CStageManager::Instance().GetGimmicState();
	if (m_bGimmicState != _bGimmicState) {
		CSoundManager::Instance().PlaySound((_bGimmicState) ? L"risinglava_loop.wav" : L"risingice_loop.wav", SC_BGM2, 0.5f, true);
	}
	m_bGimmicState = _bGimmicState;

	CreateBubble();
	if (!IsActive()) return;
	CObject::Update(_dDeltaTime);
	auto _pPlayer = dynamic_cast<CPlayer*>(CObjectManager::Instance().Get_Player());
	if (!_pPlayer) return;

	auto _playerPos = _pPlayer->GetTransform()->GetLocalPos();
	auto _curPos = GetTransform()->GetLocalPos();

	if (_playerPos.second > _curPos.second) _pPlayer->Abracadabra();
	
	double _ratio = pow(0.6, _dDeltaTime);
	if (IsOnScreen()) {
		m_curSpeed = (_ratio) * m_curSpeed + (1-_ratio) * ONSCREENBOUNDARYSPEED;
	}
	else {
		m_curSpeed = (_ratio)*m_curSpeed + (1 - _ratio) * OFFSCREENBOUNDARYSPEED;
	}

	_curPos.second -= m_curSpeed * _dDeltaTime;
	GetTransform()->SetPos(_curPos);

}

void CBoundary::LateUpdate(double _dDeltaTime)
{
	CObject::LateUpdate(_dDeltaTime);
}

void CBoundary::Release()
{
	CObject::Release();
}

void CBoundary::CreateSprite()
{
	auto _pSprite = new CStaticSprite;
	_pSprite->Load(ETCIMGPATH, L"Square.png");
	_pSprite->SetInScreenSize({ WINCX,WINCY });
	_pSprite->SetPivot({ 0.5, 0.0 });
	GetComponent<CRendererComponent>()->SetSprite(_pSprite);
}

void CBoundary::SetCollision()
{
	//AddComponent<CCollisionComponent>();
}

void CBoundary::OnCollision(FCollisionInfo _collisionInfo)
{
}

void CBoundary::CreateBubble()
{
	if (CLavaBubble::GetParticleCount() <= MAXBUBBLEPARTICLECNT) {
		auto _pParticle = CParticle::CreateParticle<CLavaBubble>(make_pair(rand() % WINCX - (WINCX >> 1), +0.4 * (rand() % WINCY) + 10), 1, { 0, -20 * PIXEL });
		_pParticle->SetParent(this);
		_pParticle->Initialize();
	}

	if (CBubblePop::GetParticleCount() <= MAXBUBBLEPOPPARTICLECNT) {
		auto _pParticle = CParticle::CreateParticle<CBubblePop>(make_pair(rand() % WINCX - (WINCX >> 1), 0), 1, { 0, 0 });
		_pParticle->SetParent(this);
		_pParticle->Initialize();
	}
}

void CBoundary::SetKeyContext()
{
	m_pKeyContext = CreateKeyContext();
	for (int _vkey : {VK_UP, VK_LEFT, VK_RIGHT, VK_DOWN, (int)'C', (int)'Z', (int)'X'})
		m_pKeyContext->AddFunc(_vkey, [&]() {SetActive(true); }, EKeyActionState::Enter);
}
