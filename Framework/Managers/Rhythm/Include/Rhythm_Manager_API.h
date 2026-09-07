#pragma once

#include "Listener.h"

NS_BEGIN(Engine)

class CBeatListener
{
    friend class CRhythm_Manager;

    CBeatListener(const CBeatListener&) = delete;
    CBeatListener& operator=(const CBeatListener&) = delete;
private:
    bool Update(float _fDeltaTime) { return m_pListener->Update(_fDeltaTime); }
    void Activate(float _fDuration) { m_pListener->Activate(_fDuration); };
    void Deactivate() { m_pListener->Deactivate(); };
    void Reset() { m_pListener->Reset(); }
private:
    ~CBeatListener() {
        Safe_Release(m_pListener);
    };
    CBeatListener(CListener* pListener){
        m_pListener = pListener;
    };
private:
    CListener*  m_pListener;
};
using   UniqueBeatListener = unique_ptr<CBeatListener, std::function<void(CBeatListener*)>>;

NS_END