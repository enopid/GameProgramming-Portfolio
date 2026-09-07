#include "pch.h"
#include "FlipSwitch.h"
#include "Player.h"


void CFlipSwitch::Initialize()
{
	CTile::Initialize();
	m_bGimmicState = CStageManager::Instance().GetGimmicState();
}

void CFlipSwitch::Update(double _dDeltaTime)
{
	CObject::Update(_dDeltaTime);
	m_bGimmicState = CStageManager::Instance().GetGimmicState();
}

void CFlipSwitch::CreateTileSprite()
{
	SAFE_DELETE(m_pSprite)
	auto _pSprite = new CDynamicSprite;
	_pSprite->SetInScreenSize({ TILESIZE * 4, TILESIZE * 4 });
	_pSprite->SetPos({ TILESIZE >> 1 , TILESIZE >> 1 });

	auto _pActiveFireSprite		= new CAnimationSprite;
	_pActiveFireSprite->Load(OBJECTIMGPATH, L"Switch_Active_Fire.png");
	_pActiveFireSprite->CreateTimer(ACTIVEANIMATIONTIME);
	auto _pActiveIceSprite		= new CAnimationSprite;
	_pActiveIceSprite->Load(OBJECTIMGPATH, L"Switch_Active_Ice.png");
	_pActiveIceSprite->CreateTimer(ACTIVEANIMATIONTIME);

	auto _pTransitFireSprite	= new CAnimationSprite;
	_pTransitFireSprite->Load(OBJECTIMGPATH, L"Switch_Transit_Fire.png");
	_pTransitFireSprite->CreateTimer(TRANSITANIMATIONTIME);
	auto _pTransitIceSprite		= new CAnimationSprite;
	_pTransitIceSprite->Load(OBJECTIMGPATH, L"Switch_Transit_Ice.png");
	_pTransitIceSprite->CreateTimer(TRANSITANIMATIONTIME);

	auto _pDisActiveFireSprite	= new CDynamicSprite;
	{
		auto _pDisActiveFireTransitSprite = new CAnimationSprite;
		_pDisActiveFireTransitSprite->Load(OBJECTIMGPATH, L"Switch_Disactive_Transit_Fire.png");
		_pDisActiveFireTransitSprite->CreateTimer(DISACTIVEANIMATIONTIME);

		auto _pDisActiveFireIdleSprite = new CStaticSprite;
		_pDisActiveFireIdleSprite->Load(OBJECTIMGPATH, L"Switch_DisActive_Fire.png");

		_pDisActiveFireSprite->AddState(_pDisActiveFireTransitSprite);
		_pDisActiveFireSprite->AddState(_pDisActiveFireIdleSprite);
		_pDisActiveFireSprite->SetStateIdx(0);

		_pDisActiveFireSprite->AddTransition(0, 1, []() {return true; }, true);
	}

	auto _pDisActiveICeSprite	= new CDynamicSprite;
	{
		auto _pDisActiveIceTransitSprite = new CAnimationSprite;
		_pDisActiveIceTransitSprite->Load(OBJECTIMGPATH, L"Switch_Disactive_Transit_Ice.png");
		_pDisActiveIceTransitSprite->CreateTimer(DISACTIVEANIMATIONTIME);

		auto _pDisActiveIceIdleSprite = new CStaticSprite;
		_pDisActiveIceIdleSprite->Load(OBJECTIMGPATH, L"Switch_DisActive_Ice.png");

		_pDisActiveICeSprite->AddState(_pDisActiveIceTransitSprite);
		_pDisActiveICeSprite->AddState(_pDisActiveIceIdleSprite);
		_pDisActiveICeSprite->SetStateIdx(0);

		_pDisActiveICeSprite->AddTransition(0, 1, []() {return true; }, true);
	}
	
	_pSprite->AddState(_pActiveFireSprite);
	_pSprite->AddState(_pActiveIceSprite);
	_pSprite->AddState(_pTransitFireSprite);
	_pSprite->AddState(_pTransitIceSprite);
	_pSprite->AddState(_pDisActiveFireSprite);
	_pSprite->AddState(_pDisActiveICeSprite);
	

	_pSprite->AddTransition(0, 2, [&]() {m_bTransit = false; return !m_bGimmicState; });
	_pSprite->AddTransition(1, 3, [&]() {m_bTransit = false; return  m_bGimmicState; });
	_pSprite->AddTransition(4, 2, [&]() {m_bTransit = false; return !m_bGimmicState; });
	_pSprite->AddTransition(5, 3, [&]() {m_bTransit = false; return  m_bGimmicState; });

	_pSprite->AddTransition(2, 1, [&]() {m_bTransit = true; return  (m_iState & (1 << 0)); }, true);
	_pSprite->AddTransition(2, 5, [&]() {m_bTransit = true; return !(m_iState & (1 << 0)); }, true);

	_pSprite->AddTransition(3, 0, [&]() {m_bTransit = true; return  (m_iState & (1 << 1)); }, true);
	_pSprite->AddTransition(3, 4, [&]() {m_bTransit = true; return !(m_iState & (1 << 1)); }, true);


	m_bGimmicState = CStageManager::Instance().GetGimmicState();
	if (m_bGimmicState) //fire
	{
		if (m_iState & 2)
			_pSprite->SetStateIdx(0);
		else
			_pSprite->SetStateIdx(4);
	}
	else//ice
	{
		if (m_iState & 1)
			_pSprite->SetStateIdx(1);
		else
			_pSprite->SetStateIdx(5);
	}

	m_pSprite = _pSprite;

	GetComponent<CRendererComponent>()->SetSprite(m_pSprite);
}

void CFlipSwitch::SetCollision()
{
	AddComponent<CCollisionComponent>();
	auto _pCollisionComponent = GetComponent<CCollisionComponent>();
	_pCollisionComponent->SetSize({ TILESIZE * 2 , TILESIZE * 2 });
	_pCollisionComponent->SetPivot({ +0.25, +0.25 });

	_pCollisionComponent->SetOntriggerFunc(bind(&CFlipSwitch::OnCollision, this, placeholders::_1));
	_pCollisionComponent->SetRenderable(true);
}

void CFlipSwitch::OnCollision(FCollisionInfo _collisionInfo)
{
	if (_collisionInfo.m_OBJID == PLAYER) {
		auto _pPlayer = dynamic_cast<CPlayer*>(_collisionInfo.m_pObject);
		if (_pPlayer) {
			if ((m_iState & 2) && m_bGimmicState && !m_bTransit) // fire to ice
			{
				CStageManager::Instance().SetGimmicState(!m_bGimmicState);
				CSoundManager::Instance().PlayObjectSound(L"switch_gen_m.wav", 0.1f);
			}
			if ((m_iState & 1) && !m_bGimmicState && !m_bTransit) // ice to fire
			{
				CStageManager::Instance().SetGimmicState(!m_bGimmicState);
				CSoundManager::Instance().PlayObjectSound(L"switch_gen_m.wav", 0.1f);
			}
		}
	}
}

