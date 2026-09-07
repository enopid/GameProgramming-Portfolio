#pragma once
#include "Object.h"
#include "StaticSprite.h"
#include "AnimationSprite.h"
#include "DynamicSprite.h"
#include "Debris.h"

class CBumpBlockTile :
    public CObject
{
public:
    CBumpBlockTile(int _type) : CObject(), m_type(_type) {}
    ~CBumpBlockTile() {};
    void	Initialize()    override;
    void	Update(double _dDeltaTime)    override;
private:
    int     m_type;
    bool    m_bGimmicState;
};

class CBumpBlock :
    public CObject
{
    constexpr static double FIREANIMATIONTIME = 0.6f;
    constexpr static double ICEANIMATIONTIME  = 0.6f;

    constexpr static double BLOCKACTIVETIME   = 0.1f;
    constexpr static double BLOCKRESPAWNTIME  = 2.0f;

    constexpr static double ICESINKTIME  = 1.4f;

    constexpr static double SINKTIME    = 0.4f;
    constexpr static double SINKDIST    = 16 * PIXEL;
    constexpr static double BOUNCECOEFF = -2.0;
public:
    CBumpBlock(RECT _rect, int _type = 15) : CObject(), m_rect(_rect), m_iType(_type) {
        m_pSprite = nullptr;
        m_size          = { _rect.right - _rect.left + 1, _rect.bottom - _rect.top + 1 };
        m_centerOffset = { m_size.first * (TILESIZE >> 1), m_size.second * (TILESIZE >> 1) };
    }
    ~CBumpBlock() {};
    void	Initialize()    override;
    void	Update(double _dDeltaTime)    override;
    void Collapse();
    void Regenerate();
    void	LateUpdate(double _dDeltaTime)    override;

    void    CreateSprite();
    void    CreateTiles();
    void    OnCollision(FCollisionInfo _collisionInfo);
    void    SetCollision();

private:
    UniqueTimer m_pBumpFireTimer;
    void    CreateBumpFireTimer();
    double  GetBumpOffset(double _curtime) 
    {
        double _b = BOUNCECOEFF;
        double _a = _b/tan(-_b);
        double _A = SINKDIST/(exp(_a)*sin(_b));

        return _A * (exp(_a * _curtime/SINKTIME) * sin(_b * _curtime / SINKTIME));
    }
    UniqueTimer m_pBumpIceTimer;
    void    CreateBumpIceTimer();

    UniqueTimer m_pRespawnTimer;
    void    CreateBumpRespawnTimer();
    UniqueTimer m_pActiveTimer;
    void    CreateActiveTimer();
    bool    m_bActive;
private:
    CSprite*    m_pSprite;
    bool        m_bGimmicState;
    RECT        m_rect;
    pos<int>    m_initPos;
    pos<int>    m_centerOffset;
    pos<int>    m_size;
    int         m_iType;

private:
    pos<int>    m_iDirection;

    bool        m_bOnLift;
    UniqueTimer m_LiftTImer;
};

