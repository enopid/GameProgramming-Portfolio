#pragma once
#include "Tile.h"
class CFlipSwitch : public CTile
{
    constexpr static double TRANSITANIMATIONTIME    = 1.0f;
    constexpr static double DISACTIVEANIMATIONTIME  = 0.7f;
    constexpr static double ACTIVEANIMATIONTIME     = 0.7f;
public:
    CFlipSwitch(pos<int> _tileIdx, int _Type) : CTile(_tileIdx, TILESIZE), m_iState(_Type){

    }
    CFlipSwitch(FObjectInfo _tileInfo) : CFlipSwitch(_tileInfo.MapPos, _tileInfo.State) {

    }
    ~CFlipSwitch() {};

    // CTile을(를) 통해 상속됨
    void Update(double _dDeltaTime) override;
    void Initialize()       override;
    void CreateTileSprite() override;
    void SetCollision() override;
    int  GetState() { return m_iState; }

    void OnCollision(FCollisionInfo _collisionInfo);
private:
    int		    m_iState;
    bool        m_bGimmicState;
    bool        m_bTransit;
};

