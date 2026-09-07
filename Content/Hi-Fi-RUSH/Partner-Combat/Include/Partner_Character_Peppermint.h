#pragma once
#include "Partner.h"
#include "Magazine.h"

NS_BEGIN(Client)
class CPartner_Character_Peppermint final : public CPartner
{
	enum ETARGET {
		NONE,
		SWITCH,
		SHIELD
	};
	enum EWEAPON {
		UNARM,
		HANDGUN,
		BLASTER
	};
public:
	CPartner_Character_Peppermint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartner_Character_Peppermint(const CPartner_Character_Peppermint& Prototype);
	virtual ~CPartner_Character_Peppermint() = default;
public:
	virtual HRESULT Initialize_Prototype(void* pArg);
	virtual HRESULT Initialize(void* pDesc);
	virtual HRESULT Late_Initialize() override;

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	virtual void    Add_Listener() override;
public:
	CGameObject* Clone(void* pArg) override;
	virtual void	Free() override;
	virtual HRESULT	Reset() override;
public:
	HRESULT Save(void* _pDesc, _uint& _iSize) const override;
	HRESULT Load(void* _pDesc) override;
	HRESULT Add_InitComponents() override;

	void	CheckOnTarget();
public:
	HRESULT Load_Config();

	void Update_PB(_float fTimeDelta);
	void Update_PN(_float fTimeDelta);

	void Update_PG_CTX(_float fTimeDelta);
	void Update_PB_CTX(_float fTimeDelta);
	void Update_PN_CTX(_float fTimeDelta);

private:
	//Shot
	void Shot_Handgun(bool bIsLeft, bool bIsGimmic);
	RAY Get_HandgunRay(bool bIsLeft);
	//Idle
	void Compute_Angle();
	void Set_Ratio();
	void Set_TurnValue();
	
	//Idle
	void Set_Weapon( );
	void ActiveWeapon(EWEAPON eWeaponTag);
private:
	UniqueMagazine	m_upMagazine = nullptr;


	_float	m_fYaw			= 0.f; 
	_float	m_fPitch		= 0.f; 
	_int	m_iTurnSide		= 0;
	ETARGET	m_eTarget		= ETARGET::NONE;

	class CWeapon*  m_pShotGun = nullptr;
	class CWeapon*	m_pBlaster = nullptr;

	CEffectRoot* m_pBeamEffect		= nullptr;
	CEffectRoot* m_pBeamHitEffect	= nullptr;

	// CPartner을(를) 통해 상속됨
	_bool OnDamage(FDamageInfo _fDamageInfo) override;

	// CPartner을(를) 통해 상속됨
	HRESULT InitFX() override;
};
NS_END
