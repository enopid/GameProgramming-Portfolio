#include "Time_Manager_Impl.h"


CTime_Manager::CTime_Manager() { 
    m_dTimeScale = 1.0; 
};
CTime_Manager* CTime_Manager::Create() {
    CTime_Manager* pInstance = new CTime_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CTime_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

HRESULT CTime_Manager::Initialize() {
    m_lastTime = std::chrono::steady_clock::now();
    return S_OK;
}

HRESULT CTime_Manager::Update(double _deltaTime) {
    for (const auto& _timer : m_listTimers)
        if (_timer->IsActive()) _timer->Update(_deltaTime);
    return S_OK;
}

void CTime_Manager::Free() {
    __super::Free();
}

void CTime_Manager::Freeze(double _deltaTime) {
    m_FreezeTime = _deltaTime;
    m_FreezeStartTime = std::chrono::steady_clock::now();
}

_float CTime_Manager::GetDeltaTime() {
    auto curTime = std::chrono::steady_clock::now();
    auto deltaTime = curTime - m_lastTime;
    _float dDeltaTime = chrono::duration_cast<chrono::nanoseconds>(deltaTime).count() / 1'000'000'000.0f;

    m_lastTime = curTime;

    while (DMINDELTASECOND > dDeltaTime) {
        auto curTime = std::chrono::steady_clock::now();
        deltaTime += curTime - m_lastTime;
        dDeltaTime = chrono::duration_cast<chrono::nanoseconds>(deltaTime).count() / 1'000'000'000.0f;
        m_lastTime = curTime;
    }

    if (m_FreezeTime > 0) {
        auto _deltaTime = curTime - m_FreezeStartTime;
        _float _dDeltaTime = chrono::duration_cast<chrono::nanoseconds>(_deltaTime).count() / 1'000'000'000.0f;
        if (m_FreezeTime < _dDeltaTime) m_FreezeTime = -1;
        else                            return 0;
    }

    return (_float)min(dDeltaTime * m_dTimeScale, DMAXDELTASECOND);
}

void CTime_Manager::SetTimeScale(const double& _InTimeScale) {
    if (_InTimeScale <= 0) return;
    m_dTimeScale = _InTimeScale;
}

CTimer* CTime_Manager::CreateTimer(bool _bIsLoop) {
    CTimer* pNewTimer = new CTimer(_bIsLoop);
    m_listTimers.push_back(pNewTimer);
    pNewTimer->Initialize();
    return pNewTimer;
}

void CTime_Manager::DestroyTimer(CTimer* _pTimer) {
    m_listTimers.remove(_pTimer);
    if (_pTimer) delete _pTimer;
}

// Timer implementation

UniqueTimer CTime_Manager::CreateTimerWithFrequency(double _frequency, bool _bIsLoop = false, function<void(void)> _func = nullptr, bool _bInitialActive = true) {
    CTimer* _pTimer = CreateTimer(_bIsLoop);
    _pTimer->SetFrequency(_frequency);
    if (_func) _pTimer->SetFunc(_func);
    _pTimer->SetActive(_bInitialActive);

    return UniqueTimer(_pTimer, [=](CTimer* _pTImer) { DestroyTimer(_pTImer); });
}

UniqueTimer CTime_Manager::CreateTimerWithDuration(double _duration, bool _bIsLoop = false, function<void(void)> _func = nullptr, bool _bInitialActive = true) {
    CTimer* _pTimer = CreateTimer(_bIsLoop);
    _pTimer->SetDuration(_duration);
    if (_func) _pTimer->SetFunc(_func);
    _pTimer->SetActive(_bInitialActive);

    return UniqueTimer(_pTimer, [=](CTimer* _pTImer) {DestroyTimer(_pTImer); });
}