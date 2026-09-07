#include "pch.h"
#include "Hair.h"
#include "StaticSprite.h"

CHair::CHair()
{
}

CHair::~CHair()
{
}

void CHair::Initialize()
{
	CObject::Initialize();
	AddComponent<CRendererComponent>();
	CreateHairSprite();
}

void CHair::Update(double _dDeltaTime)
{
	CObject::Update(_dDeltaTime);
}

void CHair::LateUpdate(double _dDeltaTime)
{
	CObject::LateUpdate(_dDeltaTime);
}

void CHair::Release()
{
}

void CHair::SetScale(double _scale)
{
	m_pSprite->SetScale(_scale);
}

void CHair::SetReverse(bool* _bReverse)
{
	m_pSprite->SetReverse(_bReverse);
}

void CHair::SetHairColor(D3DCOLOR _color)
{
	m_pSprite->SetColor(_color);
}

void CHair::CreateHairSprite()
{
	auto _pRenderComopnent = GetComponent<CRendererComponent>();
	if (!_pRenderComopnent) return;

	auto pHairSprite = new CStaticSprite();
	_pRenderComopnent->SetSprite(pHairSprite);
	pHairSprite->Load(PLAYERHAIRIMGPATH, L"hair00.png");

	//pHairSprite->SetPos({ 0, +5 * PIXEL });
	pHairSprite->SetScale(PIXEL);
	pHairSprite->Initialize();

	m_pSprite = pHairSprite;
}
