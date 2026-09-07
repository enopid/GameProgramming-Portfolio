#include "HybridCamera.h"
#include "Transform.h"
#include "GameInstance.h"

void CHybridCamera::SetViewMatrix()
{
	auto _vCamPos = m_pTransformCom->Get_State(STATE::POSITION);
	auto _vAt = m_pTransformCom->Get_State(STATE::LOOK) + _vCamPos;
	auto _vUpDir = m_pTransformCom->Get_State(STATE::UP);

	D3DXMatrixLookAtLH(&m_pViewMatrix, &_vCamPos, &_vAt, &_vUpDir);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_pViewMatrix);
}

void CHybridCamera::SetProjMatrix()
{
	D3DXMatrixPerspectiveFovLH(&m_pProjMatrix, m_fFOVY, g_fRatio, m_fNearPlane, m_fFarPlane);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_pProjMatrix);
}

CHybridCamera::CHybridCamera(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBaseCamera(pGraphic_Device)
{
}

CHybridCamera::CHybridCamera(const CHybridCamera& Prototype)
	: CBaseCamera(Prototype)
{
}

HRESULT CHybridCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHybridCamera::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	CreateShakeTimer();
	m_pTransformCom->Set_State(STATE::POSITION, _float3(15.f, 8.f, -20.f));
	m_vOffset = { 0.f, 0.f, 0.f };
	return S_OK;
}

void CHybridCamera::Priority_Update(_float fTimeDelta)
{
	auto [_, _fRatio] = m_pGameInstance->Get_DimensionInfo();
	m_fRatio = _fRatio;
	if (m_bFree)	SetFreeTransform();
	else			SetTransform();
	SetCamera();
}

void CHybridCamera::SetTransform()
{
	//ortho					   ======> Perspective
	//fRatio : 0               ======> 1
	//marginY: 0               ======> 2 * g_iCubic
	//theta  : D3DX_PI / 2     ======> D3DX_PI / 3
	//HFOVY  : D3DX_PI / 180   ======> D3DX_PI / 18 

	_float		_yMargin = fOrthoMarginY + (fPerspectiveMarginY - fOrthoMarginY) * m_fRatio;
	_float		_fTheta  = fOrthoCameraAngle + (fPerspectiveAngle - fOrthoCameraAngle) * m_fRatio;
	m_fFOVY = fOrthoFOVY + (fPerspectiveFOVY - fOrthoFOVY) * m_fRatio;
	_float		_fHFOVY = m_fFOVY * 0.5f;

	//스크린상에 담고자 하는  Y 축 상의 범위
	_float		_fTargetY = g_iMapSizeX / g_fRatio * g_iCubic + 2 * _yMargin; 
	//상단 Y에서 카메라까지의 초점 거리
	_float		_fDepth = _fTargetY * sin(_fTheta + _fHFOVY) / sin(_fHFOVY * 2); 

	_float3		_vProjPlaneTop;
	_vProjPlaneTop.x = g_iMapSizeX * g_iCubic * .5f;
	_vProjPlaneTop.y = _fTargetY - _yMargin;
	_vProjPlaneTop.z = -0.5 * g_iCubic;

	_float3		_vCamPos= _vProjPlaneTop;
	_vCamPos += _fDepth * _float3(0.f, -cos(_fTheta - _fHFOVY), -sin(_fTheta - _fHFOVY));
	_float3		_vLook	= _float3{ 0.f, cos(_fTheta), sin(_fTheta) };
	_float3		_vUp	= _float3{ 0.f, 1.f, 0.f };
	_float3		_vRight;

	D3DXVec3Normalize	(&_vLook, &_vLook);
	D3DXVec3Cross		(&_vRight, &_vUp, &_vLook);
	D3DXVec3Normalize	(&_vRight, &_vRight);
	D3DXVec3Cross		(&_vUp, &_vLook, &_vRight);
	D3DXVec3Normalize	(&_vUp, &_vUp);

	_vCamPos += m_vOffset;
	m_pTransformCom->Set_State(STATE::POSITION, _vCamPos);
	m_pTransformCom->Set_State(STATE::LOOK,		_vLook);
	m_pTransformCom->Set_State(STATE::UP,		_vUp);
	m_pTransformCom->Set_State(STATE::RIGHT,	_vRight);
}

void CHybridCamera::SetFreeTransform()
{
	m_fFOVY = D3DX_PI / 3;
	m_pTransformCom->Set_State(STATE::UP, m_pFreeCameraTransform->Get_State(STATE::UP));
	m_pTransformCom->Set_State(STATE::RIGHT, m_pFreeCameraTransform->Get_State(STATE::RIGHT));
	m_pTransformCom->Set_State(STATE::LOOK, m_pFreeCameraTransform->Get_State(STATE::LOOK));
	m_pTransformCom->Set_State(STATE::POSITION, m_pFreeCameraTransform->Get_State(STATE::POSITION));
}

void CHybridCamera::Update(_float fTimeDelta)
{
	if (m_pUpDownShakeTimer->IsActive()) {
		auto _fRatio = m_pUpDownShakeTimer->GetElapsedRatio();
		m_vOffset = { 0.f,0.f,0.f };
		m_vOffset.y = m_fIntensity * exp(-m_fDampRatio * _fRatio / m_fDuration) * sinf(D3DX_PI * 2.f * m_fFrequency * _fRatio / m_fDuration);
		PrintDebug(m_vOffset.y);
	}

	if (m_pGameInstance->IsKeyState('L', EKeyActionState::Enter)) m_bLock ^= true;

	if (m_pGameInstance->IsKeyState('F', EKeyActionState::Enter)) m_bFree ^= true;

	if (m_bLock) {
		fPerspectiveFOVY = D3DX_PI / 2;
		fPerspectiveAngle = D3DX_PI / 2;
		fPerspectiveMarginY = .1f * g_iCubic;	// 2.5 CUbic
	}
	else {
		fPerspectiveFOVY = D3DX_PI / 6;		// 30 Degree
		fPerspectiveAngle = D3DX_PI / 3;
		fPerspectiveMarginY = 2.5f * g_iCubic;	// 2.5 CUbic
	}

	if (m_bFree) {
		if (m_pGameInstance->IsKeyState('D', EKeyActionState::Stay)) m_pFreeCameraTransform->Go_Right(fTimeDelta);
		if (m_pGameInstance->IsKeyState('A', EKeyActionState::Stay)) m_pFreeCameraTransform->Go_Left(fTimeDelta);
		if (m_pGameInstance->IsKeyState('W', EKeyActionState::Stay)) m_pFreeCameraTransform->Go_Straight(fTimeDelta);
		if (m_pGameInstance->IsKeyState('S', EKeyActionState::Stay)) m_pFreeCameraTransform->Go_Backward(fTimeDelta);
		if (m_pGameInstance->IsKeyState('Q', EKeyActionState::Stay)) m_pFreeCameraTransform->Go_Up(fTimeDelta);
		if (m_pGameInstance->IsKeyState('E', EKeyActionState::Stay)) m_pFreeCameraTransform->Go_Down(fTimeDelta);
		if (m_pGameInstance->IsKeyState(VK_NUMPAD8, EKeyActionState::Stay)) m_pFreeCameraTransform->Turn({ +1.f, 0.f, 0.f }, fTimeDelta);
		if (m_pGameInstance->IsKeyState(VK_NUMPAD2, EKeyActionState::Stay)) m_pFreeCameraTransform->Turn({ -1.f, 0.f, 0.f }, fTimeDelta);
		if (m_pGameInstance->IsKeyState(VK_NUMPAD4, EKeyActionState::Stay)) m_pFreeCameraTransform->Turn({ 0.f, +1.f, 0.f }, fTimeDelta);
		if (m_pGameInstance->IsKeyState(VK_NUMPAD6, EKeyActionState::Stay)) m_pFreeCameraTransform->Turn({ 0.f, -1.f, 0.f }, fTimeDelta);
		if (m_pGameInstance->IsKeyState(VK_NUMPAD7, EKeyActionState::Stay)) m_pFreeCameraTransform->Turn({ 0.f, 0.f, +1.f }, fTimeDelta);
		if (m_pGameInstance->IsKeyState(VK_NUMPAD9, EKeyActionState::Stay)) m_pFreeCameraTransform->Turn({ 0.f, 0.f, -1.f }, fTimeDelta);
		if (m_pGameInstance->IsKeyState(VK_NUMPAD5, EKeyActionState::Enter)) {
			//m_pFreeCameraTransform->Set_State

		}
	}
}

void CHybridCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CHybridCamera::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(0.0f);

	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;
	TransformDesc.fSpeedPerSec = 40.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);
	if (FAILED(__super::Add_Component(ENUM_TO_UINT(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform1"), reinterpret_cast<CComponent**>(&m_pFreeCameraTransform), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CGameObject* CHybridCamera::Clone(void* pArg)
{

	CHybridCamera* pInstance = new CHybridCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CHybridCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHybridCamera::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHybridCamera* pInstance = new CHybridCamera(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CHybridCamera");
		Safe_Release(pInstance);
	}

	return pInstance;


	return nullptr;
}

void CHybridCamera::Free()
{
	__super::Free();
	Safe_Release(m_pFreeCameraTransform);
}

void CHybridCamera::ShakeRound(_float _fIntensity, _float _fDuration, _uint iCnt)
{
	m_fDuration = _fDuration / iCnt;
	m_fIntensity = _fIntensity;
	m_fFrequency = iCnt;

	m_pRandomShakeTimer->SetDuration(m_fDuration);
	m_pRandomShakeTimer->Restart();
	m_iRemainCnt = iCnt;

}

void CHybridCamera::ShakeRandom(_float _fIntensity, _float _fDuration, _float _fFrequency)
{
	m_fDuration = _fDuration;
	m_fIntensity = _fIntensity;
	m_fFrequency = _fFrequency;

	m_pRandomShakeTimer->SetFrequency(m_fFrequency);
	m_pRandomShakeTimer->Restart();
	m_iRemainCnt = m_fDuration * m_fFrequency;
}

void CHybridCamera::ShakeUpDown(_float _fIntensity, _float _fDuration, _float _fFrequency, _float _fDampRatio)
{
	m_fDuration = _fDuration;
	m_fIntensity = _fIntensity;
	m_fFrequency = _fFrequency;
	m_fDampRatio = _fDampRatio;
	m_pUpDownShakeTimer->SetDuration(m_fDuration);
	m_pUpDownShakeTimer->Restart();
}

void CHybridCamera::CreateShakeTimer()
{
	m_pRandomShakeTimer = m_pGameInstance->CreateTimerWithDuration(1.f, true, [=]() {
		_float _fTheta = D3DX_PI * 2.f * ((rand() % 1000) / 1000.f);
		_float _fAmplitude = ((rand() % 1000) / 1000.f);
		_fAmplitude = _fAmplitude * (1.f - 0.1f) + 0.1f;
		_fAmplitude *= (m_iRemainCnt / (m_fDuration * m_fFrequency)) * .8f + (1.f - .8f);
		m_vOffset = { m_fIntensity * sin(_fTheta), m_fIntensity * cos(_fTheta), 0.f };
		m_vOffset *= _fAmplitude;
		if (!(m_iRemainCnt--)) {
			m_iRemainCnt = 0;
			m_pRandomShakeTimer->Reset();
			m_vOffset = { 0.f, 0.f, 0.f };
		}
		}, false);

	m_pUpDownShakeTimer = m_pGameInstance->CreateTimerWithDuration(1.f, false, [=]() {
		}, false);

	m_pRoundShakeTimer = m_pGameInstance->CreateTimerWithDuration(1.f, true, [=]() {
		_float _fTheta = D3DX_PI * 2.f * m_iRemainCnt / m_fFrequency;
		_float _fAmplitude = ((rand() % 1000) / 1000.f);
		_fAmplitude = _fAmplitude * (1.f - 0.1f) + 0.1f;
		//_fAmplitude *= (m_iRemainCnt / (m_fDuration * m_fFrequency)) * .8f + (1.f - .8f);
		m_vOffset = { m_fIntensity * sin(_fTheta), m_fIntensity * cos(_fTheta), 0.f };
		m_vOffset *= _fAmplitude;
		if (!(m_iRemainCnt--)) {
			m_iRemainCnt = 0;
			m_pRoundShakeTimer->Reset();
			m_vOffset = { 0.f, 0.f, 0.f };
		}
		}, false);
}