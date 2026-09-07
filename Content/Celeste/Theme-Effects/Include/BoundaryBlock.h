#pragma once
#include "Object.h"
class CBoundaryBlock : public CObject
{
public:
    //_type1 : 불만 활성 / _type2 : 불만 활성 / _type3 : 둘다 활성
    CBoundaryBlock(RECT _rect, int _type = 3) : CObject(), m_rect(_rect), m_iType(_type) {
        m_pSprite = nullptr;
        m_size = { _rect.right - _rect.left + 1, _rect.bottom - _rect.top + 1 };
        m_centerOffset = { m_size.first * (TILESIZE >> 1), m_size.second * (TILESIZE >> 1) };
    }
    ~CBoundaryBlock() {};
    void	Initialize()    override;
    void	Update(double _dDeltaTime)    override;
    void	LateUpdate(double _dDeltaTime)    override;

    void    CreateSprite();
    void    OnCollision(FCollisionInfo _collisionInfo);
    void    SetCollision();

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
};

