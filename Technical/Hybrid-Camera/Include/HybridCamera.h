#pragma once
#include "Client_Defines.h"
#include "Time_Manager_API.h"
#include "BaseCamera.h"

NS_BEGIN(Client)

class CHybridCamera final : public CBaseCamera
{
	inline static _float fOrthoFOVY = D3DX_PI / 90;		// 2  Degree
	inline static _float fPerspectiveFOVY = D3DX_PI / 6;		// 30 Degree

	inline static _float fOrthoCameraAngle = D3DX_PI / 2;		// 90 Degree
	inline static _float fPerspectiveAngle = D3DX_PI / 3;		// 60 Degree   3

	inline static _float fOrthoMarginY = .01f * g_iCubic;	// 0 CUbic
	inline static _float fPerspectiveMarginY = 2.5f * g_iCubic;	// 2.5 CUbic
private:
	CHybridCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHybridCamera(const CHybridCamera& Prototype);
	virtual ~CHybridCamera() = default;
public:
	virtual HRESULT Initialize_Prototype() override; /* 원형을 위한 초기화(서버, 파일) */
	virtual HRESULT Initialize(void* pArg) override; /* 사본객체를 위한 초기화 */
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

public:
	virtual CGameObject* Clone(void* pArg) override;
	static	CGameObject* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;

public:
	void ShakeRound(_float _fIntensity, _float _fDuration, _uint iCnt);
	void ShakeRandom(_float _fIntensity, _float _fDuration, _float _fFrequency);
	void ShakeUpDown(_float _fIntensity, _float _fDuration, _float _fFrequency, _float _fDampRatio = 0.2f);
private:
	void CreateShakeTimer();
	//Round
	UniqueTimer m_pRoundShakeTimer = { nullptr };
	//UpDown
	UniqueTimer m_pUpDownShakeTimer = { nullptr };
	//Random
	UniqueTimer m_pRandomShakeTimer = { nullptr };
	_uint	m_iRemainCnt = { 0 };
	_float	m_fDampRatio = { 0 };

	_float	m_fIntensity = { 0 };
	_float	m_fDuration = { 0 };
	_float	m_fFrequency = { 0 };

	_float3 m_vOffset = { 0.f, 0.f, 0.f };
private:
	HRESULT	Ready_Components();
private:
	// CBaseCamera을(를) 통해 상속됨
	void SetTransform();
	void SetFreeTransform();

	void SetViewMatrix() override;
	void SetProjMatrix() override;
private:

	_float	m_fFOVY = {};
	_float	m_fRatio = { 1.f };
private:
	class CTransform* m_pFreeCameraTransform = { nullptr };

	_bool m_bLock = false;
	_bool m_bFree = false;
};

NS_END