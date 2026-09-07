#pragma once

#include "Client_Defines.h"
#include "NonBlock.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
class CTexture;
struct FCollisionInfo;
NS_END

NS_BEGIN(Client)

class CLightningCloud final : public CNonBlock
{
	inline static _float ZOFFSET	= -6.f;
	inline static _float XOFFSET	= 4.f;
	inline static _float RAINOFFSET = .1f;

	inline static _float CLOUDANIMDURATION		= 2.f;
	inline static _float LIGHTNINGANIMDURATION	= 1.2f;

	inline static _uint MAXTRANSPARANCEY		= 150;

private:
	explicit CLightningCloud(LPDIRECT3DDEVICE9 pGraphic_Deivce);
	explicit CLightningCloud(const CLightningCloud& Prototype);
	virtual ~CLightningCloud() = default;

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;
	void Priority_Update(_float fTimeDelta)	override;
	void Update(_float fTimeDelta)			override;
	void Late_Update(_float fTimeDelta)		override;

	HRESULT Render()						override;
	
	HRESULT RenderCloud();
	HRESULT RenderShadow();
	HRESULT RenderLightning();
	HRESULT RenderRainSplash();
private:
	HRESULT	Ready_Components();
	HRESULT SetTexture();
	HRESULT SetTransform();

	void	Calculate_RainLength();
	void	Calculate_RainDropPos();
	virtual HRESULT Reset() override;

	void OnCollision(FCollisionInfo  _fCollisionInfo);
private:
	CVIBuffer_Rect* m_pVIBufferCom					= { nullptr };

	CTexture*		m_pCloudTextureCom				= { nullptr };
	CTexture*		m_pLightningTextureCom[3]		= {	nullptr };
	CTexture*		m_pLightningMaskTextureCom		= {	nullptr };
	CTexture*		m_pTopRainSplashTextureCom[3]	= {	nullptr };
	CTexture*		m_pTooRainSplashTextureCom[3]	= {	nullptr };

	CTransform*		m_pCloudTransformCom			= { nullptr };
	CTransform*		m_pCloudTextureTransformCom		= { nullptr };
	CTransform*		m_pShadowTransformCom			= { nullptr };
	CTransform*		m_pLightningTransformCom[3]		= { nullptr };
	CTransform*		m_pLightningPivotTransformCom[3]= { nullptr };
	CTransform*		m_pRainPivotTransformCom[3]		= { nullptr };

	_float			m_fTooRainLength[3]				= { 10.f,10.f,10.f };
	_float			m_fTopRainLength[3]				= { 0.f,0.f,0.f };
	_float			m_fRatio						= { 0.f };
	EDimensionState	m_eState						= {};
	_uint			m_iTransparency					= { 0 };

	vector<wstring>					m_vecLayerMasks;

public:
	static CLightningCloud* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg);
	void Free() override;

	virtual PickingInfo* Picking(RAY tRay);
private:
	class CPSystem_Rain* m_pSystemRain[3] = {nullptr};
};

NS_END