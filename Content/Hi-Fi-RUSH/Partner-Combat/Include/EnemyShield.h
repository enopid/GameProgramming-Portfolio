#pragma once

#include "Weapon.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CModel;
class CBone;
class CMeshRenderer;
class CEffectRoot;
NS_END

NS_BEGIN(Client)
class CEnemyShield : public CWeapon
{
	enum EENEMYSHIELDVFX {
		ESV_DEBRIS,
		ESV_END
	};
	struct FWEAPONSHIELDDESC : public CWeapon::FWEAPONDESC {
		_float	m_fGravity = -9.8f;
		_float	m_fSpeedDamp = -2.f;
		_float	m_fAngularDamp = -2.f;

		_float  m_fSleepThreshold = 0.2f;
		_float  m_fLifeTime = 8.f;

		_float2 m_vInitialSpeedRange = { 1.f, 5.f };
		_float2 m_vAngularSpeedRange = { 0.f, 4.f };

		_float  m_fImpulseStrength = 1.f;
		_float  m_fImpulseFalloff = 1.f;
		_float  m_fDirectionalBias = 1.f;

		_float  m_fSpeedrestitution = 1.f;
		_float  m_fAngularrestitution = 1.f;
		_float  m_fFriction = 0.f;
		_float  m_fAngularFriction = 5.f;
	};
	struct DebrisInstance
	{
		_float4x4	m_Initmat = IDENTITYMATRIX;
		_float3		m_vVelocity = { 0.f, 0.f, 0.f };
		_float3		m_vAngularVelocity = { 0.f, 0.f, 0.f };
		_float3		m_vInertia = { 0.f, 0.f, 0.f };
		_bool		m_bPosLock = false;
	};
public:
	CEnemyShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemyShield(const CEnemyShield& Prototype);
	virtual ~CEnemyShield() = default;
public:
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Late_Initialize();


	virtual void Update(_float fTimeDelta);

	virtual void Render_Properties() override;


	HRESULT Save(void* _pDesc, _uint& _iSize) const override;
	HRESULT Load(void* _pDesc) override;
public:
	CGameObject* Clone(void* pArg) override;
public:
	void	Damage	();
	void	Hit	();
	HRESULT Reset	() override;
	bool	IsDestroy() { return iDamageState == 4; }
private:
	void	Destroy();

	void	SetBone();
	HRESULT	SetVFX();

	void Explode();

	HRESULT LoadVFX(int iEffectIdx, wstring sPrototypeTag);
	void	PlayVFX(int iEffectIdx, _vector vPos);
	void	PlayVFX(int iEffectIdx, _vector vPos, _vector vDir);
	void	PlayVFX(int iEffectIdx, _vector vDir, CBone* pBone);
	void	PlayVFX(int iEffectIdx, CBone* pBone, bool bTrack = true, bool bUseDir = true);

private:
	_int iDamageState = 0; //0: Nondamage, 3: No Panel, 4:Break
	vector<CBone*>	m_vecPanelBones;
	vector<CBone*>	m_vecPartBones;
	CBone*			m_pShieldBone	= nullptr;
	CBone*			m_pVFXBone		= nullptr;
	vector<_int>	m_vecDamageStateIndices;
	UniqueTimer		m_upVanishTimer = nullptr;

	vector<CEffectRoot*> m_vecEffects;
private:
	FWEAPONSHIELDDESC		m_shieldDesc;
	vector<DebrisInstance>	m_vecDebrisInfos;
	bool m_bExplode = false;
};
NS_END
