#include "pch.h"
#include "BoundaryBlock.h"
#include "Player.h"
#include "StaticSprite.h"

void CBoundaryBlock::Initialize()
{
	AddComponent<CRendererComponent>();
	CObject::Initialize();
	m_initPos = { m_rect.left * TILESIZE, m_rect.top * TILESIZE };
	GetTransform()->SetPos(m_initPos);

	CreateSprite();;
	SetCollision();
}

void CBoundaryBlock::Update(double _dDeltaTime)
{
	CObject::Update(_dDeltaTime);
	m_bGimmicState = CStageManager::Instance().GetGimmicState();

	if (m_bGimmicState && (m_iType&1))//fire
		SetActive(true);
	else if (!m_bGimmicState && (m_iType & 2))//ice
		SetActive(true);
	else
		SetActive(false);

	GetComponent<CRendererComponent>()->SetActive(IsActive());
}

void CBoundaryBlock::LateUpdate(double _dDeltaTime)
{
}

void CBoundaryBlock::CreateSprite()
{
	SAFE_DELETE(m_pSprite)
	auto _pSprite = new CStaticSprite;
	_pSprite->Load(ETCIMGPATH, L"Square2.png");
	_pSprite->SetInScreenSize({ TILESIZE * m_size.first, TILESIZE * m_size.second });;
	_pSprite->SetPivot({ 0.0, 0.0 });

	m_pSprite = _pSprite;
	GetComponent<CRendererComponent>()->SetSprite(m_pSprite);
}

void CBoundaryBlock::OnCollision(FCollisionInfo _collisionInfo)
{
	if (_collisionInfo.m_OBJID == PLAYER) {
		auto _pPlayer = dynamic_cast<CPlayer*>(_collisionInfo.m_pObject);
		if (_pPlayer) _pPlayer->Abracadabra();
	}
}

void CBoundaryBlock::SetCollision()
{
	AddComponent<CCollisionComponent>();
	auto _pCollisionComponent = GetComponent<CCollisionComponent>();
	_pCollisionComponent->SetSize({ TILESIZE * m_size.first, TILESIZE * m_size.second });
	_pCollisionComponent->SetPivot({ 0.0, 0.0 });

	_pCollisionComponent->SetOntriggerFunc(bind(&CBoundaryBlock::OnCollision, this, placeholders::_1));
	_pCollisionComponent->SetRenderable(true);
}
