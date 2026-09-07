#pragma once
#include "Client_Defines.h"
#include "GameInstance.h"
#include "PSystem.h"

NS_BEGIN(Client)
class CPSystem_Ozraeen : public CPSystem
{
private:
    LPDIRECT3DDEVICE9 m_pGraphic_Device = nullptr;
    CGameInstance* m_pGameInstance = nullptr;

public:
    CPSystem_Ozraeen(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual ~CPSystem_Ozraeen() = default;
public:
    HRESULT Initialize();
    HRESULT CreatePool(_uint MaxNum, const _wstring& strPrototypeTag) override;
    void Trigger(_float3 vPos) override;
    void Update(_float fTimeDelta) override;
    void Render() override;
    void Free() override;
    void DeactivateAll() override;

    void SetTransform(CTransform* _pInitTransform) {
        m_pInitTransform = _pInitTransform;
    }
    void HitBlinkReset() {
        m_iBlinkCnt = 14;
        m_pHitTimer->Restart();
    };
    void SetActive(_bool _bIsActive) { m_pSpawnTimer->SetActive(_bIsActive); };
private:
    class CTransform* m_pInitTransform = { nullptr };
private:
    UniqueTimer	m_pHitTimer = nullptr;
    _uint		m_iBlinkCnt = 0;
    _bool		m_bBlink = false;
};
NS_END