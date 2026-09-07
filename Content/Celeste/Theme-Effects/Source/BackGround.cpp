#include "pch.h"
#include "BackGround.h"
#include "StaticSprite.h"
#include "DynamicSprite.h"

void CBackground::Update(double _dDeltaTime)
{
	CObject::Update(_dDeltaTime);
	m_bGimmicState = CStageManager::Instance().GetGimmicState();
	
	if (CStageManager::Instance().GetCurStage() == 40) {
		CObjectManager::Instance().SetObjectTimeScale(0.7);
		auto _pos    = CStageManager::Instance().GetCameraPos();
		auto _ratio = 1 + _pos.first / (CStageManager::Instance().GetStageInfo().MapSize.first * TILESIZE - WINCX);
		double _mult = _ratio;
		switch (m_iBGType)
		{
		case 0:
			m_pSprite->GetSprite()->SetColor(D3DCOLOR_ARGB(255, int(255 * _mult), int(255 * _mult), int(255 * _mult)));
			break;
		case 1:
			_mult += 0.3;
			_mult = min(_mult, 1.0);
			m_pSprite->GetSprite()->SetColor(D3DCOLOR_ARGB(255, int(255 * _mult), int(255 * _mult), int(255 * _mult)));
			break;
		case 2:
			_mult += 0.3;
			_mult = min(_mult, 1.0);
			m_pSprite->GetSprite()->SetColor(D3DCOLOR_ARGB(255, int(255 * _mult), int(255 * _mult), int(255 * _mult)));
			break;
		default:
			break;
		}
	}


	SetOffset();
}

void CBackground::SetOffset()
{
	auto _pos = CStageManager::Instance().GetCameraPos();

	double _mult = 1.0;
	if (CStageManager::Instance().GetCurStage() == 40) {
		_mult = 3.0;
	}
	_pos.first  *= BGSENSITIVITY[m_iBGType] * _mult;
	_pos.second *= BGSENSITIVITY[m_iBGType] * _mult;

	while (_pos.first < 0)		_pos.first  += WINCX;
	while (_pos.first > WINCX)	_pos.first  -= WINCX;
	while (_pos.second < 0)		_pos.second += WINCY;
	while (_pos.second > WINCY)	_pos.second -= WINCY;
	 
	m_pSprite->SetPos(_pos);
}

void CBackground::CreateTileSprite()
{
	auto _pSprite = new CDynamicSprite;
	_pSprite->SetInScreenSize({ WINCX<<1, WINCY<<1 });
	_pSprite->SetPos({ WINCX>>1, WINCY>>1 });

	auto _pFireSprite = new CStaticSprite;
	_pFireSprite->Load(BACKGROUNDIMGPATH, (L"BG" + to_wstring(m_iBGType) + L"_Fire.png").c_str());
	auto _pIceSprite  = new CStaticSprite;
	_pIceSprite->Load(BACKGROUNDIMGPATH,  (L"BG" + to_wstring(m_iBGType) + L"_Ice.png" ).c_str());

	_pSprite->AddState(_pFireSprite);
	_pSprite->AddState(_pIceSprite);

	_pSprite->AddTransition(0, 1, [&]() {return !m_bGimmicState; });
	_pSprite->AddTransition(1, 0, [&]() {return m_bGimmicState; });

	_pSprite->SetStateIdx(0);

	m_pSprite = _pSprite;
	GetComponent<CRendererComponent>()->SetSprite(m_pSprite);
}
