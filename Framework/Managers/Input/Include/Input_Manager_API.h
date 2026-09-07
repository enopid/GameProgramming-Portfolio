#pragma once

#include "Engine_Defines.h"

NS_BEGIN(Engine)

class CKeyContext {
    CKeyContext(const CKeyContext&) = delete;
    CKeyContext& operator=(const CKeyContext&) = delete;

public:
    bool IsActive() const           { return m_bIsActive; }
    void SetActive(bool isActive)   { m_bIsActive = isActive; }

    void AddKeyFunc         (_ubyte byKeyID,        function<void(void)> func, EKEYACTIONSTATE iType)   { m_vecKeyFunctions[byKeyID][ENUM_TO_UINT(iType)].push_back(func); }
    void AddMouseBtnFunc    (EMOUSEBTN eMouseBtn,   function<void(void)> func, EKEYACTIONSTATE iType)   { m_vecMouseBtnFunctions[ENUM_TO_UINT(eMouseBtn)][ENUM_TO_UINT(iType)].push_back(func); }
    void AddMouseMoveFunc   (function<void(int, int, int)> func)                                        { m_vecMouseMoveFunctions.push_back(func); }
private:
    void ExecuteKeyFunc(_ubyte byKeyID, EKEYACTIONSTATE iType) {
        if (!IsActive()) return;
        for (const auto& _func : m_vecKeyFunctions[byKeyID][ENUM_TO_UINT(iType)]) _func();
    }
    void ExecuteMouseBtnFunc(EMOUSEBTN eMouseBtn, EKEYACTIONSTATE iType) {
        if (!IsActive()) return;
        for (const auto& _func : m_vecMouseBtnFunctions[ENUM_TO_UINT(eMouseBtn)][ENUM_TO_UINT(iType)]) _func();
    }
    void ExecuteMouseMoveFunc(int dx, int dy, int dz) {
        if (!IsActive()) return;
        for (const auto& _func : m_vecMouseMoveFunctions) _func(dx, dy, dz);
    }

    friend class CInput_Manager;

    CKeyContext()   { m_bIsActive = true; };
    ~CKeyContext()  {};
    
    bool					            m_bIsActive;
    list<function<void(void)>>          m_vecKeyFunctions[256][ENUM_TO_UINT(EKEYACTIONSTATE::END)];
    list<function<void(void)>>          m_vecMouseBtnFunctions[ENUM_TO_UINT(EMOUSEBTN::END)][ENUM_TO_UINT(EKEYACTIONSTATE::END)];
    list<function<void(int,int,int)>>   m_vecMouseMoveFunctions;
};

using   UniqueKeyContext = unique_ptr<CKeyContext, std::function<void(CKeyContext*)>>;

NS_END