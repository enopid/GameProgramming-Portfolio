#pragma once

#include "pch.h"

class CTimeManager;

class CTimer
{
    friend CTimeManager;

    CTimer(const CTimer&) = delete;
    CTimer& operator=(const CTimer&) = delete;
public:
    void SetActive(bool _isActive) { m_bIsActive = _isActive; }
    //duration in second
    void SetDuration(double _dDuration) { m_dDuration = _dDuration; }
    //frequency per second
    void SetFrequency(double _dFrequency) { m_dDuration = 1.0 / _dFrequency; }
    void SetFunc(function<void(void)> _func) { m_function = _func; }
    double GetElapsedTime()  const { return max(min(m_dElapsedTime, m_dDuration - DBL_EPSILON), 0); }
    double GetElapsedRatio() const { return max(min(m_dElapsedTime / m_dDuration, 1.0 - DBL_EPSILON), 0); }
    void Initialize() {
        m_dElapsedTime = 0.0;
        m_bIsActive = true;
    };
    void Clear() { m_dElapsedTime = 0; }
    bool IsActive() const { return m_bIsActive; }
    void Restart() { m_bIsActive = true; Clear(); }
    void Reset()   { m_bIsActive = false; Clear(); }

private:
    ~CTimer() {};
    CTimer(bool _bIsLoop) : m_bIsLoop(_bIsLoop) {
        m_dElapsedTime = 0.0;
        m_function = nullptr;
        m_bIsActive = false;
    };

    void Update(double _deltaTime) {
        m_dElapsedTime += _deltaTime;
        if (m_dElapsedTime >= m_dDuration) {
            if (!m_bIsLoop) {
                m_bIsActive = false;
                m_dElapsedTime = 0;
            }
            else {
                m_dElapsedTime -= m_dDuration;
            }
            if (m_function) m_function();
        }
    };

    void Release() {};

private:
    const bool	m_bIsLoop;
    bool		m_bIsActive;
    double		m_dDuration;

    double		m_dElapsedTime;

    function<void(void)> m_function;
};

using   UniqueTimer = unique_ptr<CTimer, std::function<void(CTimer*)>>;
UniqueTimer CreateTimerWithFrequency(double _frequency, bool _bIsLoop, function<void(void)> _func, bool _bInitialActive);
UniqueTimer CreateTimerWithDuration(double _frequency, bool _bIsLoop, function<void(void)> _func, bool _bInitialActive);

class CTimeManager
{
    constexpr static double DMAXDELTASECOND = 0.064 * 1;
    constexpr static double DMINDELTASECOND = 0.016 * 1;

    CTimeManager(const CTimeManager&) = delete;
    CTimeManager& operator=(const CTimeManager&) = delete;

    friend UniqueTimer CreateTimerWithFrequency(double _frequency, bool _bIsLoop, function<void(void)> _func, bool _bInitialActive);
    friend UniqueTimer CreateTimerWithDuration(double _frequency, bool _bIsLoop, function<void(void)> _func, bool _bInitialActive);
public:
    void Initialize() {
        m_lastTime = std::chrono::steady_clock::now();
    };

    void Freeze(double _deltaTime) {
        m_FreezeTime        = _deltaTime;
        m_FreezeStartTime   = std::chrono::steady_clock::now();
    }

    bool IsFreeze() {
        return m_FreezeTime > 0;
	}

    void Update(double _deltaTime) {
        for (const auto& _timer : m_listTimers)
            if (_timer->IsActive()) _timer->Update(_deltaTime);
    };

    static CTimeManager& Instance() {
        static CTimeManager instance;
        return instance;
    }


    double GetDeletaTime() {
        auto curTime = std::chrono::steady_clock::now();
        auto deltaTime = curTime - m_lastTime;
        double dDeltaTime = chrono::duration_cast<chrono::nanoseconds>(deltaTime).count() / 1'000'000'000.0;

        m_lastTime = curTime;

        while (DMINDELTASECOND > dDeltaTime) {
            auto curTime = std::chrono::steady_clock::now();
            deltaTime += curTime - m_lastTime;
            dDeltaTime = chrono::duration_cast<chrono::nanoseconds>(deltaTime).count() / 1'000'000'000.0;
            m_lastTime = curTime;
        }

        if (m_FreezeTime > 0) {
            auto _deltaTime = curTime - m_FreezeStartTime;
            double _dDeltaTime = chrono::duration_cast<chrono::nanoseconds>(_deltaTime).count() / 1'000'000'000.0;
            if (m_FreezeTime < _dDeltaTime) m_FreezeTime = -1;
            else                            return 0;
        }

        return min(dDeltaTime * m_dTimeScale, DMAXDELTASECOND);
    }

    void SetTimeScale(const double& _InTimeScale) {
        if (_InTimeScale <= 0) return;
        m_dTimeScale = _InTimeScale;
    }

private:
    CTimer* CreateTimer(bool _bIsLoop = false) {
        CTimer* pNewTimer = new CTimer(_bIsLoop);
        m_listTimers.push_back(pNewTimer);
        pNewTimer->Initialize();
        return pNewTimer;
    }

    void DestroyTimer(CTimer* _pTimer) {
        m_listTimers.remove(_pTimer);
        SAFE_DELETE(_pTimer);
    }

    void Release() {
    };
    CTimeManager() {
        m_dTimeScale = 1.0;
    };
    ~CTimeManager() { Release(); };

    chrono::steady_clock::time_point m_lastTime;
    chrono::steady_clock::time_point m_FreezeStartTime;
    double                           m_FreezeTime;
    double m_dTimeScale;

    list<CTimer*> m_listTimers;
};


inline  UniqueTimer CreateTimerWithFrequency(double _frequency, bool _bIsLoop = false, function<void(void)> _func = nullptr, bool _bInitialActive = true) {
    CTimer* _pTimer = CTimeManager::Instance().CreateTimer(_bIsLoop);
    _pTimer->SetFrequency(_frequency);
    if (_func) _pTimer->SetFunc(_func);
    _pTimer->SetActive(_bInitialActive);

    return UniqueTimer(_pTimer, [](CTimer* _pTImer) { CTimeManager::Instance().DestroyTimer(_pTImer); });
}

inline  UniqueTimer CreateTimerWithDuration(double _duration, bool _bIsLoop = false, function<void(void)> _func = nullptr, bool _bInitialActive = true) {
    CTimer* _pTimer = CTimeManager::Instance().CreateTimer(_bIsLoop);
    _pTimer->SetDuration(_duration);
    if (_func) _pTimer->SetFunc(_func);
    _pTimer->SetActive(_bInitialActive);

    return UniqueTimer(_pTimer, [](CTimer* _pTImer) {CTimeManager::Instance().DestroyTimer(_pTImer); });
}