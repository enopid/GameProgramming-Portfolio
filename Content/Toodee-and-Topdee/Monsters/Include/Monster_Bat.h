#pragma once
#include "Monster.h"

NS_BEGIN(Client)
class CMonster_Bat : public CMonster
{
	constexpr static _float BATANIMDURATION	= 1.f;
	constexpr static _float BATHEIGHT		= 6.f;

	enum EBodyPart {
		BP_BODY,
		BP_MOUTH,

		BP_LLEG,
		BP_RLEG,
		BP_LEYE,
		BP_REYE,
		BP_LEAR,
		BP_REAR,
		BP_LWING,
		BP_RWING,

		BP_END
	};
private:
	explicit CMonster_Bat(LPDIRECT3DDEVICE9 pGraphic_Deivce);
	explicit CMonster_Bat(const CMonster_Bat& Prototype);
	virtual ~CMonster_Bat() = default;

public:
	HRESULT Initialize_Prototype()				override;
	HRESULT Initialize(void* pArg)				override;
	void	Priority_Update(_float fTimeDelta)	override;
	void	Update(_float fTimeDelta)			override;
	void	Late_Update(_float fTimeDelta)		override;
public:
	void Free() override;
	static CMonster_Bat* Create(LPDIRECT3DDEVICE9 pGraphic_Deivce);
	CGameObject* Clone(void* pArg) override;
public:
	HRESULT	Ready_Components();
	virtual void OnCollision(FCollisionInfo _fCollisionInfo);
private:
	// CMonster을(를) 통해 상속됨
	void Move(_float _deltaTime) override;
	HRESULT TwoDimRender() override;
	HRESULT TopDimRender() override;
	HRESULT BodyRender();

	HRESULT SetTexture();
	HRESULT SetTransform();
private:
	CTexture*	m_pBodyTexture	= { nullptr };
	CTexture*	m_pWingTexture	= { nullptr };

	CTransform* m_pBodyTransform						= { nullptr };
	CTransform* m_pBodyPartPivotTransforms	[BP_END]	= { nullptr };
	CTransform* m_pBodyPartTextureTransforms[BP_END]	= { nullptr };
};
NS_END
 