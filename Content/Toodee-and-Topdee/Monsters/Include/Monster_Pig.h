#pragma once
#include "Monster.h"

NS_BEGIN(Client)
class CMonster_Pig : public CMonster
{
	constexpr static _float TOODEEANIMDURATION = 1.f;
	constexpr static _float TOPDEEANIMDURATION = 1.f;

	enum EBodyPart {
		BP_BODY,
		BP_NOSE,
		BP_TAIL,
		BP_LEG,

		BP_LEYE,
		BP_REYE,
		BP_LEAR,
		BP_REAR,
		BP_END
	};
private:
	explicit CMonster_Pig(LPDIRECT3DDEVICE9 pGraphic_Deivce);
	explicit CMonster_Pig(const CMonster_Pig& Prototype);
	virtual ~CMonster_Pig() = default;

public:
	HRESULT Initialize_Prototype()				override;
	HRESULT Initialize(void* pArg)				override;
	void	Priority_Update(_float fTimeDelta)	override;
	void	Update(_float fTimeDelta)			override;
	void	Late_Update(_float fTimeDelta)		override;

public:
	void Free() override;
	static CMonster_Pig* Create(LPDIRECT3DDEVICE9 pGraphic_Deivce);
	CGameObject* Clone(void* pArg) override;
public:
	HRESULT	Ready_Components();
	virtual void OnCollision(FCollisionInfo _fCollisionInfo);
	virtual void OnGroundCollision(FCollisionInfo _fCollisionInfo);
	virtual HRESULT Reset() override;
private:
	// CMonster을(를) 통해 상속됨
	void Move(_float _deltaTime) override;
	HRESULT TwoDimRender() override;
	HRESULT TopDimRender() override;

	HRESULT SetTexture();
	HRESULT SetTransform();
private:

	CTexture* m_pToodeeTexture	= { nullptr };
	CTexture* m_pTopdeeTexture	= { nullptr };
	CTexture* m_pShadowTexture	= { nullptr };
	CTexture* m_pLegTexture		= { nullptr };

	CTransform* m_pToodeeTransform						= { nullptr };
	CTransform* m_pTopdeeTransform						= { nullptr };
	CTransform* m_pTopdeeShadowTransform				= { nullptr };
	CTransform* m_pBodyPartPivotTransforms	[BP_END]	= { nullptr };
	CTransform* m_pBodyPartTextureTransforms[BP_END]	= { nullptr };

	CTransform* m_pDetectorTransform = { nullptr };
	CCollider*	m_pGroundCollidor				= { nullptr };

private:
	_float m_fGravity	= -64.f;
	_float m_fYSpeed	= 0.f;
	_bool  m_bFlip		= false;
	_bool  m_bNoFloor	= false;
};
NS_END
 