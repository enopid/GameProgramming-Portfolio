#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)
class CSpline : public CGameObject
{
	inline static bool BSELFDESTRUCTSEQUENCE = false;
public:
	static const int NUMSPLINESAMPLE = 32;
	struct CB_SPLINE
	{
		_float4 g_vPos		[NUMSPLINESAMPLE];
		_float4 g_vTangent	[NUMSPLINESAMPLE];
		_float4 g_vOffset	= {0.f, 0.f, 0.f, 1.f};
	};

	struct RAILOBSTACLEDESC
	{
		_int	iObtacleType	= 0;
		_float	fRailLength		= 0.f;
	};

	struct FSPLINEDESC {
		_int				m_iSampleCnt		= 50;
		_int				m_iSegmentCnt		= 10;
		_float3				m_vSegmentOffset	= { 0.f, 0.f, 0.f };
		_float				m_fSegmentInterval	= 1.f;

		_int				m_iNumObstacles		= 0;
		RAILOBSTACLEDESC	m_arrObstacles[20];

		_bool				m_bIncludeTunnel			= false;
	};
public:
	CSpline(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpline(const CSpline& Prototype);
	virtual ~CSpline() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Late_Initialize() override;

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update			(_float fTimeDelta);
	void CalcSplineDesc();
	void RebakeRenderer();
	virtual void Late_Update	(_float fTimeDelta);

	HRESULT Add_InitComponents() override;
	virtual void Render_Properties() override;

	static void SelfDestructSequence() { BSELFDESTRUCTSEQUENCE = true; }
public:
	_vector			GetPos		(_float fLength);
	_vector			GetDir		(_float fLength, _float fDelta = 0.01f);
	_float			GetMaxLength();
	const FSPLINEDESC& GetDesc() { return m_desc; };
private:
	void			CreatePath();
	void			RebakeModel();
	void			RebakeObstcle();
	void			Destroy();
public:
	virtual void	Free() override;

	CGameObject* Clone(void* pArg) override;
	HRESULT Save(void* _pDesc, _uint& _iSize) const override;
	HRESULT Load(void* _pDesc) override;
protected:
	FSPLINEDESC						m_desc				= {};
	vector<WPTransform>				m_vecWPTransforms;
	vector<_float4>					m_vecPathPoints;
	vector<pair<_float, _float>>	m_vecSamples; // t and length
	vector<CGameObject*>			m_vecSplineSegments;
	vector<CGameObject*>			m_vecObstacles;
	
	_float							m_fMaxLength = 1.f;
	vector<CB_SPLINE>				m_vecSplineDescs;
	_float4							m_vRailOffset = { 0.f, 0.f, 0.f, 1.f };
	_bool							m_bRebakeRenderer	= false;
	_bool							m_bRecalc			= false;
	class CMagnetRail*				m_pMagnetRail = nullptr;
};

NS_END
