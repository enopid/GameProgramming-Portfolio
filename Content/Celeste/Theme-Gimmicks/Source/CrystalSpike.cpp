#include "pch.h"
#include "CrystalSpike.h"
#include "Player.h"


void CCrystalSpike::Initialize()
{
	CTile::Initialize();
	GetTransform()->SetPos({ m_TileIdx.first * m_iTileSize , m_TileIdx.second * m_iTileSize  });
}

void CCrystalSpike::Update(double _dDeltatime)
{
	CObject::Update(_dDeltatime);
	m_bGimmicState = CStageManager::Instance().GetGimmicState();
}

void CCrystalSpike::CreateTileSprite()
{
	SAFE_DELETE(m_pSprite)
	auto _pSprite = new CDynamicSprite;
	_pSprite->SetInScreenSize({ ((TILESIZE*3/4) << 2), ((TILESIZE * 3 / 4) << 2) });

	auto _pFireSprite = new CStaticSprite;
	_pFireSprite->Load(TILEBLOCKIMGPATH, L"CrystalSpike_Fire.png");
	_pFireSprite->SetCroppedSize({ (TILESIZE * 3 / 4), (TILESIZE * 3 / 4) });
	_pFireSprite->SetCropOffset({ (TILESIZE * 3 / 4)*m_iType,0 });


	auto _pIceSprite = new CStaticSprite;
	_pIceSprite->Load(TILEBLOCKIMGPATH, L"CrystalSpike_Ice.png");
	_pIceSprite->SetCroppedSize({ (TILESIZE * 3 / 4), (TILESIZE * 3 / 4) });
	_pIceSprite->SetCropOffset({ (TILESIZE * 3 / 4) * m_iType,0 });

	_pSprite->AddState(_pIceSprite);
	_pSprite->AddState(_pFireSprite);
	_pSprite->SetStateIdx(0);

	_pSprite->AddTransition(0, 1, [&]() {return m_bGimmicState;  });
	_pSprite->AddTransition(1, 0, [&]() {return !m_bGimmicState; });

	m_pSprite = _pSprite;
	GetComponent<CRendererComponent>()->SetSprite(m_pSprite);
}

void CCrystalSpike::SetCollision()
{
	AddComponent<CCollisionComponent>();
	auto _pCollisionComponent = GetComponent<CCollisionComponent>();
	_pCollisionComponent->SetSize({ (TILESIZE * 3 /4 * 2), (TILESIZE * 3 / 4 * 2) });
	_pCollisionComponent->SetPivot({ +0.5, +0.5 });

	_pCollisionComponent->SetOntriggerFunc(bind(&CCrystalSpike::OnCollision, this, placeholders::_1));
	_pCollisionComponent->SetRenderable(true);
}

void CCrystalSpike::OnCollision(FCollisionInfo _collisionInfo)
{
	if (_collisionInfo.m_OBJID == PLAYER) {
		auto _pPlayer = dynamic_cast<CPlayer*>(_collisionInfo.m_pObject);
		if (_pPlayer) _pPlayer->Abracadabra();
	}
}
