#pragma once
#include "Projectile.h"

NS_BEGIN(Client)

class CProjectile_Peppermint_Bullet final : public CProjectile
{
	friend	CGameObject;
	enum VFX { BULLET, MUZZLE, HIT, END };
public:
	CProjectile_Peppermint_Bullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Peppermint_Bullet(const CProjectile_Peppermint_Bullet& Prototype);
	virtual ~CProjectile_Peppermint_Bullet() = default;
public:
	virtual HRESULT Initialize(void* pDesc)	override;
	virtual HRESULT Late_Initialize()		override;

	virtual void	Update(_float fTimeDelta)	override;

	virtual void	Shot(RAY ray) override;
public:
	CGameObject* Clone(void* pArg) override;
	HRESULT Save(void* _pDesc, _uint& _iSize) const override;
	HRESULT Load(void* _pDesc) override;

	virtual void	Vanish() override;
private:
	CEffectRoot* m_pEffect[END] = {};
};

NS_END

