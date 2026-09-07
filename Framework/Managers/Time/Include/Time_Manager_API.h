#pragma once

#include "Engine_Defines.h"

#include <chrono>

NS_BEGIN(Engine)

class CTimer
{
    friend class CTime_Manager;

    CTimer(const CTimer&)            = delete;
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
    void Reset() { m_bIsActive = false; Clear(); }

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

NS_END