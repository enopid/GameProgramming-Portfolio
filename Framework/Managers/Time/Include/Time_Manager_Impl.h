#pragma once

#include "Engine_Defines.h"
#include "Time_Manager_API.h"

#include "Base.h"

#include <chrono>

NS_BEGIN(Engine)

class ENGINE_DLL CTime_Manager : public CBase
{
    constexpr static double DMAXDELTASECOND = 1.0 / 20;
    constexpr static double DMINDELTASECOND = 1.0 / 60;

private:
    CTime_Manager();
    virtual ~CTime_Manager() = default;
public:
    static CTime_Manager* Create();
    HRESULT Initialize();
    HRESULT Update(double _deltaTime);
    void    Free() override;

public:
    UniqueTimer CreateTimerWithFrequency(double _frequency, bool _bIsLoop, function<void(void)> _func, bool _bInitialActive);
    UniqueTimer CreateTimerWithDuration (double _frequency, bool _bIsLoop, function<void(void)> _func, bool _bInitialActive);
public:
    void    Freeze(double _deltaTime);

    _float  GetDeltaTime();

    void    SetTimeScale(const double& _InTimeScale);

private:
    CTimer* CreateTimer(bool _bIsLoop = false);
    void    DestroyTimer(CTimer* _pTimer);

    chrono::steady_clock::time_point m_lastTime;
    chrono::steady_clock::time_point m_FreezeStartTime;
    double                           m_FreezeTime = {};
    double m_dTimeScale;

    list<CTimer*> m_listTimers;
};

NS_END