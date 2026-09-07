#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Asset.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CEffectRoot;
NS_END

NS_BEGIN(Client)

class CGravitySphere : public CGameObject
{
	friend class CGameObject;
	struct FGRAVITYSPHEREDESC {
		_float4					m_vGravitySphereColor0			= { 0.000f, 0.000f, 0.033f, 1.0f };
		_float4					m_vGravitySphereColor1			= { 0.033f, 0.002f, 1.000f, 1.0f };

		_float2					m_vMaskUVScale0					= { 3.00f, -5.00f };
		_float2					m_vMaskUVScale1					= { 3.00f, -5.00f };
		_float2					m_vScratchUVScale				= { 1.00f, +1.00f };

		_float					m_fGravitySphereRimThreshold	= 0.11f;
		_float					m_fGravitySphereShoreDepth		= 0.22f;
		_float					m_fScrollSpeed					= 1.25f;

		_float					m_fGravitySphereAlpha			= 0.85f;
		_float					m_fShoreAlpha					= 1.f;

		_float					m_fRimIntensity0				= 0.01f;
		_float					m_fRimIntensity1				= 0.01f;
		_float					m_fLayerIntensity				= 6.6f;

		_float					m_fDisAppearTime				= 0.4f;
		_float					m_fApearTime					= 0.4f;
		_float					m_fInitScale					= 0.01f;
		_float					m_fTargetScale					= 1.0f;
	};

public:
	enum ETextureType
	{
		TT_MASK,
		TT_SCRATCH,
		TT_END
	};

public:
	CGravitySphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGravitySphere(const CGravitySphere& Prototype);
	virtual ~CGravitySphere() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Load_Asset();
	virtual HRESULT Late_Initialize();

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	virtual HRESULT Add_InitComponents();
	void SetCollidor();

	void Appear(_vector vPos, _vector _vDir, bool bUseFX);
	void Disappear();
public:
	virtual void Render_Properties();
	void	Set_ShaderParam();

public:
	virtual CGameObject* Clone(void* pArg);
	virtual HRESULT Save(void* _pDesc, _uint& _iSize) const;
	virtual HRESULT Load(void* _pDesc);
	virtual void Free() override;

private:
	WPCollidor m_wpCollidor;
protected:
	WPShader				m_wpShader;
	weak_ptr<CModel>		m_wpModel;
	weak_ptr<CMeshRenderer>	m_wpMeshRenderer;
private:
	SP_TextureAsset			m_spGravitySphereTexture[TT_END] = {};
	FGRAVITYSPHEREDESC		m_desc				= {};

	_float					m_fScrollAmount		= 0.f;
	_float					m_fScratchAmount	= 0.f;

	UniqueTimer				m_upDisappearTimer		= nullptr;
	UniqueTimer				m_upDisappearDelayTimer	= nullptr;
	UniqueTimer				m_upAppearTimer			= nullptr;

	vector<class CEnemy*>	m_vecEnemies;

	class CEffectRoot*		m_pWave = nullptr;
	class CEffectRoot*		m_pHand = nullptr;
};

NS_END