#pragma once

#include "Engine_Defines.h"
#include "Input_Manager_API.h"

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CInput_Manager : public CBase
{
private:
    CInput_Manager()    = default;
    ~CInput_Manager()   = default;

public:
    static CInput_Manager* Create(HINSTANCE hInstance, HWND hWnd);
    HRESULT Initialize(HINSTANCE hInstance, HWND hWnd);
    void    Update();
    void    Free() override;

public:
    EKEYACTIONSTATE GetKeyState         (_ubyte     byKeyID     ) const   { return m_arrKeyActionState[byKeyID]; }
    EKEYACTIONSTATE GetMouseBtnState    (EMOUSEBTN  eMouseBtn   ) const   { return m_arrMouseBtnActionState[ENUM_TO_UINT(eMouseBtn)]; }
    _long           GetMouseMoveState   (EMOUSEMOVE eMouseMove  ) const   { 
        switch (eMouseMove)
        {
        case Engine::EMOUSEMOVE::X:
            return m_tMouseState.lX;
            break;
        case Engine::EMOUSEMOVE::Y:
            return m_tMouseState.lY;
            break;
        case Engine::EMOUSEMOVE::WHEEL:
            return m_tMouseState.lZ;
            break;
        default:
            return 0;
            break;
        }
    }

    bool IsKeyState      (_ubyte byKeyID,       EKEYACTIONSTATE _KeyState) const  { return GetKeyState      (byKeyID)   == _KeyState; }
    bool IsMouseBtnState (EMOUSEBTN eMouseBtn,  EKEYACTIONSTATE _KeyState) const  { return GetMouseBtnState (eMouseBtn) == _KeyState; }

    UniqueKeyContext CreateKeyContext(bool _bInitialActive);
private:
    CKeyContext* CreateContext();

    void DestroyKeyContext(CKeyContext* _pKeyContext);

private:
    LPDIRECTINPUT8			m_pInputSDK = { nullptr };

    LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
    LPDIRECTINPUTDEVICE8	m_pMouse    = { nullptr };

private:
    _byte					m_byKeyState[256]               = {};
    DIMOUSESTATE			m_tMouseState                   = {};
    _bool					m_bIsKeyPressed[256]            = { false, };
    _bool					m_bIsMouseBtnPressed[256]       = { false, };
    EKEYACTIONSTATE         m_arrKeyActionState[256]        = {EKEYACTIONSTATE::NONE, };
    EKEYACTIONSTATE         m_arrMouseBtnActionState[256]   = { EKEYACTIONSTATE::NONE, };

    list<CKeyContext*>	m_listContexts;
};

NS_END