#include "pch.h"
#include "BumpBlockManager.h"
#include "BumpBlock.h"
#include "Scene.h"

void CBumpBlockManager::Initialize()
{
	m_vecRects.resize(45, vector<RECT>());
	m_vecTypes.resize(45, vector<int>(10,0));
	//test
	{
		int _stage = 0;
		m_vecRects[_stage].push_back({ 34, 9 ,36,11 });
		m_vecTypes[_stage][0] = 8+1;
	}

	{
		int _stage = 6;
		m_vecRects[_stage].push_back({ 26,17,29,20 });
	}
	{
		int _stage = 9;
		m_vecRects[_stage].push_back({ 36,18,38,20 });
		m_vecRects[_stage].push_back({ 12, 17,14,19 });
	}
	{
		int _stage = 10;
		m_vecRects[_stage].push_back({ 11,10,13,12 });
		m_vecRects[_stage].push_back({ 44, 18,49,20 });
		
		m_vecTypes[_stage][0] = 1;
	}
	{
		int _stage = 11;
		m_vecRects[_stage].push_back({ 17,17,23,19 });
		m_vecRects[_stage].push_back({ 32, 17,38,19 });
	}
	{
		int _stage = 12;
		m_vecRects[_stage].push_back({ 22, 9 , 24, 12 });
		m_vecRects[_stage].push_back({ 65, 17, 68, 20 });
		m_vecRects[_stage].push_back({ 75, 16, 78, 19 });

		m_vecTypes[_stage][0] = 1;
	}
	{
		int _stage = 16;
		m_vecRects[_stage].push_back({ 17, 28 , 22, 30 });
	}
	{
		int _stage = 17;
		m_vecRects[_stage].push_back({ 36, 13, 48, 16 });
		m_vecRects[_stage].push_back({ 55,  7, 57, 15 });
		m_vecRects[_stage].push_back({ 75, 17, 79, 19 });
		m_vecRects[_stage].push_back({ 82, 12, 84, 16 });

		m_vecTypes[_stage][3] = 9;
	}
	{
		int _stage = 18;
		m_vecRects[_stage].push_back({ 62, 15, 69, 18 });
	}
	{
		int _stage = 20;
		m_vecRects[_stage].push_back({ 24, 13, 27, 15 });
		m_vecRects[_stage].push_back({ 37, 19, 40, 21 });
		m_vecRects[_stage].push_back({ 65, 18, 71, 20 });
	}
	{
		int _stage = 21;
		m_vecRects[_stage].push_back({ 80, 14, 82, 19 });

		m_vecTypes[_stage][0] = 9;
	}
	{
		int _stage = 27;
		m_vecRects[_stage].push_back({ 16, 28, 23, 31 });
		m_vecRects[_stage].push_back({  7, 35, 11, 37 });
		m_vecRects[_stage].push_back({ 28, 35, 32, 37 });
		m_vecRects[_stage].push_back({ 14, 37, 17, 45 });
		m_vecRects[_stage].push_back({ 22, 37, 25, 45 });
		m_vecRects[_stage].push_back({ 17, 49, 21, 51 });
	}
	{
		int _stage = 30;
		m_vecRects[_stage].push_back({ 27, 15, 29, 22 });
		m_vecRects[_stage].push_back({ 10, 25, 12, 28 });
		m_vecRects[_stage].push_back({ 27, 38, 29, 40 });
		m_vecRects[_stage].push_back({ 27, 45, 29, 47 });
		m_vecRects[_stage].push_back({ 10, 41, 12, 43 });
		m_vecRects[_stage].push_back({ 10, 48, 12, 50 });

		for (size_t i = 0; i < 6; i++)
			m_vecTypes[_stage][i] = 1;
	}
	{
		int _stage = 31;
		m_vecRects[_stage].push_back({ 26, 96, 31, 98 });
		m_vecRects[_stage].push_back({ 7, 144, 9, 148 });

		m_vecTypes[_stage][1] = 1;
	}
	{
		int _stage = 32;
		m_vecRects[_stage].push_back({ 118, 18, 122, 20 });
	}
	{
		int _stage = 35;
		m_vecRects[_stage].push_back({ 10, 16, 14, 18 });
	}
	{
		int _stage = 39;
		m_vecRects[_stage].push_back({  2, 39,  4, 47 });
		m_vecTypes[_stage][0] = 1;
	}
}

void CBumpBlockManager::Update(double _dDeltaTime)
{
}


void CBumpBlockManager::LateUpdate(double _dDeltaTime)
{
}

void CBumpBlockManager::SetStage(int _stage)
{
	m_iStage = _stage;
	Reset();
}

void CBumpBlockManager::Reset()
{
	vector<RECT> _Blocks = m_vecRects[m_iStage];
	vector<int>  _Types  = m_vecTypes[m_iStage];
	auto _pCurScene = CSceneManager::Instance().GetScene();
	if (_Blocks.empty()) return;
	int i(0);
	for (auto _rect : _Blocks) {
		auto _pBall = _pCurScene->CreateObject(BLOCK, new CBumpBlock(_rect, _Types[i++]));
	}
}

void CBumpBlockManager::Release()
{
}
