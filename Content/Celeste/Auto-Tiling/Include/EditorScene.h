#pragma once
#include "Scene.h"

class CEditorScene : public CScene
{
    constexpr static int MAPSIZE = 45;

    enum ENeighbourDirection {
        ND_NONE     = 0b000'000'000,

        ND_LT       = 0b000'000'001,
        ND_TOP      = 0b000'000'010,
        ND_RT       = 0b000'000'100,
        ND_LEFT     = 0b000'001'000,
        ND_MIDDLE   = 0b000'010'000,
        ND_RIGHT    = 0b000'100'000,
        ND_LD       = 0b001'000'000,
        ND_BOTTOM   = 0b010'000'000,
        ND_RD       = 0b100'000'000,

        ND_CROSS    = 0b010'101'010,
        ND_X        = 0b101'000'101,
        ND_O        = 0b111'101'111,

        ND_ALL      = 0b111'111'111,
    };
    pos<int>            GetNeighbourPosition (pos<int> _curPos, int _NDFlag);
    vector<pos<int>>    GetNeighbourPositions(pos<int> _curPos, int _NDFlag);
public:
    CEditorScene();
    ~CEditorScene();

    // CScene을(를) 통해 상속됨
    void Initialize() override;
    void Update(double _deltaTime) override;
    void LateUpdate(double _deltaTime) override;
    void Render() override;
    FSceneID CheckTransition() override;
public:
    virtual void		OnEnter() override;
    virtual void		OnExit() override;
private:
    void SetKeyContext();
    void SetDirectionKeyContext();
    UniqueKeyContext	m_pKeyContext;

private:
    pos<float>      MapOffset;
    pos<int>        m_mapSize;
    pos<int>        m_curDirection;
    pos<int>        m_BoundaryResizerPos;
    bool            m_bPressBoundaryResizerPos;
    constexpr static float  SCROLLSPEED             = 300.0f;
    constexpr static float  MARGIN                  = 500.0f;
    constexpr static float  BoundaryResizerSize     = 100.0f;

    void DrawMapBoundary(LPD3DXLINE _pLine);
    void DrawGrid(LPD3DXLINE _pLine);
    void DrawHighlightTile(LPD3DXLINE _pLine, bool _bTileset);
    void DrawTrigger(LPD3DXLINE _pLine, LPD3DXFONT _pFont);

    void DrawBoundaryResizer(LPD3DXLINE _pLine);
    void UpdateBoundaryResizer();

    void DrawUI(LPD3DXFONT _pFont);

private:
    void SaveStage();
    void LoadStage();

    pos<int> GetMousePos();
    pos<int> GetTileIdx(bool _bTileset);

    void UpdateNeighbourTile(pos<int> _tileMapPos, OBJID _objID, int m_iType);

    void AddEntity();
    void RemoveEntity();

    //TileBlock
    void UpdateNeighbourTileBlock(pos<int> _tileMapPos);
    void AddTileBlock(pos<int> _tileMapPos);

    //Object
    void AddObject(pos<int> _tileMapPos);

    //Trigger
    void AddTrigger(pos<int> _tileMapPos);

    OBJID       m_iCategory;
    int         m_iType;
    int         m_iSubType;

    wstring     m_sCategory;
    wstring     m_sType;
    int         m_iCurStage;
    vector<pair<wstring, int>> vecTypeInfo[HAIR];

    bool        m_bTile     = false;
    bool        m_bTileSet  = false;

    pos<int>    m_RespawnPos;

    map<pos<int>, CObject*> m_mapObject[OBJ_END];
private:
    pos<int> GetTileSetPos(pos<int> _tileMapPos, OBJID _objID, int m_iType);
};

