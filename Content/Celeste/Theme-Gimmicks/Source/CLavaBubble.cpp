#include "pch.h"
#include "CLavaBubble.h"

CLavaBubble::CLavaBubble(int m_iType) : CParticle(m_iType)
{
	PARTICLECNT++;
}

CLavaBubble::~CLavaBubble()
{
	Release();
}

void CLavaBubble::Release()
{
	PARTICLECNT--;
}

void CLavaBubble::Initialize()
{
	CObject::Initialize();
}

void CLavaBubble::Update(double _delata)
{
	CObject::Update(_delata);

	if (!CStageManager::Instance().GetGimmicState()) {
		m_pLifeCycleTimer->SetActive(false);
		return;
	}
	else {
		m_pLifeCycleTimer->SetActive(true);
	}
	auto _pos = GetTransform()->GetLocalPos();
	_pos.second += m_initVelocity.second * _delata;

	if ((GetTransform()->GetLocalPos().second - m_initVelocity.first) * (2 * m_iType - 1) < 10) {
		SetDead(true);
		m_pLifeCycleTimer->SetActive(false);
		;
	}
	if (m_pLifeCycleTimer->IsActive()) {
		auto _ratio = m_pLifeCycleTimer->GetElapsedRatio();
		_pos.first = m_initPos.first;
		_pos.first += 2 * PIXEL * cos(TWO_PI * _ratio * 4);
	}
	GetTransform()->SetPos(_pos);
}

void CLavaBubble::CreateSprite()
{
	auto _pSprite = new CStaticSprite;
	_pSprite->Load(PARTICLEIMGPATH, L"bubble.png");
	_pSprite->Initialize();
	_pSprite->SetScale(PIXEL);
	m_pSprite = _pSprite;

	m_pSprite->SetColor(D3DCOLOR_ARGB(int(0.7 * (rand() % 255)), 255, 255, 255));

	auto _pComponent = GetComponent<CRendererComponent>();
	_pComponent->SetSprite(m_pSprite);
}

void CLavaBubble::CreateLifeCycleTimer()
{
	m_pLifeCycleTimer = CreateTimerWithDuration(LIFETIME, false, [&]() {
		SetDead(true);
		});
}