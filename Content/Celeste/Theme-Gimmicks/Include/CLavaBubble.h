#pragma once
#include "Particle.h"
class CLavaBubble :
    public CParticle
{
    inline      static int      PARTICLECNT  = 0;
    constexpr   static double   LIFETIME    = 2.0f;
public:
    CLavaBubble(int m_iType);
    ~CLavaBubble();
    // CParticle을(를) 통해 상속됨
    void Release()      override;
    void Initialize() override;
    void Update(double _delata) override;
    void CreateSprite() override;
    void CreateLifeCycleTimer() override;

    static int GetParticleCount() { return PARTICLECNT; };
private:

};

