#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Asset.h"
#include "Time_Manager_API.h"

NS_BEGIN(Client)

class CEnemyBarrier : public CGameObject
{
	friend class CGameObject;
	struct FBARRIERDESC {
		_float4					m_vBarrierColor0		= { 0.108f, 0.525f, 0.866f, 1.0f };
		_float4					m_vBarrierColor1		= { 0.015f, 0.315f, 0.820f, 1.0f };
		_float4					m_vBarrierColor2		= { 0.711f, 0.782f, 0.992f, 1.0f };

		_float4					m_vBarrierShoreColor0	= { 4.000f, 4.000f, 4.000f, 1.0f };
		_float4					m_vBarrierShoreColor1	= { 0.357f, 0.386f, 0.916f, 1.0f };

		_float2					m_vMaskUVScale			= { 15.98f, 10.95f };
		_float2					m_vNoiseUVScale			= { 5.0f, 5.0f };
		_float2					m_vScratchUVScale		= { 5.0f, 3.2f };
		_float2					m_vShoreNoiseUVScale	= { 5.0f, 5.0f };

		_float					m_fBarrierRimThreshold0 = 0.84f;
		_float					m_fBarrierRimThreshold1 = 0.56f;
		_float					m_fBarrierShoreDepth	= 0.22f;
		_float					m_fShoreNoiseSpeed		= 1.25f;
		_float					m_fNoiseSpeed			= 1.25f;

		_float					m_fBarrierAlpha			= 0.5f;
		_float					m_fShoreAlpha			= 1.f;

		_float					m_fNoiseIntensity		= 0.01f;
		_float					m_fShoreIntensity		= 0.7f;
		_float					m_fShoreNoiseIntensity	= 0.05f;
		_float					m_fDissolveTime			= 0.4f;
	};

public:
	enum EColType
	{
		Barrier,
		Body,
		Col_Type_End
	};
	enum ETextureType
	{
		TT_MASK0,
		TT_MASK1,
		TT_NOISE,
		TT_SCRATCH,
		TT_END
	};

public:
	CEnemyBarrier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemyBarrier(const CEnemyBarrier& Prototype);
	virtual ~CEnemyBarrier() = default;

public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Load_Asset();
	virtual HRESULT Late_Initialize();

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	virtual HRESULT Add_InitComponents();

	void Initialize_BarrierGauge(_float fMaxGauge);
	void SetCollidor();
	void BarrierRecovery();
	void BarrierBroken();

	void OnDamage(_float fDamage);

public:
	virtual void Render_Properties();
	void	Set_ShaderParam();

public:
	virtual CGameObject* Clone(void* pArg);
	virtual HRESULT Save(void* _pDesc, _uint& _iSize) const;
	virtual HRESULT Load(void* _pDesc);
	virtual void Free() override;

private:
	_float m_fMaxBarrierGauge = 0.f;
	_float m_fCurBarrierGauge = 0.f;

	WPCollidor m_wpCollidor[Col_Type_End];
protected:
	WPShader				m_wpShader;
	weak_ptr<CModel>		m_wpModel;
	weak_ptr<CMeshRenderer>	m_wpMeshRenderer;
private:
	SP_TextureAsset			m_spBarrierTexture[TT_END] = {};
	FBARRIERDESC			m_desc				= {};
	_float					m_fBarrierAmount	= 0.f;
	_float					m_fScratchAmount	= 0.f;
	UniqueTimer				m_upDissolveTimer	= nullptr;

	_bool					m_bIsBroken			= false;
};

NS_END