#include "Input_Manager_Impl.h"

CInput_Manager* CInput_Manager::Create(HINSTANCE hInstance, HWND hWnd)
{
    CInput_Manager* pInstance = new CInput_Manager();

    if (FAILED(pInstance->Initialize(hInstance, hWnd)))
    {
        MSG_BOX("Failed to Created : CInput_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

HRESULT CInput_Manager::Initialize(HINSTANCE hInstance, HWND hWnd)
{

    // DInput 컴객체를 생성하는 함수
    if (FAILED(DirectInput8Create(hInstance,
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&m_pInputSDK,
        NULL)))
        return E_FAIL;;

    // 키보드 객체 생성
    if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))) return E_FAIL;
    m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
    m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    m_pKeyBoard->Acquire();

    if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))       return E_FAIL;
    m_pMouse->SetDataFormat(&c_dfDIMouse);
    m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    m_pMouse->Acquire();

    return S_OK;
}
void CInput_Manager::Update()
{
    m_pKeyBoard->GetDeviceState(256, m_byKeyState);
    m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);

    for (int i = 0; i < 256; i++) {
        if (m_byKeyState[i] & 0x80) {
            m_arrKeyActionState[i] = (m_bIsKeyPressed[i]) ? EKEYACTIONSTATE::STAY : EKEYACTIONSTATE::ENTER;
            m_bIsKeyPressed[i] = true;
        }
        else {
            m_arrKeyActionState[i] = (m_bIsKeyPressed[i]) ? EKEYACTIONSTATE::EXIT : EKEYACTIONSTATE::NONE;
            m_bIsKeyPressed[i] = false;
        }
    }
    for (int i = 0; i < ENUM_TO_UINT(EMOUSEBTN::END); i++) {
        
        if (m_tMouseState.rgbButtons[ENUM_TO_UINT(i)] & 0x80) {
            m_arrMouseBtnActionState[i] = (m_bIsMouseBtnPressed[i]) ? EKEYACTIONSTATE::STAY : EKEYACTIONSTATE::ENTER;
            m_bIsMouseBtnPressed[i] = true;
        }
        else {
            m_arrMouseBtnActionState[i] = (m_bIsMouseBtnPressed[i]) ? EKEYACTIONSTATE::EXIT : EKEYACTIONSTATE::NONE;
            m_bIsMouseBtnPressed[i] = false;
        }
    }

    for (auto& pKeyContext : m_listContexts) {
        if (!pKeyContext || !pKeyContext->IsActive()) continue;

        for (int i = 0; i < 256; i++) {
            pKeyContext->ExecuteKeyFunc(i, GetKeyState(i));
        }
        for (int i = 0; i < ENUM_TO_UINT(EMOUSEBTN::END); i++) {
            pKeyContext->ExecuteMouseBtnFunc(static_cast<EMOUSEBTN>(i), GetMouseBtnState(static_cast<EMOUSEBTN>(i)));
        }
        pKeyContext->ExecuteMouseMoveFunc(m_tMouseState.lX, m_tMouseState.lY, m_tMouseState.lZ);
    }
};


void CInput_Manager::Free() {
    Safe_Release(m_pKeyBoard);
    Safe_Release(m_pMouse);
    Safe_Release(m_pInputSDK);

    for (auto& pKeyContext : m_listContexts) if (pKeyContext) delete pKeyContext;
    __super::Free();
}

UniqueKeyContext CInput_Manager::CreateKeyContext(bool _bInitialActive)
{
    CKeyContext* _pKeyContext = CreateContext();
    _pKeyContext->SetActive(_bInitialActive);
    return UniqueKeyContext(_pKeyContext, [=](CKeyContext* _pKeyContext) {DestroyKeyContext(_pKeyContext); });
}

CKeyContext* CInput_Manager::CreateContext() {
    CKeyContext* pNewKeyContext = new CKeyContext();
    m_listContexts.push_back(pNewKeyContext);
    return pNewKeyContext;
}

void CInput_Manager::DestroyKeyContext(CKeyContext* _pKeyContext) {
    m_listContexts.remove(_pKeyContext);
    if (_pKeyContext) delete _pKeyContext;
}