#pragma once
#include "Interact_RhythmChallenge.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CEffectRoot;
NS_END

NS_BEGIN(Client)
class CInteract_Macaron_Wall final : public CInteract_RhythmChallenge
{
	friend class CGameObject;
public:
	struct FINTERACTWALLDESC : public CInteractObject::FINTERACTOBJECTDESC {
		_float	m_fGravity			= -9.8f;
		_float	m_fSpeedDamp		= -2.f;
		_float	m_fAngularDamp		= -2.f;

		_float  m_fSleepThreshold	= 0.2f;
		_float  m_fLifeTime			= 8.f;

		_float2 m_vInitialSpeedRange = { 1.f, 5.f };
		_float2 m_vAngularSpeedRange = { 0.f, 4.f };

		_float  m_fImpulseStrength	= 1.f;
		_float  m_fImpulseFalloff	= 1.f;
		_float  m_fDirectionalBias	= 1.f;

		_float  m_fSpeedrestitution		= 1.f;
		_float  m_fAngularrestitution	= 1.f;
		_float  m_fFriction				= 0.f;
		_float  m_fAngularFriction		= 5.f;
	};
	struct DebrisInstance
	{
		_float4x4	m_Initmat			= IDENTITYMATRIX;
		_float3		m_vVelocity			= {0.f, 0.f, 0.f};
		_float3		m_vAngularVelocity	= {0.f, 0.f, 0.f}; 
		_float3		m_vInertia			= {0.f, 0.f, 0.f};
		_float		m_fMass				= 1.f;
		_float		m_fLifeTIme			= 0.f;
		_bool		m_bPosLock			= false;
	};
public:
	CInteract_Macaron_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteract_Macaron_Wall(const CInteract_Macaron_Wall& Prototype);
	virtual ~CInteract_Macaron_Wall() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Late_Initialize() override;

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	virtual void Render_Properties() override;
	void Explode();
	void ResetInfo();
public:
	virtual void	Free() override;

	CGameObject* Clone(void* pArg) override;
	HRESULT Add_InitComponents() override;

	HRESULT Save(void* _pDesc, _uint& _iSize) const override;
	HRESULT Load(void* _pDesc) override;
private:
	CGameObject*			m_pTriggerPoint = nullptr;
	CGameObject*			m_pUnbrokenWall = nullptr;
	CGameObject*			m_pBrokenWall	= nullptr;

	_uint					m_iNumDebris = 0;
	vector<CGameObject*>	m_vecDebris;
	vector<DebrisInstance>	m_vecDebrisInfos;

	FINTERACTWALLDESC		m_wallDesc;
	_bool					m_bForceReset = false;
};
NS_END