#pragma once
#include "Tile.h"
class CCrystalSpike : public CTile
{
public:
    CCrystalSpike(pos<int> _tileIdx, int _Type) : CTile(_tileIdx, TILESIZE), m_iType(_Type) {

    }
    CCrystalSpike(FTileBlockInfo _tileInfo) : CCrystalSpike(_tileInfo.TileMapPos, _tileInfo.State) {

    }
    ~CCrystalSpike() {};

    // CTile을(를) 통해 상속됨
    void Initialize() override;
    void Update(double _dDeltatime) override;
    void CreateTileSprite() override;
    void SetCollision() override;
    int  GetState() { return m_iType; }

    void OnCollision(FCollisionInfo _collisionInfo);
private:
    int			m_iType;
    bool        m_bGimmicState;
};

