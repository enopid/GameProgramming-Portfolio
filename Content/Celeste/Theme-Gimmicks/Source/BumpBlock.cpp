#include "pch.h"
#include "BumpBlock.h"
#include "Player.h"
#include "Spike.h"
#include "Scene.h"

void CBumpBlock::Initialize()
{
	AddComponent<CRendererComponent>();
	CObject::Initialize();
	m_initPos = { m_rect.left * TILESIZE, m_rect.top * TILESIZE };
	GetTransform()->SetPos(m_initPos);
	
	CreateSprite();
	CreateTiles();
	SetCollision();
	
	CreateBumpFireTimer();
	CreateBumpIceTimer();
	CreateBumpRespawnTimer();
	CreateActiveTimer();
	m_LiftTImer = CreateTimerWithDuration(2.0f, false, [&]() {m_bOnLift = false; }, false);
	m_bActive = false;
}

void CBumpBlock::Update(double _dDeltaTime)
{
	CObject::Update(_dDeltaTime);
	m_bGimmicState = CStageManager::Instance().GetGimmicState();
	

	if (m_pBumpFireTimer->IsActive()) {
		auto _curTIme = m_pBumpFireTimer->GetElapsedTime();

		pos<int> _prevPos = GetTransform()->GetLocalPos();

		pos<int> _nxtPos = m_initPos;
		int _offset = (int) (GetBumpOffset(_curTIme) * ((m_iDirection.first == 0 || m_iDirection.second == 0) ? 1 : sqrt(0.5))) / PIXEL * PIXEL;
		_nxtPos.first  -= m_iDirection.first  * _offset;
		_nxtPos.second += m_iDirection.second * _offset;

		//PrintDebug("POS : ", _nxtPos.first, L" ", _nxtPos.second);
		pos<int> _Posoffset = { _nxtPos.first - _prevPos.first, _nxtPos.second - _prevPos.second};

		auto _pPlayer = dynamic_cast<CPlayer*>(CObjectManager::Instance().Get_Player());
		
		
		if (m_bOnLift || _pPlayer->IsOnWall())
			_pPlayer->Move(_Posoffset);
		else
			m_pActiveTimer->SetActive(false);
		GetTransform()->SetPos(_nxtPos);
		
	}
	else if (m_pBumpIceTimer->IsActive()) {
		auto _ratio = m_pBumpIceTimer->GetElapsedRatio();
		pos<int> _nxtPos = m_initPos;
		int _offset = (int)(_ratio * 10 * PIXEL) / PIXEL * PIXEL;
		_nxtPos.second += _offset;
		GetTransform()->SetPos(_nxtPos);
	}

	else {
		GetTransform()->SetPos(m_initPos);
	}
}

void CBumpBlock::Collapse()
{
	GetComponent<CRendererComponent>()->SetActive(false);
	for (auto _pObject : GetChilden()) {
		_pObject->GetComponent<CRendererComponent>()->SetActive(false);
		auto _pos = _pObject->GetTransform()->GetLocalPos();
		_pos.first += GetTransform()->GetLocalPos().first;
		_pos.second += GetTransform()->GetLocalPos().second;
		if (m_bGimmicState) {
			pos<double> _vel = { 50 * PIXEL * cos(rand()), 50 * PIXEL * sin(rand()) };
			_vel.first  += m_iDirection.first  * 200 * PIXEL;
			_vel.second -= m_iDirection.second * 200 * PIXEL;
			CParticle::CreateParticle<CDebris>(_pos, 1, _vel);
		}
		else {
			pos<double> _vel = {20 * PIXEL * cos(rand()),20 * PIXEL * sin(rand()) };
			CParticle::CreateParticle<CDebris>(_pos, 0, _vel);
		}
	}
	m_bActive = false;
	m_pActiveTimer->SetActive(false);


	CSoundManager::Instance().PlaySound(L"bounceblock_break.wav", SC_GIMMICSFX3, 0.5f);
}

void CBumpBlock::Regenerate()
{
	GetComponent<CRendererComponent>()->SetActive(true);
	for (auto _pObject : GetChilden()) {
		_pObject->GetComponent<CRendererComponent>()->SetActive(true);
	}
	if (m_bGimmicState) CSoundManager::Instance().PlaySound(L"bounceblock_reappear.wav", SC_GIMMICSFX3, 0.5f);
	else {
		wstring ss = L"iceblock_reappear_0";
		ss += to_wstring(1 + rand() % 3);
		ss += L".wav";
		CSoundManager::Instance().PlaySound(ss.c_str(), SC_GIMMICSFX3, 0.5f);
	}
}
void CBumpBlock::LateUpdate(double _dDeltaTime)
{
}

void CBumpBlock::CreateSprite()
{
	SAFE_DELETE(m_pSprite)
	auto _pSprite = new CDynamicSprite;
	_pSprite->SetInScreenSize({ TILESIZE*6/4, TILESIZE*6/4 });
	_pSprite->SetPos(m_centerOffset);

	auto _pIceSprite = new CAnimationSprite;
	_pIceSprite->Load(BLOCKIMGPATH, L"BumpBlock_Center_Ice.png", 12);
	_pIceSprite->CreateTimer(ICEANIMATIONTIME);

	auto _pFireIdleSprite = new CAnimationSprite;
	_pFireIdleSprite->Load(BLOCKIMGPATH, L"BumpBlock_Center_Fire.png", 7);
	_pFireIdleSprite->CreateTimer(FIREANIMATIONTIME);

	_pSprite->AddState(_pIceSprite);
	_pSprite->AddState(_pFireIdleSprite);

	_pSprite->SetStateIdx(0);
	_pSprite->AddTransition(-1, 1, [&]() { return  m_bGimmicState; });
	_pSprite->AddTransition(-1, 0, [&]() { return !m_bGimmicState; });

	m_pSprite = _pSprite;
	GetComponent<CRendererComponent>()->SetSprite(m_pSprite);
}

void CBumpBlock::CreateTiles()
{
	auto _pScene = CSceneManager::Instance().GetScene();
	for (int i = 0; i < m_size.first; i++) for (int j = 0; j < m_size.second; j++)
	{
		pos<int> _pos = { TILESIZE >> 1, TILESIZE >> 1 };
		_pos.first  += i * TILESIZE;
		_pos.second += j * TILESIZE;

		int _type = 0;
		if (j == 0) {
			if		(i == 0)				_type = 0;
			else if (i == m_size.first - 1) _type = 2;
			else							_type = 1;
		}
		else if (j == m_size.second -1) {
			if (i == 0)						_type = 6;
			else if (i == m_size.first - 1) _type = 8;
			else							_type = 7;
		}
		else  {
			if (i == 0)						_type = 3;
			else if (i == m_size.first - 1) _type = 5;
			else							_type = 4;
		}


		CObject* _pObject = _pScene->CreateObject(BUMPBLOCKTILE, new CBumpBlockTile(_type));
		_pObject->GetTransform()->SetPos(_pos);
		_pObject->SetParent(this);
	}

	if (m_iType & (1 << 0)) { //top
		for (int i = 0; i < m_size.first; i++)
		{
			pos<int> _pos = {i, -1};
			CObject* _pObject = _pScene->CreateObject(TILEBLOCK, new CSpike(_pos, 0));
			_pObject->SetParent(this);
		}
	}
	if (m_iType & (1 << 1)) { //right
		for (int i = 0; i < m_size.second; i++)
		{
			pos<int> _pos = { m_size.first, i };
			CObject* _pObject = _pScene->CreateObject(TILEBLOCK, new CSpike(_pos, 2));
			_pObject->SetParent(this);
		}

	}
	if (m_iType & (1 << 2)) { //bottom
		for (int i = 0; i < m_size.first; i++)
		{
			pos<int> _pos = { i, m_size.second };
			CObject* _pObject = _pScene->CreateObject(TILEBLOCK, new CSpike(_pos, 1));
			_pObject->SetParent(this);
		}

	}
	if (m_iType & (1 << 3)) { //left
		for (int i = 0; i < m_size.second; i++)
		{
			pos<int> _pos = { -1, i };
			CObject* _pObject = _pScene->CreateObject(TILEBLOCK, new CSpike(_pos, 3));
			_pObject->SetParent(this);
		}
	}

}

void CBumpBlock::OnCollision(FCollisionInfo _collisionInfo)
{
	if (_collisionInfo.m_OBJID == PLAYER) {
		auto _pPlayer = dynamic_cast<CPlayer*>(_collisionInfo.m_pObject);
		/*{
			if (_collisionInfo.m_Collisiontype == CF_Bottom) {
				_pPlayer->Move(pos<int> {0, +_collisionInfo.m_fY});
			}
			else if (_collisionInfo.m_Collisiontype == CF_Top) {
				_pPlayer->Move(pos<int> {0, -_collisionInfo.m_fY});

			}
			else if (_collisionInfo.m_Collisiontype == CF_Left) {
				_pPlayer->Move(pos<int> {-_collisionInfo.m_fX, 0});
			}
			else if (_collisionInfo.m_Collisiontype == CF_Right) {
				_pPlayer->Move(pos<int> {+_collisionInfo.m_fX, 0});
			}

		}*/
		_pPlayer->SetOnlift();
		m_bOnLift = true;
		m_LiftTImer->Restart();
		if (!m_bActive && !m_pActiveTimer->IsActive()) m_pActiveTimer->Restart();
		if (!m_bActive) return;
		if (m_bGimmicState) {	//fire : ¹üÆÛ
			if (_pPlayer) {
				if (!m_pBumpFireTimer->IsActive()) {
					switch (_collisionInfo.m_Collisiontype)
					{
					case CF_Top:
						m_pBumpFireTimer->Restart();
						m_iDirection = { 0, 1 };
						break;
					case CF_Left:
						m_pBumpFireTimer->Restart();
						m_iDirection = { -1, 0 };
						break;
					case CF_Right:
						m_pBumpFireTimer->Restart();
						m_iDirection = { +1, 0 };
						break;
					default:
						m_iDirection = { 0, 0 };
						break;
					}
				}
				else {
					auto _pPlayer = dynamic_cast<CPlayer*>(CObjectManager::Instance().Get_Player());
					auto _ratio = m_pBumpFireTimer->GetElapsedRatio();
					if (_ratio > 0.5) {
						auto _direction = m_iDirection;
						//auto _playerDirection = _pPlayer->GetDirection();
						//if (_direction.first == 0) _direction.second = _playerDirection.second;
						double _mult = 0.5 + min((_ratio - 0.5)*2, 0.5) * ((_direction.first==0 || _direction.second == 0) ? 1 : sqrt(0.5));
						_pPlayer->SetLiftBoost({ 300 * _mult * _direction.first * PIXEL, -300 * _mult * _direction.second * PIXEL });
					}
				}
			}
		}
		else {					//ice  : ¼Ò¸ê
			if (_pPlayer) {
				if (!m_pBumpIceTimer->IsActive()) {
					m_pBumpIceTimer->Restart();
				}
			}
		}
	}
}

void CBumpBlock::SetCollision()
{
	AddComponent<CCollisionComponent>();
	auto _pCollisionComponent = GetComponent<CCollisionComponent>();
	_pCollisionComponent->SetSize({ TILESIZE * m_size.first, TILESIZE * m_size.second });
	_pCollisionComponent->SetPivot({ 0.0, 0.0 });

	_pCollisionComponent->SetOntriggerFunc(bind(&CBumpBlock::OnCollision, this, placeholders::_1));
	_pCollisionComponent->SetRenderable(true);
}

void CBumpBlock::CreateBumpFireTimer()
{
	m_pBumpFireTimer = CreateTimerWithDuration(2 * SINKTIME, false, [&]() {
		Collapse();
		SetActive(false); 
		m_pRespawnTimer->Restart(); 
		}, false);
}

void CBumpBlock::CreateBumpIceTimer()
{
	m_pBumpIceTimer = CreateTimerWithDuration(ICESINKTIME, false, [&]() {
		Collapse(); 
		SetActive(false);
		m_pRespawnTimer->Restart(); 
	}, false);
}

void CBumpBlock::CreateBumpRespawnTimer()
{
	m_pRespawnTimer = CreateTimerWithDuration(BLOCKRESPAWNTIME, false, [&](){
		Regenerate();
		SetActive(true);
	}, false);
}

void CBumpBlock::CreateActiveTimer()
{
	m_pActiveTimer = CreateTimerWithDuration(BLOCKACTIVETIME, false, [&]() {
		m_bActive = true;

		if(m_bGimmicState) CSoundManager::Instance().PlaySound(L"bounceblock_touch.wav", SC_GIMMICSFX3, 0.5f);
		else {
			wstring ss = L"iceblock_touch_0";
			ss += to_wstring(1 + rand() % 3);
			ss += L".wav";
			CSoundManager::Instance().PlayPlayerSound(ss.c_str());
		}
	}, false);
}

void CBumpBlockTile::Initialize()
{
	AddComponent<CRendererComponent>();
	CObject::Initialize();

	auto _pSprite = new CDynamicSprite;
	_pSprite->SetInScreenSize({ TILESIZE, TILESIZE });

	pos<int> _cropoffset = { 0,0 };
	switch (m_type) {
	case 0:
		_cropoffset = { 0,0 };
		break;
	case 1:
		_cropoffset = { ((rand() % 6) + 1)*8,0 };
		break;
	case 2:
		_cropoffset = { 7*8,0 };
		break;
	case 3:
		_cropoffset = { 0, ((rand() % 6) + 1) * 8 };
		break;
	case 4:
		_cropoffset = { ((rand() % 6) + 1) * 8,((rand() % 6) + 1) * 8 };
		break;
	case 5:
		_cropoffset = { 7 * 8, ((rand() % 6) + 1) * 8 };
		break;
	case 6:
		_cropoffset = { 0,7*8 };
		break;
	case 7:
		_cropoffset = { ((rand() % 6) + 1) * 8, 7*8 };
		break;
	case 8:
		_cropoffset = { 7*8,7*8 };
		break;
	}

	auto _pIceSprite = new CStaticSprite;
	_pIceSprite->Load(BLOCKIMGPATH, L"BumpBlock_ice.png");
	_pIceSprite->SetCroppedSize({ 8,8 });
	_pIceSprite->SetCropOffset(_cropoffset);
	auto _pFireSprite = new CStaticSprite;
	_pFireSprite->Load(BLOCKIMGPATH, L"BumpBlock_fire.png");
	_pFireSprite->SetCroppedSize({ 8,8 });
	_pFireSprite->SetCropOffset(_cropoffset);

	_pSprite->AddState(_pIceSprite);
	_pSprite->AddState(_pFireSprite);

	_pSprite->SetStateIdx(0);
	_pSprite->AddTransition(-1, 1, [&]() { return  m_bGimmicState; });
	_pSprite->AddTransition(-1, 0, [&]() { return !m_bGimmicState; });

	GetComponent<CRendererComponent>()->SetSprite(_pSprite);
}

void CBumpBlockTile::Update(double _dDeltaTime)
{
	CObject::Update(_dDeltaTime);
	m_bGimmicState = CStageManager::Instance().GetGimmicState();
}
