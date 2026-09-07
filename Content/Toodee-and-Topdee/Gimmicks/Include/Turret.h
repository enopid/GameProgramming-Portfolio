#pragma once

#include "Client_Defines.h"
#include "NonBlock.h"
#include "Time_Manager_API.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

enum class ETURRETTYPE {
	FIREPELLET,
	LASER,
	DIAGLASER
};

class CTurret final : public CNonBlock
{
	constexpr static _float SCALEFREQUENCY	= 3.f;
	constexpr static _float SCALERATIO		= 2.f;
	constexpr static _uint	MAXNUMFIREPELLETS	= 20;

private:
	explicit CTurret(LPDIRECT3DDEVICE9 pGraphic_Deivce, ETURRETTYPE _turretType);
	explicit CTurret(const CTurret& Prototype);
	virtual ~CTurret() = default;

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;
	void Priority_Update(_float fTimeDelta)	override;
	void Update(_float fTimeDelta)			override;
	void Late_Update(_float fTimeDelta)		override;
	HRESULT Render()						override;

private:
	HRESULT	Ready_Components();

private:
	CVIBuffer_Rect* m_pVIBufferCom	= { nullptr };

public:
	static CTurret* Create(LPDIRECT3DDEVICE9 pGraphic_Device, ETURRETTYPE _TURRETTYPE);
	virtual Engine::CGameObject* Clone(void* pArg);
	void Free() override;

	virtual PickingInfo* Picking(RAY tRay);
private:
	void ShotFirePellet();
	void ShotLaser();

	void AdjustTurretScale();
	void AdjustLaserScale();

	HRESULT CreateFirePelletPool(PairInfo pairInfo);
	HRESULT CreateLaser(PairInfo pairInfo, bool _bIsDiag = false);
private:
	const ETURRETTYPE m_turretType;
	_uint		m_iCurDIr = {0};	//up, down , left, right

	CTransform* m_pPivotTransformCom1	= { nullptr };
	CTransform* m_pPivotTransformCom2	= { nullptr };
	CTexture*	m_pLaserTexture			= { nullptr };

	UniqueTimer m_pScaleTimer			= { nullptr };

	class CFirePellet*	m_arrpFirePellets[MAXNUMFIREPELLETS] = {nullptr};
	class CLaser*		m_pLaser = {nullptr};
	_float	m_fLaserDist = { 0.f };
private:
	class CPSystem_Dust* m_pSystemDust = { nullptr };
};

NS_END