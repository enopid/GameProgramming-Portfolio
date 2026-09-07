#pragma once
#include "Tile.h"
class CTileSetTile :
    public CTile
{
public:
    CTileSetTile(pos<int> _tileIdx, pos<int> _tileSetIdx, int _Type) : CTile(_tileIdx, TILESIZE), m_TileSetIdx(_tileSetIdx), m_iTileType(_Type){
        
    }
    CTileSetTile(FTileInfo _tileInfo) : CTileSetTile(_tileInfo.TileMapPos, _tileInfo.TileSetPos, _tileInfo.Type) {

    }
    ~CTileSetTile() {};
    
    void        SetTileSetIdx(pos<int>	_tileSetIdx);
    
    pos<int>    GetTileSetIdx() const;
    int         GetTileType()   const;

    // CTile을(를) 통해 상속됨
    void Initialize() override;
    void Update(double _dDeltaTime) override;
    void LateUpdate(double _dDeltaTime) override;
    void Release() override;

    // CTile을(를) 통해 상속됨
    void CreateTileSprite() override;
    void SetCollision() override;
    
private:
    pos<int>	m_TileSetIdx;
    int			m_iTileType;
};

inline bool IsSameTileSet(pos<int>	_TileSetIdx1, pos<int>	_TileSetIdx2) {
    if (_TileSetIdx1.first<=3 && _TileSetIdx2.first <= 3) {
        if (_TileSetIdx1.second     != _TileSetIdx2.second)     return false;
        return true;
    }
    if (_TileSetIdx1.first == 4 && _TileSetIdx2.first == 4) {
        if (_TileSetIdx1.second != _TileSetIdx2.second)     return false;
        return true;
    }
    if (_TileSetIdx1.first == 5 && _TileSetIdx2.first == 5) {
        if (_TileSetIdx1.second <  12 && _TileSetIdx2.second <  12)     return true;
        if (_TileSetIdx1.second >= 12 && _TileSetIdx2.second >= 12)     return true;
        return false;
    }
    return false;
}

