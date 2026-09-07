#include "pch.h"
#include "EditorScene.h"
#include "CTileSetTile.h"

#include "Spike.h"
#include "JumpPad.h"
#include "Wall.h"
#include "CrystalSpike.h"
#include "Bumper.h"
#include "SemiBlock.h"

#include "DashCrystal.h"
#include "FlipSwitch.h"
#include "Feather.h"

#include"Player.h"
#include "AbstractFactory.h"

#include "RespawnTrigger.h"

CEditorScene::CEditorScene() : CScene()
{
    m_bPressBoundaryResizerPos  = false;
    m_iCategory                 = TILE;
    m_iType                     = 0;
    m_iSubType                  = 0;
    m_iCurStage                 = 0;
}

CEditorScene::~CEditorScene()
{
}

void CEditorScene::Initialize()
{
    m_mapSize     = { 24 ,24  };
    MapOffset   = { 100,100 };

    vecTypeInfo[BACKGROUND]   = { make_pair(L"BG0", 1 ), make_pair(L"BG1", 1), make_pair(L"BG2", 1) };
    vecTypeInfo[BGTILE]       = { make_pair(L"CoreBG", 1) };
    vecTypeInfo[DECAL]        = { 
        make_pair(L"Ball", 1),
        make_pair(L"Capsule", 1),
        make_pair(L"Chain_Top", 1),
        make_pair(L"Chain_Mid", 1),
        make_pair(L"Chain_Bottom", 1),
        make_pair(L"Checkpoint", 1),
        make_pair(L"Column_Top", 1),
        make_pair(L"Column_Mid", 1),
        make_pair(L"Column_Bottom", 1),
        make_pair(L"Fossil", 1),
        make_pair(L"Heart", 1),
        make_pair(L"Heart_Bevel", 1),
        make_pair(L"Heart_Sphere", 1),
        make_pair(L"Machine", 1),
        make_pair(L"Shovel", 1),
        make_pair(L"Rock", 1),
        make_pair(L"LargeRock", 1),
        make_pair(L"Rope", 1),
        make_pair(L"Rubble", 1),
        make_pair(L"Stalagtite", 1),
        make_pair(L"Stump", 1)
    };
    vecTypeInfo[TILE]           = { make_pair(L"Core", 1), make_pair(L"Dirt", 1) };
    vecTypeInfo[TILEBLOCK]      = { make_pair(L"Spike", 1), make_pair(L"Gimmic_Wall", 1), make_pair(L"JumpPad", 1), make_pair(L"CrystalSpike", 4), make_pair(L"Bumper", 1), make_pair(L"SemiBlock", 6) };
    vecTypeInfo[BLOCK]          = { make_pair(L"Gimmic_Block", 1) };
    vecTypeInfo[OBJECT]         = { make_pair(L"DashCrystal", 1), make_pair(L"Flip_Switch", 3),make_pair(L"Feather", 1), make_pair(L"Gimmic_Ball", 1)};
    vecTypeInfo[TRIGGER]        = { make_pair(L"RespawnPos", 1), make_pair(L"RespawnTrigger", 1), make_pair(L"NextStage", 1), make_pair(L"DeadZone", 1) };

    m_RespawnPos = { 0,0 };
    SetKeyContext();
}

void CEditorScene::Update(double _deltaTime)
{
    // 카메라이동
    if (2 * MARGIN + m_mapSize.first * TILESIZE > WINCX)
        MapOffset.first = clamp(MapOffset.first - m_curDirection.first * (float)_deltaTime * SCROLLSPEED, -m_mapSize.first * TILESIZE - MARGIN + WINCX, 0 + MARGIN);
    else
        MapOffset.first = (WINCX - m_mapSize.first * TILESIZE) / 2.0;
    if (2 * MARGIN + m_mapSize.second * TILESIZE > WINCY)
        MapOffset.second    = clamp(MapOffset.second + m_curDirection.second * (float)_deltaTime * SCROLLSPEED, -m_mapSize.second * TILESIZE - MARGIN + WINCY, 0 + MARGIN);
    else
        MapOffset.second = (WINCY - m_mapSize.second * TILESIZE) / 2.0;

    CStageManager::Instance().SetCameraPos(MapOffset);
    
    //카테고리 선택
    switch (m_iCategory) {
    case BACKGROUND:
        m_sCategory = L"BackGround";
        m_bTile     = false;
        m_bTileSet  = false;
        break;
    case BGTILE:
        m_sCategory = L"BGTile";
        m_bTile     = true;
        m_bTileSet  = true;
        break;
    case DECAL:
        m_sCategory = L"Decal";
        m_bTile     = false;
        m_bTileSet  = false;
        break;
    case TILE:
        m_sCategory = L"Tile";
        m_bTile     = true;
        m_bTileSet  = true;
        break;
    case TILEBLOCK:
        m_sCategory = L"TileBlock";
        m_bTile     = true;
        m_bTileSet  = true;
        break;
    case BLOCK:
        m_sCategory = L"Block";
        m_bTile     = false;
        m_bTileSet  = false;
        break;
    case OBJECT:
        m_sCategory = L"Object";
        m_bTile     = true;
        m_bTileSet  = true;
        break;
    case TRIGGER:
        m_sCategory = L"Trigger";
        m_bTile     = true;
        m_bTileSet  = true;
        break;
    }

    //리사이저 설정
    UpdateBoundaryResizer();
}

void CEditorScene::LateUpdate(double _deltaTime)
{
}

void CEditorScene::Render()
{
    auto _pLine = CScreenManager::Instance().GetLine();
    auto _pFont = CScreenManager::Instance().GetFont();

    DrawGrid(_pLine);
    DrawMapBoundary(_pLine);
    if (m_bTile)
        DrawHighlightTile(_pLine, m_bTileSet);
    else {

    }
    DrawBoundaryResizer(_pLine);
    DrawTrigger(_pLine, _pFont);
    DrawUI(_pFont);
}

void CEditorScene::OnEnter()
{
    CScene::OnEnter();
    m_pKeyContext->SetActive(true);
}

void CEditorScene::OnExit()
{
    CScene::OnExit();
    m_pKeyContext->SetActive(false);
}

void CEditorScene::SetKeyContext()
{
    m_pKeyContext = CreateKeyContext(false);
    SetDirectionKeyContext();
    
    m_pKeyContext->AddFunc(VK_RIGHT, [&]() {m_iCurStage = (m_iCurStage + 1 + MAPSIZE) % MAPSIZE; }, EKeyActionState::Enter);
    m_pKeyContext->AddFunc(VK_LEFT,  [&]() {m_iCurStage = (m_iCurStage - 1 + MAPSIZE) % MAPSIZE; }, EKeyActionState::Enter);

    m_pKeyContext->AddFunc(VK_UP,   [&]() {m_iSubType  = (m_iSubType + 1 + vecTypeInfo[m_iCategory][m_iType].second) % vecTypeInfo[m_iCategory][m_iType].second; }, EKeyActionState::Enter);
    m_pKeyContext->AddFunc(VK_DOWN, [&]() {m_iSubType  = (m_iSubType - 1 + vecTypeInfo[m_iCategory][m_iType].second) % vecTypeInfo[m_iCategory][m_iType].second; }, EKeyActionState::Enter);

    m_pKeyContext->AddFunc(VK_LBUTTON, bind(&CEditorScene::AddEntity,this),     EKeyActionState::Stay);
    m_pKeyContext->AddFunc(VK_RBUTTON, bind(&CEditorScene::RemoveEntity,this),  EKeyActionState::Stay);

    m_pKeyContext->AddFunc(VK_F1, [&]() { SaveStage(); }, EKeyActionState::Enter);
    m_pKeyContext->AddFunc(VK_F2, [&]() { LoadStage(); }, EKeyActionState::Enter);

    int iNum = 0;
    for (int i : {BGTILE, DECAL, TILE, TILEBLOCK, BLOCK, OBJECT, TRIGGER})
    {
        m_pKeyContext->AddFunc(iNum +'0', [=]() {
            this->m_iCategory = (OBJID)(i); 
            this->m_iType %= this->vecTypeInfo[this->m_iCategory].size();
        }, EKeyActionState::Enter);
        iNum++;
    }

    m_pKeyContext->AddFunc(VK_SPACE, [&]() {m_iType = (m_iType + 1)%vecTypeInfo[m_iCategory].size(); }, EKeyActionState::Enter);
}


void CEditorScene::SetDirectionKeyContext()
{
    auto fKeyLeftEnter = [&]() { m_curDirection.first = -1; };
    auto fKeyLeftStay = [&]() { if (m_curDirection.first == 0) m_curDirection.first = -1; };
    auto fKeyLeftExit = [&]() { if (m_curDirection.first == -1) m_curDirection.first = 0; };
    m_pKeyContext->AddFunc('A', fKeyLeftEnter, EKeyActionState::Enter);
    m_pKeyContext->AddFunc('A', fKeyLeftStay, EKeyActionState::Stay);
    m_pKeyContext->AddFunc('A', fKeyLeftExit, EKeyActionState::Exit);

    auto fKeyRightEnter = [&]() { m_curDirection.first = +1; };
    auto fKeyRightStay = [&]() { if (m_curDirection.first == 0) m_curDirection.first = +1; };
    auto fKeyRightExit = [&]() { if (m_curDirection.first == +1) m_curDirection.first = 0; };
    m_pKeyContext->AddFunc('D', fKeyRightEnter, EKeyActionState::Enter);
    m_pKeyContext->AddFunc('D', fKeyRightStay, EKeyActionState::Stay);
    m_pKeyContext->AddFunc('D', fKeyRightExit, EKeyActionState::Exit);

    auto fKeyUpEnter = [&]() { m_curDirection.second = 1; };
    auto fKeyUpStay = [&]() { if (m_curDirection.second == 0) m_curDirection.second = 1; };
    auto fKeyUpExit = [&]() { if (m_curDirection.second == 1) m_curDirection.second = 0; };
    m_pKeyContext->AddFunc('W', fKeyUpEnter, EKeyActionState::Enter);
    m_pKeyContext->AddFunc('W', fKeyUpStay, EKeyActionState::Stay);
    m_pKeyContext->AddFunc('W', fKeyUpExit, EKeyActionState::Exit);

    auto fKeyDownEnter = [&]() { m_curDirection.second = -1; };
    auto fKeyDownStay = [&]() { if (m_curDirection.second == 0) m_curDirection.second = -1; };
    auto fKeyDownExit = [&]() { if (m_curDirection.second == -1) m_curDirection.second = 0; };
    m_pKeyContext->AddFunc('S', fKeyDownEnter, EKeyActionState::Enter);
    m_pKeyContext->AddFunc('S', fKeyDownStay,   EKeyActionState::Stay);
    m_pKeyContext->AddFunc('S', fKeyDownExit,   EKeyActionState::Exit);
}

void CEditorScene::DrawMapBoundary(LPD3DXLINE _pLine)
{

    _pLine->SetWidth(5.0);
    _pLine->Begin();
    D3DXVECTOR2 points[] = {
       { MapOffset.first                                , MapOffset.second },
       { MapOffset.first                                , MapOffset.second + m_mapSize.second * TILESIZE  },
       { MapOffset.first + m_mapSize.first * TILESIZE     , MapOffset.second + m_mapSize.second * TILESIZE  },
       { MapOffset.first + m_mapSize.first * TILESIZE     , MapOffset.second },
       { MapOffset.first                                , MapOffset.second },
    };

    _pLine->Draw(points, 5, D3DCOLOR_XRGB(0, 255, 0));
    _pLine->End();
}

pos<int> CEditorScene::GetNeighbourPosition(pos<int> _curPos, int _NDFlag)
{
    for (int i = 0; i < 9; i++)
        if (_NDFlag & (1 << i)) 
            return make_pair( _curPos.first + (-1 + i % 3), _curPos.second + (-1 + i / 3) );
    return { -1,-1 };
}

vector<pos<int>> CEditorScene::GetNeighbourPositions(pos<int> _curPos, int _NDFlag)
{
    vector<pos<int>> _vecNeighbourPos;
    for (int i = 0; i < 9; i++)
        if (_NDFlag & (1 << i))
            _vecNeighbourPos.push_back({ _curPos.first + (-1 + i % 3), _curPos.second + (-1 + i / 3) });
    return _vecNeighbourPos;
}

void CEditorScene::DrawGrid(LPD3DXLINE _pLine)
{
    _pLine->SetWidth(1.0);
    _pLine->Begin();
    for (int i = 0; i < m_mapSize.first; i++)
    {
        D3DXVECTOR2 points[] = {
            {MapOffset.first + TILESIZE * i     , MapOffset.second                           },
            {MapOffset.first + TILESIZE * i     , MapOffset.second + m_mapSize.second * TILESIZE},
        };
        _pLine->Draw(points, 2, D3DCOLOR_XRGB(0, 200, 0));
    }

    for (int i = 0; i < m_mapSize.second; i++)
    {
        D3DXVECTOR2 points[] = {
            {MapOffset.first                            , MapOffset.second + TILESIZE * i },
            {MapOffset.first + m_mapSize.first * TILESIZE , MapOffset.second + TILESIZE * i },
        };
        _pLine->Draw(points, 2, D3DCOLOR_XRGB(0, 200, 0));
    }
    _pLine->End();
    

    _pLine->SetWidth(2.0);
    _pLine->Begin();
    for (int i = 0; i < (m_mapSize.first >> 1); i++)
    {
        D3DXVECTOR2 points[] = {
            {MapOffset.first + 2 * TILESIZE * i     , MapOffset.second                           },
            {MapOffset.first + 2 * TILESIZE * i     , MapOffset.second + m_mapSize.second * TILESIZE},
        };
        _pLine->Draw(points, 2, D3DCOLOR_XRGB(200, 200, 200));
    }

    for (int i = 0; i < (m_mapSize.second >> 1); i++)
    {
        D3DXVECTOR2 points[] = {
            {MapOffset.first                                , MapOffset.second + 2 * TILESIZE * i },
            {MapOffset.first + m_mapSize.first * TILESIZE     , MapOffset.second + 2 * TILESIZE * i },
        };
        _pLine->Draw(points, 2, D3DCOLOR_XRGB(200, 200, 200));
    }
    _pLine->End();

}

void CEditorScene::DrawHighlightTile(LPD3DXLINE _pLine, bool _bTileset)
{
    pos<int> _posIdx = GetTileIdx(_bTileset);
    if (_posIdx == make_pair(-1, -1)) return;
    int _tileSize = (_bTileset) ? TILESIZE : 2 * TILESIZE;
    pos<int> _offset = { MapOffset.first + _tileSize * _posIdx.first + _tileSize / 2, MapOffset.second + _tileSize * _posIdx.second + _tileSize / 2 };

    _pLine->SetWidth(5.0);
    _pLine->Begin();
    D3DXVECTOR2 points[] = {
        {_offset.first + _tileSize * 0.5f, _offset.second + _tileSize * 0.5f},
        {_offset.first - _tileSize * 0.5f, _offset.second + _tileSize * 0.5f},
        {_offset.first - _tileSize * 0.5f, _offset.second - _tileSize * 0.5f},
        {_offset.first + _tileSize * 0.5f, _offset.second - _tileSize * 0.5f},
        {_offset.first + _tileSize * 0.5f, _offset.second + _tileSize * 0.5f},
    };

    _pLine->Draw(points, 5, D3DCOLOR_XRGB(255, 255, 255));
    _pLine->End();
}

void CEditorScene::DrawTrigger(LPD3DXLINE _pLine, LPD3DXFONT _pFont)
{
    pos<int> _offset = { MapOffset.first + TILESIZE * m_RespawnPos.first + TILESIZE / 2, MapOffset.second + TILESIZE * m_RespawnPos.second + TILESIZE / 2 };
    _pLine->SetWidth(5.0);
    _pLine->Begin();
    D3DXVECTOR2 points[] = {
        {_offset.first + TILESIZE * 0.5f, _offset.second + TILESIZE * 0.5f},
        {_offset.first - TILESIZE * 0.5f, _offset.second + TILESIZE * 0.5f},
        {_offset.first - TILESIZE * 0.5f, _offset.second - TILESIZE * 0.5f},
        {_offset.first + TILESIZE * 0.5f, _offset.second - TILESIZE * 0.5f},
        {_offset.first + TILESIZE * 0.5f, _offset.second + TILESIZE * 0.5f},
    };

    _pLine->Draw(points, 5, D3DCOLOR_XRGB(255, 255, 0));
    _pLine->End();

    RECT _rect = {  _offset.first - TILESIZE * 0.5f,  _offset.second - TILESIZE * 0.5f,
                            _offset.first + TILESIZE * 0.5f,  _offset.second + TILESIZE * 0.5f
    };

    _pFont->DrawTextW(nullptr, L"R", -1, &_rect, DT_CENTER | DT_VCENTER, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CEditorScene::DrawUI(LPD3DXFONT _pFont) {
    RECT _categoryRect  = { 10,  10, 210,  100 };
    RECT _typeRect      = { 210, 10, 410,  100 };
    RECT _subTypeRect   = { 410, 10, 610,  100 };
    RECT _mapSizeRect   = { 610, 10, 810,  100 };
    RECT _curStageRect  = { 810, 10, 1010,  100 };
    RECT _curCursorRect = { 1010, 10, 1210, 100 };
    
    _pFont->DrawTextW(nullptr, m_sCategory.c_str(), -1, &_categoryRect, DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 255, 255, 255));

    wstring stype  = L"Type : " + vecTypeInfo[m_iCategory][m_iType].first;
    _pFont->DrawTextW(nullptr, stype.c_str(),       -1, &_typeRect,     DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 255, 255, 255));

    wstring sSubType  = L"SubType : " + to_wstring(m_iSubType);
    _pFont->DrawTextW(nullptr, sSubType.c_str(),       -1, &_subTypeRect,     DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 255, 255, 255));

    wstring sSize  = L"Size : " + to_wstring(m_mapSize.first) + L" X " + to_wstring(m_mapSize.second);
    _pFont->DrawTextW(nullptr, sSize.c_str(),       -1, &_mapSizeRect,  DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 255, 255, 255));

    wstring sStage = L"CurStage : " + to_wstring(m_iCurStage);
    _pFont->DrawTextW(nullptr, sStage.c_str(), -1, &_curStageRect, DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 255, 255, 255));


    pos<int> _posIdx = GetTileIdx(true);
    wstring sCursor = L"CurCursor : " + to_wstring(_posIdx.first) + L" / " + to_wstring(_posIdx.second);
    _pFont->DrawTextW(nullptr, sCursor.c_str(), -1, &_curCursorRect, DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CEditorScene::DrawBoundaryResizer(LPD3DXLINE _pLine)
{
    pos<int> _offset = { MapOffset.first + m_mapSize.first * TILESIZE, MapOffset.second + m_mapSize.second * TILESIZE };

    _pLine->SetWidth(5.0);
    _pLine->Begin();
    D3DXVECTOR2 points[] = {
        {_offset.first + 0.f,                   _offset.second + 0.0f},
        {_offset.first + 0.f,                   _offset.second + BoundaryResizerSize},
        {_offset.first + BoundaryResizerSize,   _offset.second + BoundaryResizerSize},
        {_offset.first + BoundaryResizerSize,   _offset.second + 0.0f},
        {_offset.first + 0.f,                   _offset.second + 0.0f},
    };
    if (m_bPressBoundaryResizerPos)
        _pLine->Draw(points, 5, D3DCOLOR_XRGB(255, 255, 255));
    else
        _pLine->Draw(points, 5, D3DCOLOR_XRGB(200, 200, 200));
    _pLine->End();
}

void CEditorScene::UpdateBoundaryResizer()
{
    auto _mousePos = GetMousePos();
    pos<int> _offset = { MapOffset.first + m_mapSize.first * TILESIZE, MapOffset.second + m_mapSize.second * TILESIZE };
    m_bPressBoundaryResizerPos = false;
    if (CInputManager::Instance().IsKeyState(VK_LBUTTON, EKeyActionState::Stay)) {
        if (_mousePos.first > _offset.first && _mousePos.first <  _offset.first + BoundaryResizerSize &&
            _mousePos.second > _offset.second && _mousePos.second < _offset.second + BoundaryResizerSize) {
            m_bPressBoundaryResizerPos = true;
        }
    }

    if (m_bPressBoundaryResizerPos) {
        _mousePos.first  -= (int)MapOffset.first;
        _mousePos.second -= (int)MapOffset.second;
        m_mapSize.first   = clamp((int)(_mousePos.first  - BoundaryResizerSize / 2) / TILESIZE, 10, 400);
        m_mapSize.second  = clamp((int)(_mousePos.second - BoundaryResizerSize / 2) / TILESIZE, 10, 400);
    }
}

void CEditorScene::SaveStage()
{
    int result = MessageBoxA(
        NULL,                       // 부모 창 (없으면 NULL)
        "저장 하시겠습니까?", // 메시지 텍스트
        "확인 요청",                // 제목
        MB_OKCANCEL | MB_ICONQUESTION // 확인/취소 버튼 + 물음표 아이콘
    );
    if (result != IDOK) return;
    //SetRespawn
    CStageManager::Instance().SetMapSize(m_mapSize);
    CStageManager::Instance().SetRespawnPos(m_RespawnPos);
    CStageManager::Instance().SaveStage(m_iCurStage);
}

void CEditorScene::LoadStage()
{
    if (!CStageManager::Instance().LoadStage(m_iCurStage)) return;
    for (size_t i = 0; i < OBJ_END; i++) m_mapObject[i].clear();
    auto _StageInfo = CStageManager::Instance().GetStageInfo();
    m_mapSize    = _StageInfo.MapSize;
    m_RespawnPos = _StageInfo.RespawnPos;

    //respawn
    auto it = m_ObjList[BGTILE].begin();
    for (int i = 0; i < _StageInfo.BGTileCount; i++, it++)
        m_mapObject[BGTILE].insert({ _StageInfo.vecBGTiles[i].TileMapPos , (*it) });

    it      = m_ObjList[TILE].begin();
    for (int i = 0; i < _StageInfo.TileCount; i++, it++)
        m_mapObject[TILE].insert({ _StageInfo.vecTiles[i].TileMapPos ,      (*it) });

    it = m_ObjList[TILEBLOCK].begin();
    for (int i = 0; i < _StageInfo.TileBlockCount; i++, it++)
        m_mapObject[TILEBLOCK].insert({ _StageInfo.vecTileBlocks[i].TileMapPos ,      (*it) });

    it = m_ObjList[OBJECT].begin();
    for (int i = 0; i < _StageInfo.ObjectCount; i++, it++)
        m_mapObject[OBJECT].insert({ _StageInfo.vecObjects[i].MapPos ,      (*it) });

    it = m_ObjList[TRIGGER].begin();
    for (int i = 0; i < _StageInfo.TriggerCount; i++, it++)
        m_mapObject[TRIGGER].insert({ _StageInfo.vecTriggers[i].TileMapPos ,      (*it) });
}

pos<int> CEditorScene::GetMousePos()
{
    POINT pt = GetMouseCursor();

    return pos<int>(pt.x, pt.y);
}

pos<int> CEditorScene::GetTileIdx(bool _bTileset)
{
    auto _mousePos = GetMousePos();
    _mousePos.first  -= (int) MapOffset.first;
    _mousePos.second -= (int) MapOffset.second;
    if (_bTileset) {
        _mousePos.first     /= TILESIZE;
        _mousePos.second    /= TILESIZE;
    }
    else {
        _mousePos.first     /= 2 * TILESIZE;
        _mousePos.second    /= 2 * TILESIZE;
    }
    if (_mousePos.first < 0     || _mousePos.first  > (int)(((_bTileset) ? 1 : 0.5) * m_mapSize.first ) - 1) return { -1,-1 };
    if (_mousePos.second < 0    || _mousePos.second > (int)(((_bTileset) ? 1 : 0.5) * m_mapSize.second) - 1) return { -1,-1 };
    return _mousePos;
}

vector<pos<int>> GetNeighbourPos(pos<int> _curPos)
{
    vector<pos<int>> vecResult;
    for (int i = -1; i < 2; i++) for (int j = -1; j < 2; j++) {
        if (i == 0 && j == 0) continue;
		pos<int> _neighbourPos = { _curPos.first + i, _curPos.second + j };
        vecResult.push_back(_neighbourPos);
    }
	return vecResult;
}

void CEditorScene::UpdateNeighbourTile(pos<int> _tileMapPos, OBJID _objID, int _iType)
{
    queue<pos<int>> q;
    q.push(_tileMapPos);
    while (q.size()) {
        auto _curPos = q.front();
        q.pop();
        for(const auto& _neighbourPos : GetNeighbourPos(_curPos)) {
            if (m_mapObject[_objID].find(_neighbourPos) == m_mapObject[_objID].end()) continue;
            auto _neighbourTileSetTile = dynamic_cast<CTileSetTile*>(m_mapObject[_objID][_neighbourPos]);

            auto _neighbourTileSetType      = _neighbourTileSetTile->GetTileType();
            auto _neighbourCurTileSetPos    = _neighbourTileSetTile->GetTileSetIdx();
            auto _neighbourNxtTileSetPos    = GetTileSetPos(_neighbourPos, _objID, _neighbourTileSetType);

            if (!IsSameTileSet(_neighbourNxtTileSetPos, _neighbourCurTileSetPos)) {
                _neighbourTileSetTile->SetTileSetIdx(_neighbourNxtTileSetPos);
                if (_neighbourTileSetType == _iType)  q.push(_neighbourPos);
            }
        }
    }
}

void CEditorScene::UpdateNeighbourTileBlock(pos<int> _tileMapPos)
{
    switch (m_iType)
    {
    case 0://Spike
    {
    }
    break;
    case 1: //Wall
    {
        int iOffset;
        iOffset = 0;
        while (true) {
            pos<int> _neighbourPos = { _tileMapPos.first, _tileMapPos.second + (++iOffset) };
            if (m_mapObject[TILEBLOCK].find(_neighbourPos) != m_mapObject[TILEBLOCK].end()) {
                auto _neighbourTileSetTile = dynamic_cast<CTile*>(m_mapObject[TILEBLOCK][_neighbourPos]);
                if (_neighbourTileSetTile) {
                    _neighbourTileSetTile->SetDead(true);
                    AddTileBlock(_neighbourPos);
                }
            }
            else {
                break;
            }
        }
        iOffset = 0;
        while (true) {
            pos<int> _neighbourPos = { _tileMapPos.first, _tileMapPos.second - (++iOffset) };
            if (m_mapObject[TILEBLOCK].find(_neighbourPos) != m_mapObject[TILEBLOCK].end()) {
                auto _neighbourTileSetTile = dynamic_cast<CTile*>(m_mapObject[TILEBLOCK][_neighbourPos]);
                if (_neighbourTileSetTile) {
                    _neighbourTileSetTile->SetDead(true);
                    AddTileBlock(_neighbourPos);
                }
            }
            else {
                break;
            }
        }
    }
    break;
    default:
        break;
    }
}
void CEditorScene::AddTileBlock(pos<int> _tileMapPos) {
    CObject* _pObject =nullptr;
    switch (m_iType)
    {
    case 0://Spike
        {
            int _iDirection = 0;
            vector<pos<int>> _vecNeighbourPositions = { 
                GetNeighbourPosition(_tileMapPos, ND_BOTTOM), 
                GetNeighbourPosition(_tileMapPos, ND_TOP),
                GetNeighbourPosition(_tileMapPos, ND_LEFT),
                GetNeighbourPosition(_tileMapPos, ND_RIGHT)
            };
            for (size_t i = 0; i < 4; i++)
            {
                pos<int> _neighbourPos = _vecNeighbourPositions[i];
                if (m_mapObject[TILE].find(_neighbourPos) != m_mapObject[TILE].end()) {
                    _iDirection = i;
                    break;
                }
            }
            _pObject = new CSpike(_tileMapPos, _iDirection);
        }
        break;
    case 1: //Wall
        {
            bool _bDirection = 0;
            if      (m_mapObject[TILE].find(GetNeighbourPosition(_tileMapPos, ND_LEFT)) != m_mapObject[TILE].end())
                _bDirection = true;
            else if (m_mapObject[TILE].find(GetNeighbourPosition(_tileMapPos, ND_RIGHT)) != m_mapObject[TILE].end())
                _bDirection = false;

            int _iType = 0;
            if      (m_mapObject[TILEBLOCK].find(GetNeighbourPosition(_tileMapPos, ND_TOP))    == m_mapObject[TILEBLOCK].end())
                _iType += 1;
            if      (m_mapObject[TILEBLOCK].find(GetNeighbourPosition(_tileMapPos, ND_BOTTOM)) == m_mapObject[TILEBLOCK].end())
                _iType += 2;
            _iType %= 3;

            _pObject = new CWall(_tileMapPos, 2*_iType + _bDirection);
        }
        break;
    case 2://JumpPad
    {
        int _iDirection = 0;
        vector<pos<int>> _vecNeighbourPositions = {
            GetNeighbourPosition(_tileMapPos, ND_BOTTOM),
            GetNeighbourPosition(_tileMapPos, ND_TOP),
            GetNeighbourPosition(_tileMapPos, ND_LEFT),
            GetNeighbourPosition(_tileMapPos, ND_RIGHT)
        };
        for (size_t i = 0; i < 4; i++)
        {
            pos<int> _neighbourPos = _vecNeighbourPositions[i];
            if (m_mapObject[TILE].find(_neighbourPos) != m_mapObject[TILE].end()) {
                _iDirection = i;
                break;
            }
        }
        _pObject = new CJumpPad(_tileMapPos, _iDirection);
    }
    break;
    case 3://CrystalSpikes
    {
        _pObject = new CCrystalSpike(_tileMapPos, m_iSubType);
    }
    break;
    case 4://Bumper
    {
        _pObject = new CBumper(_tileMapPos, m_iSubType);
    }
    break;
    case 5://SemiBlock
    {
        _pObject = new CSemiBlock(_tileMapPos, m_iSubType);
    }
    break;
    default:
        break;
    }

    if(_pObject) m_mapObject[TILEBLOCK][_tileMapPos] = CreateObject(TILEBLOCK, _pObject);
}

void CEditorScene::AddObject(pos<int> _tileMapPos)
{
    CObject* _pObject = nullptr;
    switch (m_iType)
    {
    case 0://DashCrystal
    {
        _pObject = new CDashCrystal(_tileMapPos, 0);
        break;
    }
    case 1://Switch
    {
        _pObject = new CFlipSwitch(_tileMapPos, m_iSubType+1);
        break;
    }
    case 2://Feather
    {
        _pObject = new CFeather(_tileMapPos, 0);
        break;
    }
    break;
    default:
        break;
    }

    if (_pObject) m_mapObject[OBJECT][_tileMapPos] = CreateObject(OBJECT, _pObject);
}

void CEditorScene::AddTrigger(pos<int> _tileMapPos)
{
    CObject* _pObject = nullptr;
    switch (m_iType)
    {
    case 0://RespawnPos
        m_RespawnPos = _tileMapPos;
        break;
    case 1://RespawnTrigger}
        _pObject = new CRespawnTrigger(_tileMapPos,  1);
        break;
    default:
        break;
    }
    if (_pObject) m_mapObject[TRIGGER][_tileMapPos] = CreateObject(TRIGGER, _pObject);
}

void CEditorScene::AddEntity()
{
    if (m_bPressBoundaryResizerPos) return;

    pos<int> _mousePos = GetTileIdx(m_bTileSet);
    if (_mousePos == make_pair(-1, -1)) return;
    switch (m_iCategory) {
    case BACKGROUND:
        break;
    case BGTILE:
        if (m_mapObject[BGTILE].find(_mousePos) != m_mapObject[BGTILE].end()) {
            m_mapObject[BGTILE][_mousePos]->SetDead(true);
        }
        m_mapObject[BGTILE][_mousePos] = CreateObject(BGTILE, new CTileSetTile(_mousePos, GetTileSetPos(_mousePos, BGTILE, m_iType), m_iType));
        UpdateNeighbourTile(_mousePos, BGTILE, m_iType);
        break;
    case DECAL:
        break;
    case TILE:
        if (m_mapObject[TILE].find(_mousePos) != m_mapObject[TILE].end()) {
            m_mapObject[TILE][_mousePos]->SetDead(true);
        }
        m_mapObject[TILE][_mousePos] = CreateObject(TILE, new CTileSetTile(_mousePos, GetTileSetPos(_mousePos, TILE, vecTypeInfo[BGTILE].size() + m_iType), vecTypeInfo[BGTILE].size() + m_iType));
        UpdateNeighbourTile(_mousePos, TILE, vecTypeInfo[BGTILE].size() + m_iType);
        break;
    case TILEBLOCK:
        if (m_mapObject[TILEBLOCK].find(_mousePos) != m_mapObject[TILEBLOCK].end()) {
            m_mapObject[TILEBLOCK][_mousePos]->SetDead(true);
        }
        AddTileBlock(_mousePos);
        UpdateNeighbourTileBlock(_mousePos);
        break;
    case BLOCK:
        break;
    case OBJECT:
        if (m_mapObject[OBJECT].find(_mousePos) != m_mapObject[OBJECT].end()) {
            m_mapObject[OBJECT][_mousePos]->SetDead(true);
        }
        AddObject(_mousePos);
        break;
    case TRIGGER:
        if (m_mapObject[TRIGGER].find(_mousePos) != m_mapObject[TRIGGER].end()) {
            m_mapObject[TRIGGER][_mousePos]->SetDead(true);
        }
        AddTrigger(_mousePos);

        break;
    }
}

void CEditorScene::RemoveEntity()
{
    pos<int> _mousePos = GetTileIdx(m_bTileSet);
    if (_mousePos == make_pair(-1, -1)) return;
    switch (m_iCategory) {
    case BACKGROUND:
        break;
    case BGTILE:
        if (m_mapObject[BGTILE].find(_mousePos) != m_mapObject[BGTILE].end()) {
            m_mapObject[BGTILE][_mousePos]->SetDead(true);
            m_mapObject[BGTILE].erase(_mousePos);
        }
        UpdateNeighbourTile(_mousePos, BGTILE, m_iType);
        break;
    case DECAL:
        break;
    case TILE:
        if (m_mapObject[TILE].find(_mousePos) != m_mapObject[TILE].end()) {
            m_mapObject[TILE][_mousePos]->SetDead(true);
            m_mapObject[TILE].erase(_mousePos);
        }
        UpdateNeighbourTile(_mousePos, TILE, vecTypeInfo[BGTILE].size() + m_iType);
        break;
    case TILEBLOCK:
        if (m_mapObject[TILEBLOCK].find(_mousePos) != m_mapObject[TILEBLOCK].end()) {
            m_mapObject[TILEBLOCK][_mousePos]->SetDead(true);
            m_mapObject[TILEBLOCK].erase(_mousePos);
        }
        UpdateNeighbourTileBlock(_mousePos);
        break;
    case BLOCK:
        break;
    case OBJECT:
        if (m_mapObject[OBJECT].find(_mousePos) != m_mapObject[OBJECT].end()) {
            m_mapObject[OBJECT][_mousePos]->SetDead(true);
            m_mapObject[OBJECT].erase(_mousePos);
        }
        break;
    case TRIGGER:
        if (m_mapObject[TRIGGER].find(_mousePos) != m_mapObject[TRIGGER].end()) {
            m_mapObject[TRIGGER][_mousePos]->SetDead(true);
            m_mapObject[TRIGGER].erase(_mousePos);
        }
        break;
    }

}

pos<int> CEditorScene::GetTileSetPos(pos<int> _tileMapPos, OBJID _objID, int _iType)
{
    // Mask
    // 0 1 2
    // 3 4 5
    // 6 7 8

    int AdjanceyValidBlockMask = 0; // 유효한 인접 블록의 비트마스킹
	int AdjanceyInnerBlockMask = 0; // 유효한 인접 내부 블록의 비트마스킹
    for (int i = -1; i < 2; i++) for (int j = -1; j < 2; j++)
    {
        pos<int> _neighbourPos = { _tileMapPos.first + i, _tileMapPos.second + j };
        if (m_mapObject[_objID].find(_neighbourPos) != m_mapObject[_objID].end()) {
            auto _pNeighbourTileSetTile  = dynamic_cast<CTileSetTile*>(m_mapObject[_objID][_neighbourPos]);
            auto _neighbourTileSetIdx    = _pNeighbourTileSetTile->GetTileSetIdx();
            auto _neighbourTileType      = _pNeighbourTileSetTile->GetTileType();
            if (_iType == _neighbourTileType) {
                AdjanceyValidBlockMask |= 1 << (i + 1 + 3 * (j + 1));
                if (_neighbourTileSetIdx.first<=3)
                    AdjanceyInnerBlockMask |= 1 << (i + 1 + 3 * (j + 1));
            }
        }

        //맵 외곽은 유효한 블록으로 간주
        if (_neighbourPos.first  < 0    || _neighbourPos.first  >(m_mapSize.first  - 1) ||
            _neighbourPos.second < 0    || _neighbourPos.second >(m_mapSize.second - 1)
        ) AdjanceyValidBlockMask |= 1 << (i + 1 + 3 * (j + 1)); 
    }

    int _CrossMask  = (1 << 1) | (1 << 3) | (1 << 5) | (1 << 7);
    int _XMask      = (1 << 0) | (1 << 2) | (1 << 6) | (1 << 8);


    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 3) | (1 << 5) | (1 << 7)))   return pos<int>{0+rand()%4, 0};
    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 1) | (1 << 5) | (1 << 7)))   return pos<int>{0 + rand() % 4, 2};
    // 추가적인 Cross 마스크 체크는 생략...
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 0) | (1 << 2) | (1 << 6)))   return pos<int>{4, 0};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 0) | (1 << 6) | (1 << 8)))   return pos<int>{4, 1};
    // 추가적인 X 마스크 체크는 생략...


    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 3) | (1 << 5) | (1 << 7)))   return pos<int>{0 + rand() % 4, 0};
    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 1) | (1 << 3) | (1 << 5)))   return pos<int>{0+rand()%4, 1};

    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 1) | (1 << 5) | (1 << 7)))   return pos<int>{0+rand()%4, 2};
    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 1) | (1 << 3) | (1 << 7)))   return pos<int>{0+rand()%4, 3};

    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 3) | (1 << 5)))              return pos<int>{0+rand()%4, 4};
    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 1) | (1 << 7)))              return pos<int>{0+rand()%4, 5};


    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 7)))                         return pos<int>{0+rand()%4, 6};
    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 1)))                         return pos<int>{0+rand()%4, 7};
    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 5)))                         return pos<int>{0+rand()%4, 8};
    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 3)))                         return pos<int>{0+rand()%4, 9};

    if ((AdjanceyValidBlockMask & _CrossMask) == (0))                                return pos<int>{0+rand()%4, 10};

    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 5) | (1 << 7)))              return pos<int>{0+rand()%4, 11};
    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 3) | (1 << 7)))              return pos<int>{0+rand()%4, 12};
    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 1) | (1 << 5)))              return pos<int>{0+rand()%4, 13};
    if ((AdjanceyValidBlockMask & _CrossMask) == ((1 << 1) | (1 << 3)))              return pos<int>{0+rand()%4, 14};

    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 0) | (1 << 2) | (1 << 6)))   return pos<int>{4, 0};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 0) | (1 << 6) | (1 << 8)))   return pos<int>{4, 1};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 0) | (1 << 2) | (1 << 8)))   return pos<int>{4, 2};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 2) | (1 << 6) | (1 << 8)))   return pos<int>{4, 3};

    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 0) | (1 << 4)))              return pos<int>{4, 4};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 6) | (1 << 8)))              return pos<int>{4, 5};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 2) | (1 << 4)))              return pos<int>{4, 6};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 0) | (1 << 2)))              return pos<int>{4, 7};

    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 4)))                         return pos<int>{4, 8};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 8)))                         return pos<int>{4, 9};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 2)))                         return pos<int>{4, 10};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 0)))                         return pos<int>{4, 11};

    if ((AdjanceyValidBlockMask & _XMask) == (0))                                return pos<int>{4, 12};

    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 0) | (1 << 8)))              return pos<int>{4, 13};
    if ((AdjanceyValidBlockMask & _XMask) == ((1 << 2) | (1 << 6)))              return pos<int>{4, 14};

    //Special
    AdjanceyValidBlockMask = 0;
    for (int i = -1; i < 2; i++) for (int j = -1; j < 2; j++)
    {
        pos<int> _neighbourPos = { _tileMapPos.first + i*2, _tileMapPos.second + j*2 };
        if (m_mapObject[_objID].find(_neighbourPos) != m_mapObject[_objID].end())
            AdjanceyValidBlockMask |= 1 << (i + 1 + 3 * (j + 1));
    }

    if ((AdjanceyInnerBlockMask & (_XMask | _CrossMask)) == (0))   return pos<int>{5, 12 + rand()%3};

    return pos<int>{5, rand()%12};
}

FSceneID CEditorScene::CheckTransition()
{
    return FSceneID();
}
